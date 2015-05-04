class AddStartedToSolution < ActiveRecord::Migration
  def change
    add_column :solutions, :started, :datetime
  end
end
