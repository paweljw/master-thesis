require_relative '../../environment.rb'

class AddGlobalSizeToTask < ActiveRecord::Migration
  def change
    add_column :tasks, :global_size, :integer
  end
end

AddGlobalSizeToTask.migrate(:up)
