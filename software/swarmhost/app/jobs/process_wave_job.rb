class ProcessWaveJob < ActiveJob::Base
  require 'zlib'
  queue_as :default

  def partnum(v, ps)
    v = v.to_i
    ps = ps.to_i
    partnum = 0

    while v > ps
      v -= ps
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

    # create an array of parts
    parts = []

    ps = wave.solution.part_size
    fs = wave.solution.dim

    num_parts = fs.to_i / ps.to_i

    num_parts.times { |i| parts[i] = [] }

    reduction_map.each do |r| # r is an array here
      canonical = r.shift
      origin_part_number = partnum(canonical, ps)
      r.each do |rr|
        remote_part_number = partnum(rr, ps)
        parts[ origin_part_number ][ remote_part_number ] = [] if parts[ origin_part_number ][ remote_part_number ].nil?
        parts[ origin_part_number ][ remote_part_number ] << { canonical: canonical, reduced: rr}
      end
    end

    parts.each_with_index do |e, i|
      p "Part #{i}"
      e.each_with_index do |p, i|
        p "+++ vs part #{i}: #{p}" unless p.blank?
      end
    end
  end
end
