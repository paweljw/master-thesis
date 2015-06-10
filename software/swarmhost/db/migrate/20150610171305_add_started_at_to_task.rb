class AddStartedAtToTask < ActiveRecord::Migration
  def change
    add_column :tasks, :started_at, :datetime
  end
end
