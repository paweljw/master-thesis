require_relative './backend_base'

module Backend
  class FakeBackend < Backend::Base
      def run(opts = {})
        logger = opts[:logger]
        platform = opts[:platform]
        device = opts[:device]

        10.times { |f| logger.info "Backend on device #{device} firing #{f} time"; sleep 3 }
      end

      def opencl
        false
      end
  end
end
