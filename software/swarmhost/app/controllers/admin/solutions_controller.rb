class Admin::SolutionsController < Admin::ApplicationController
  include CrudController

  def create
    super

    wave = @resource.waves.build
    wave.state = :unpublished
    wave.save!

    splitter_path = File.join( Rails.root, "bin", "splitter" )
    storage_dir = File.join( Rails.root, Solution::STORAGE_DIR )
    tmp_dir = File.join( Rails.root, Solution::TMP_DIR )

    description_file = "solution_descr_#{@resource.name}"
    description_path = File.join( Rails.root, Solution::STORAGE_DIR, description_file)

    cmd = "#{splitter_path} #{@resource.mtx.path} #{@resource.rhs.path} #{@resource.part_size.to_i} #{storage_dir}/ #{tmp_dir}/"
    ret = `#{cmd}`

    description = File.open(description_path)

    description.each_with_index do |f, i|
      f.strip!
      puts f
      t = wave.tasks.build
      t.name = File.basename f
      t.kind = 1
      t.state = :ready
      t.part_num = i
      t.metafile = File.open f
      t.save!

      File.delete f
    end

    File.delete description_path

    wave.update state: 1
    @resource.update state: 1
  end

  def resource_class
    Solution
  end
end
