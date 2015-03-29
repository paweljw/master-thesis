class CreateTasks < ActiveRecord::Migration
  def change
    create_table :tasks do |t|
      t.references :wave, index: true, foreign_key: true
      t.string :name
      t.integer :type
      t.integer :state
      t.integer :part_num
      t.string :metafile
      t.references :node, index: true, foreign_key: true

      t.timestamps null: false
    end
  end
end
