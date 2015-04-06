class AddMetafileToSolution < ActiveRecord::Migration
  def change
    add_column :solutions, :metafile, :string
  end
end
