require_relative '../../environment.rb'

class AddPartNumToTask < ActiveRecord::Migration
  def change
    add_column :tasks, :part_num, :integer
  end
end

AddPartNumToTask.migrate(:up)
