class RemoveTasksFromWaves < ActiveRecord::Migration
  def change
    remove_column :waves, :tasks, :integer
  end
end
