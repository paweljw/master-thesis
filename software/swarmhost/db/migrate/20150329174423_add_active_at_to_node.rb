class AddActiveAtToNode < ActiveRecord::Migration
  def change
    add_column :nodes, :active_at, :datetime
  end
end
