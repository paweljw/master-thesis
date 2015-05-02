#!/usr/bin/env ruby

require './environment'

# OPTIONS
options = { loop: true }

OptionParser.new do |opts|
  opts.banner = "Usage: locust.rb [options]"

  opts.on("-q", "--quiet", "Run quietly") do |v|
    options[:quiet] = true
  end

  opts.on("--no-loop", "Don't start looping") do |v|
    options[:loop] = false
  end
end.parse!

# LOGGING
log_file = File.open(File.join( LOCUST_ROOT, "log/locust.log" ), "a")
logger = Logger.new MultiIO.new(log_file) if options[:quiet]
logger = Logger.new MultiIO.new(STDOUT, log_file) unless options[:quiet]
logger.info 'Starting Locust'

# CONNECTION SETUP
server_key = LOCUST_CONFIG['server']['key']
heartbeat_uri = URI.join(LOCUST_CONFIG['server']['address'], '/nodes/heartbeat')
heartbeat_delay = LOCUST_CONFIG['client']['heartbeat'].to_i
tasks_requisition_uri = URI.join(LOCUST_CONFIG['server']['address'], '/tasks.json')
tasks_return_uri = URI.join(LOCUST_CONFIG['server']['address'], '/tasks/')

# PLATFORM SETUP

## OpenCL detection

has_opencl = true

opencl_platforms = []

begin
  logger.info "Enumerating OpenCL platforms, this will take a while"
  platforms = OpenCL.platforms

  platforms.each_with_index do |p, i|
    logger.info "Found OpenCL platform #{i}: #{p.name}"

    p.devices.each_with_index do |d, j|
      logger.info "--- Found device #{j}: #{d.name}"
      opencl_platforms << "#{i}:#{j}"
    end
  end
rescue OpenCL::Error => e
  has_opencl = false
  logger.warn "OpenCL setup failed. Falling back to CPU. (#{e.message})"
end

unless options[:loop]
  logger.info "Exiting because locust was called with --no-loop"
  exit
end

thread_array = []

loader_location = LOCUST_CONFIG['client']['loader_location']

semaphore = Mutex.new

if has_opencl
  platforms = OpenCL.platforms

  platforms.each_with_index do |p, i|
    p.devices.each_with_index do |d, j|
      Thread.new { Backend::PjwBackend.new.run(logger: logger, platform: i, device: j, loader_location: loader_location, semaphore: semaphore) }
    end
  end
else
  threads = LOCUST_CONFIG['client']['no_opencl_threads'].to_i

  threads.times do |i|
    Thread.new { Backend::FakeBackend.new.run(logger: logger, platform: 0, device: i) }
  end
end

logger.info "Connecting to swarm host @ #{URI.join(LOCUST_CONFIG['server']['address'])}..."
logger.info "Joining the swarm..."

Task.where(state: 5).update_all state: 4

logger.info "Restarted crashed tasks"

begin
  while true do
    # HEARTBEAT
    begin
      logger.info 'Sending heartbeat...'
      response = RestClient.get heartbeat_uri.to_s, { params: { auth: server_key }}
      logger.info "Server responded #{response}"
      heartbeat_delay = LOCUST_CONFIG['client']['heartbeat'].to_i
    rescue Exception => e
      logger.fatal "Server appears to be down (#{e}), delaying communication"
      heartbeat_delay = 60
    end

    # logger.info 'Requisition step'

    # TASK REQUISITION
    if Task.undone.count < LOCUST_CONFIG['client']['store_tasks'].to_i
      begin
        needed_tasks = LOCUST_CONFIG['client']['store_tasks'].to_i - Task.undone.count

        logger.info "Requisitioning #{needed_tasks} more tasks..."
        response = RestClient.get tasks_requisition_uri.to_s, { params: { auth: server_key, num: needed_tasks }}

        remote_tasks = JSON.parse(response, symbolize_names: true)

        remote_tasks.each do |t|
          t_attrs = { swarmhost_id: t[:id], kind: t[:kind], state: 3, name: t[:name],
            remote_url: t[:metafile][:url], part_size: t[:part_size], global_size: t[:global_size], part_num: t[:part_num] }
          task = Task.create(t_attrs)
          logger.info "Created task with id #{task.id} from Swarmhost task #{t[:id]}"
        end

        logger.info "Server returned #{remote_tasks.count} task(s)"
      rescue Exception => e
        logger.fatal "Error during task requisition (#{e})"
      end
    end

    # TASK PROVISIONING
    
    # logger.info 'provisioning step'

# logger.info "Tasks for provisioning: #{Task.for_provisioning.count}"

    Task.for_provisioning.each do |f|
      begin
        logger.info "Provisioning task #{f.id}"
        remote_url = URI.join(LOCUST_CONFIG['server']['address'], f.remote_url)

        response = RestClient.get remote_url.to_s
        storage_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{f.id.to_s}.gz")
        unzipped_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{f.id.to_s}.mcx")

        mtx_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{f.id.to_s}.mtx")
        rhs_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{f.id.to_s}.rhs")

        File.open( storage_path, 'w' ) { |file| file.write response }

        Zlib::GzipReader.open(storage_path) do |gz|
          gz = gz.read
          File.open( unzipped_path, 'w' ) { |file| file.write(gz) }
        end

        File.delete(storage_path)

        mtx_file = File.open(mtx_path, 'w')
        rhs_file = File.open(rhs_path, 'w')

        lines = 0

        ary = File.readlines(unzipped_path)
        rhs_line = ary[0]

        rhs_file.write("#{f.global_size} 1\n")

        rhs_line.split(" ").each { |l| rhs_file.write("#{l}\n")}

        rhs_file.close

        nonzero = 0
        counting = false

        ary.each do |line|
          nonzero += 1 if counting
          counting = false and nonzero -= 1 if line.strip == "%" && counting
          counting = true if line.strip == "%" && !counting
        end

        mtx_file.write("#{f.global_size} #{f.part_size} #{nonzero}\n")

        counting = false

	offset = f.offset

        ary.each do |line|
          if counting && line.strip != "%"
	   arln = line.strip.split(" ")
           arln[0] = (arln[0].to_i - offset.to_i).to_s
           mtx_file.write("#{arln.join(" ")}\n")
	  end

          counting = false and nonzero -= 1 if line.strip == "%" && counting
          counting = true if line.strip == "%" && !counting
        end

        mtx_file.close

        f.update(state: 4) # task is provisioned and ready for operations
      rescue Exception => e
        logger.fatal "Server appears to be down (#{e}), delaying communication"
	logger.info e.backtrace
      end
     end

      # logger.info "Upload should happen here"

      Task.for_upload.each do |f|
        begin
          logger.info "Uploading task #{f.id}"

          url = URI.join(tasks_return_uri, "#{f.swarmhost_id}")

          response = RestClient.put(url.to_s,
            task: {
              metafile: File.new(File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{f.id.to_s}.ret.gz"), 'rb'),
              state: 'complete',
              completed: DateTime.now,
              time: f.time,
              backend: f.backend
            },
	    auth: server_key )

            f.update(state: 7) # has been sent. Good bye, task!
        rescue Exception => e
          logger.fatal "Server appears to be down (#{e}), delaying communication"
        end
      end

    sleep heartbeat_delay
  end
rescue SignalException => e
  logger.info "Graceful exit begin"

  # ActiveRecord::Base.verify_active_connections!

  logger.info "Graceful exit end"
  exit
end
