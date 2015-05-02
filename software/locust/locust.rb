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

Task.where("state = 5 OR state = 4").update_all(state: 3)

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

    sleep heartbeat_delay
  end
rescue SignalException => e
  logger.info "Graceful exit begin"
  logger.info "Graceful exit end"
  exit
end
