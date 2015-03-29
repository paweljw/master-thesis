class RemoveWaveFromSolution < ActiveRecord::Migration
  def change
    remove_column :solutions, :wave_id
  end
end
