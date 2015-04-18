class AddGlobalSizeToTasks < ActiveRecord::Migration
  def change
    add_column :tasks, :global_size, :integer
  end
end
