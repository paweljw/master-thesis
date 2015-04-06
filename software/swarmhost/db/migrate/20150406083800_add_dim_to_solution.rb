class AddDimToSolution < ActiveRecord::Migration
  def change
    add_column :solutions, :dim, :integer
  end
end
