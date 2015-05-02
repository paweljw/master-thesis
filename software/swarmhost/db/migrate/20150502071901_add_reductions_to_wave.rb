class AddReductionsToWave < ActiveRecord::Migration
  def change
    add_column :waves, :reductions, :integer, default: 0
  end
end
