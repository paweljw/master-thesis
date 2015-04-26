class ProcessWaveJob < ActiveJob::Base
  require 'zlib'
  queue_as :default

  def partnum(v)
    partnum = 0

    while v > 1024
      v -= 1024
      partnum += 1
    end

    partnum
  end

  def perform(wave)
    reduction_map = []
    wave.tasks.each do |t|
      that_line = nil

      Zlib::GzipReader.open(t.metafile.path) do |gz|
        cnt = 0
        gz.each_line do |l|
          (that_line = l) && cnt += 1 if cnt == 2
          break if cnt == 3
          cnt += 1 if l.strip == "%"
        end
      end

      that_line.strip.split(" ").each do |e|
        e = e.strip.split(":")
        reduction_map[ e[0].to_i ] = [] if reduction_map[ e[0].to_i ] == nil
        reduction_map[ e[0].to_i ] << e[1].to_i
      end
    end

    reduction_map = reduction_map.select { |v| v != nil && v.try(:size) > 1 }

    reduction_map.each do |l|
      flag = false
      first = l.shift
      l.each { |x| flag = true if partnum(x) != partnum(first) }
      l = [first].concat(l)
      puts "Not in same part: #{l}" if flag
    end
  end
end
