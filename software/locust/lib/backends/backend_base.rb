module Backend
  class Base
    attr_accessor :platform, :device, :input, :type

    def self.opencl?
      true
    end

    def run

    end

    def report
      Backend::Report(self)
    end
  end
end
