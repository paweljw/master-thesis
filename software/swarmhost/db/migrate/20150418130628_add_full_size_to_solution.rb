class AddFullSizeToSolution < ActiveRecord::Migration
  def change
    add_column :solutions, :full_size, :integer
  end
end
