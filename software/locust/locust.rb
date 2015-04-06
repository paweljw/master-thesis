#!/usr/bin/env ruby

require './environment'

# OPTIONS
options = {}
OptionParser.new do |opts|
  opts.banner = "Usage: locust.rb [options]"

  opts.on("-q", "--quiet", "Run quietly") do |v|
    options[:quiet] = true
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
          t_attrs = { swarmhost_id: t[:id], kind: t[:kind], state: 3, name: t[:name], remote_url: t[:metafile][:url] }
          task = Task.create(t_attrs)
          logger.info "Created task with id #{task.id} from Swarmhost task #{t[:id]}"
        end

        logger.info "Server returned #{remote_tasks.count} task(s)"
      rescue Exception => e
        logger.fatal "Error during task requisition (#{e})"
      end
    end

    # TASK PROVISIONING

    Task.for_provisioning.each do |f|
      begin
        logger.info "Provisioning task #{f.id}"
        remote_url = URI.join(LOCUST_CONFIG['server']['address'], f.remote_url)

        response = RestClient.get remote_url.to_s
        storage_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{f.id.to_s}.gz")
        unzipped_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{f.id.to_s}.mcx")

        File.open( storage_path, 'w' ) { |file| file.write response }

        Zlib::GzipReader.open(storage_path) do |gz|
          gz = gz.read
          File.open( unzipped_path, 'w' ) { |file| file.write(gz) }
        end

        File.delete(storage_path)

        f.update(state: 4) # task is provisioned and ready for operations
      rescue Exception => e
        logger.fatal "Server appears to be down (#{e}), delaying communication"
      end

    end

    sleep heartbeat_delay
  end
rescue SignalException => e
  p "Graceful exit begin"

  p "Graceful exit end"
  exit
end
