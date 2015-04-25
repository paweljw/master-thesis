require_relative './backend_base'
require_relative '../../environment.rb'

module Backend
  class PjwBackend < Backend::Base
    def run(opts = {})
      logger = opts[:logger]
      platform = opts[:platform]
      device = opts[:device]
      loc = opts[:loader_location]
      sem = opts[:semaphore]

      logger.info "A backend is starting on platform #{platform}, device #{device}"

      while true
        sleep 1

	t = nil

        sem.synchronize do
          # p 'in semaphore'
          return if Task.for_doing.count == 0

          # p 'got some job'
          t = Task.for_doing.limit(1).first
          t.update! state: 5
	end

	# p 't is not nil!' unless t.nil?

        if !t.nil?
	  begin
            logger.info "Task #{t.id} is starting on #{platform}:#{device}"

            mtx_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.mtx")
            rhs_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.rhs")

            offset = t.offset

            # logger.info "Ok, now we're starting for real"

            cmd = "#{loc} #{mtx_path} #{rhs_path} 256 #{t.part_size} #{platform} #{device} #{offset}"

            # logger.info "Cmd is #{cmd}"
            ret = `#{cmd}`

	    logger.info "Task #{t.id} finishing on #{platform}:#{device}"

            File.open(File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.ret"), 'w') { |f| f.write ret }
            t.update state: 6	  
         rescue Exception => e
           logger.warn "Exception! #{e}"
           logger.warn e.backtrace

           t.update! state: 4
         end
        end
      end
    end

    def opencl
      true
    end
  end
end
