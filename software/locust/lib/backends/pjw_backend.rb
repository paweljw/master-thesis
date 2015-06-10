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

      logger.info "OpenCL backend starting @ #{platform}:#{device}"

      tasks_return_uri = URI.join(LOCUST_CONFIG['server']['address'], '/tasks/')
      server_key = LOCUST_CONFIG['server']['key']

      while true
        sleep 1

        t = nil

        sem.synchronize do
          break if Task.for_provisioning.count == 0

          begin
            t = Task.for_provisioning.limit(1).first
            t = nil unless t.update(state: 4)
          rescue Exception => e
            logger.warn "Exception during looking for work: #{e} @ #{platform}:#{device}"
          end
        end

        if !t.nil?
          begin
            logger.info "Provisioning task #{t.id} @ #{platform}:#{device}"
            remote_url = URI.join(LOCUST_CONFIG['server']['address'], t.remote_url)

            response = RestClient.get remote_url.to_s
            storage_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.gz")
            unzipped_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.mcx")

            mtx_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.mtx")
            rhs_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.rhs")

            File.open( storage_path, 'w' ) { |file| file.write response }

            Zlib::GzipReader.open(storage_path) do |gz|
              gz = gz.read
              File.open( unzipped_path, 'w' ) { |file| file.write(gz) }
            end

            File.delete(storage_path)

            mtx_file = File.open(mtx_path, 'w')
            rhs_file = File.open(rhs_path, 'w')

            ary = File.readlines(unzipped_path)
            rhs_line = ary[0]

            rhs_file.write("#{t.global_size} 1\n")

            rhs_line.split(" ").each { |l| rhs_file.write("#{l}\n")}

            rhs_file.close

            nonzero = 0
            counting = false

            ary.each do |line|
              nonzero += 1 if counting
              counting = false and nonzero -= 1 if line.strip == "%" && counting
              counting = true if line.strip == "%" && !counting
            end

            mtx_file.write("#{t.global_size} #{t.part_size} #{nonzero}\n")

            counting = false

    	      offset = t.offset

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

            t.update(state: 4) # task is provisioned and ready for operations

            logger.info "Task #{t.id} is starting @ #{platform}:#{device}"

            mtx_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.mtx")
            rhs_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.rhs")

            offset = t.offset

            cmd = "#{loc} #{mtx_path} #{rhs_path} #{t.local_size} #{t.part_size} #{platform} #{device} #{offset}"

            t.update(state: 5)

            ret = `#{cmd}`

            rsplt = ret.split(/\r?\n/).select { |l| l.include? "TIME" }

            time = rsplt[0].split(" ")[1].to_f

            logger.info "Task #{t.id} finishing @ #{platform}:#{device}"

            File.open(File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.ret.gz"), 'w') do |f|
      	      gz = Zlib::GzipWriter.new(f)
      	      gz.write ret
              gz.close
            end

            t.update state: 6, backend: "Platform #{platform} device #{device}", time: time

            logger.info "Uploading task #{t.id} @ #{platform}:#{device}"

            url = URI.join(tasks_return_uri, "#{t.swarmhost_id}")

            response = RestClient.put(url.to_s,
              task: {
                metafile: File.new(File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.ret.gz"), 'rb'),
                state: 'complete',
                completed: DateTime.now,
                time: f.time,
                backend: f.backend
              },
              auth: server_key )

              t.update(state: 7) # has been sent. Good bye, task!

              logger.info "Completed lifecycle for task #{t.id} @ #{platform}:#{device}"
          rescue Exception => e
            logger.warn "Exception! #{e}"
            logger.warn e.backtrace

            t.update! state: 3
          end
        end
      end
    end

    def opencl
      true
    end
  end
end
