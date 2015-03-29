class AddCompletedToTasks < ActiveRecord::Migration
  def change
    add_column :tasks, :completed, :datetime
  end
end
