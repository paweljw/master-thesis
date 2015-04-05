class AddFieldsToSolution < ActiveRecord::Migration
  def change
    add_column :solutions, :mtx, :string
    add_column :solutions, :rhs, :string
    add_column :solutions, :part_size, :string
  end
end
