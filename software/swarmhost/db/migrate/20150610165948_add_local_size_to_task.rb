class AddLocalSizeToTask < ActiveRecord::Migration
  def change
    add_column :tasks, :local_size, :integer
  end
end
