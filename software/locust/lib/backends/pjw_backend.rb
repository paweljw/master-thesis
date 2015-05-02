require_relative './backend_base'
require_relative '../../environment.rb'

require 'zlib'

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
        sleep 3

        t = nil

        sem.synchronize do
          # p "Backend #{platform}:#{device} enters semaphore"
          break if Task.for_doing.count == 0

          begin
            t = Task.for_doing.limit(1).first
            t = nil unless t.update(state: 5)
          rescue Exception => e
            logger.warn "Exception during looking for work: #{e}"
          end

          # p "Backend #{platform}:#{device} leaves semaphore"
        end


        # p "t is now #{t.inspect} for backend #{platform}:#{device}"

        if !t.nil?
          begin
            logger.info "Task #{t.id} is starting on #{platform}:#{device}"

            mtx_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.mtx")
            rhs_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.rhs")

            offset = t.offset

            # logger.info "Ok, now we're starting for real"

            cmd = "#{loc} #{mtx_path} #{rhs_path} 256 #{t.part_size} #{platform} #{device} #{offset}"

            logger.info "Cmd is #{cmd}"
            ret = `#{cmd}`

            rsplt = ret.split(/\r?\n/).select { |l| l.include? "TIME" }

            # logger.info "rsplat: #{rsplt}"
            time = rsplt[0].split(" ")[1].to_f

            # logger.info "Time: #{time}"

            logger.info "Task #{t.id} finishing on #{platform}:#{device}"

            File.open(File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.ret.gz"), 'w') do |f| 
	      gz = Zlib::GzipWriter.new(f)
	      gz.write ret
              gz.close
	    end

            t.update state: 6, backend: "Platform #{platform} device #{device}", time: time
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
