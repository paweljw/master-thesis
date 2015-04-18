class AddPartSizeToTask < ActiveRecord::Migration
  def change
    add_column :tasks, :part_size, :integer
  end
end
