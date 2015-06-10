class AddLocalSizeToSolution < ActiveRecord::Migration
  def change
    add_column :solutions, :local_size, :integer
  end
end
