require_relative '../../environment.rb'

class AddPartSizeToTask < ActiveRecord::Migration
  def change
    add_column :tasks, :part_size, :integer
  end
end

AddPartSizeToTask.migrate(:up)
