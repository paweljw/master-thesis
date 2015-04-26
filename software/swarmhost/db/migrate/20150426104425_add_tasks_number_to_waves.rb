class AddTasksNumberToWaves < ActiveRecord::Migration
  def change
    add_column :waves,  :tasks_number, :integer
  end
end
