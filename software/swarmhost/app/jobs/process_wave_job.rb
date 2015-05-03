class ProcessWaveJob < ActiveJob::Base
  require 'zlib'
  queue_as :default

  def partnum(v, ps)
    _v = v.to_i + 1
    _ps = ps.to_i
    partnum = 0

    while _v > _ps
      _v -= _ps
      partnum += 1
    end

    partnum
  end

  def perform(wave)
    reduction_map = []

    part_size_is = 0
    global_size_is = wave.solution.dim.to_i
    reductions = 0

    wave.tasks.each do |t|
      part_size_is = t.part_size.to_i if part_size_is == 0
      that_line = nil

      Zlib::GzipReader.open(t.metafile.path) do |gz|
        cnt = 0
        gz.each_line do |l|
          (that_line = l) && cnt += 1 if cnt == 2
          break if cnt == 3
          cnt += 1 if l.strip == "%"
        end
      end

      unzipped_path = File.join(Rails.root, "/tmp", "#{wave.id}_part_#{t.part_num}.mcx")

      Zlib::GzipReader.open(t.metafile.path) do |gz|
        gz = gz.read
        File.open( unzipped_path, 'w' ) { |file| file.write(gz) }
      end

      that_line.strip.split(" ").each do |e|
        e = e.strip.split(":")
        reduction_map[ e[0].to_i ] = [] if reduction_map[ e[0].to_i ].blank?
        reduction_map[ e[0].to_i ] << e[1].to_i
      end
    end

    new_reduction_map = []

    (0..global_size_is).each do |i|
      unless reduction_map[i].blank? || reduction_map[i].size == 1
        reductions += 1
        new_reduction_map << reduction_map[i]
      end
    end

    wave.update reductions: reductions

    parts = []

    ps = wave.solution.part_size
    fs = wave.solution.dim

    num_parts = fs.to_i / ps.to_i

    num_parts.times { |i| parts[i] = [] }

    new_reduction_map.each do |r| # r is an array here
      canonical = r.shift
      origin_part_number = partnum(canonical, ps)
      r.each do |rr|
        remote_part_number = partnum(rr, ps)
        parts[ origin_part_number ][ remote_part_number ] = [] if parts[ origin_part_number ][ remote_part_number ].nil?
        parts[ origin_part_number ][ remote_part_number ] << { canonical: canonical, reduced: rr}
      end
    end

    start_new_wave = false

    parts.each_with_index do |e, i|
      offset_origin = part_size_is * i

      e.each_with_index do |p, j|
        # reducer part_origin part_offender part_size part_number_origin part_number_offender global_size
        unless p.blank?
          start_new_wave = true

          # write out a file with pairings
          # which is f***ng ugly as f**k
          # but I ain't got the time for Open3

          command_path = File.join(Rails.root, "/tmp", "#{wave.id}_reduce_#{i}_#{j}.txt")
          offset_offender = part_size_is * j

          File.open(command_path, 'w') do |file|
            p.map { |x| x.values }.each do |pair|
              file.write("#{pair[0].to_i - offset_origin} #{pair[1].to_i - offset_offender}\n")
            end
            file.write("%\n")
          end

          reducer_path = File.join(Rails.root, "bin", "reducer")
          origin_path = File.join(Rails.root, "/tmp", "#{wave.id}_part_#{i}.mcx")
          offender_path = File.join(Rails.root, "/tmp", "#{wave.id}_part_#{j}.mcx")

          command = "#{reducer_path} #{origin_path} #{offender_path} #{part_size_is} #{i} #{j} #{global_size_is} < #{command_path}"
          ret = `#{command}`

          new_pack = File.join(Rails.root, "tmp", "#{wave.id}_part_#{j}.mcx") # write to THE SAME file just in case

          File.open(new_pack, 'w') do |file|
            file.write(ret)
          end

          File.delete command_path
        end
      end
    end

    wave.update completed: DateTime.now, state: 2

    if start_new_wave
      new_wave = wave.solution.waves.build
      new_wave.state = 0
      new_wave.seq = wave.seq + 1
      new_wave.save!

      tasks = 0
      Dir.glob(File.join(Rails.root, "tmp", "#{wave.id}_part_*.mcx")).sort.each do |f|

        Zlib::GzipWriter.open(f + '.gztmp') do |gzfile|
          File.open(f) do |partfile|
            # process the file while rewriting to make sure no maps/timing info remains
            percents = 0
            partfile.each_line do |line|
              percents += 1 if line.strip == '%'
              gzfile.write(line) unless percents > 1
              break if percents > 1
            end
          end
          gzfile.close
        end

        FileUtils.mv(f + '.gztmp', f)

        t = new_wave.tasks.build
        t.name = File.basename f
        t.kind = 1
        t.state = 1
        t.part_num = f.split('_part_').last.scan(/\d/).join.to_i
        t.metafile = File.open f
        t.save!
        tasks += 1

        File.delete f
      end

      new_wave.update state: 1, tasks_number: tasks
    else
      wave.solution.update state: 2, completed: DateTime.now
    end
  end
end
