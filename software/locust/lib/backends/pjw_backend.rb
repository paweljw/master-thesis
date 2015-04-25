require_relative './backend_base'
require_relative '../../environment.rb'

module Backend
  class PjwBackend < Backend::Base
=begin
    def run(opts = {})
      logger = opts[:logger]
      platform = opts[:platform]
      device = opts[:device]

      # load task
      t = Task.for_running.limit(1)
      t.update(state: 5)

      mtx_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.mtx")
      rhs_path = File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.rhs")

      offset = 0 # zmiana logiki ma razie

      cmd = "./mtx_loader #{mtx_file} #{rhs_file} 192 0 #{platform} #{device} #{offset}"
      ret = `#{cmd}`

      File.open(File.join(LOCUST_CONFIG['client']['storage_dir'], "tasks", "#{t.id.to_s}.ret")).do |f|
        f.write ret
      end
    end

    def opencl
      true
    end
=end
  end
end
