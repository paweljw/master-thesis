class CreateWaves < ActiveRecord::Migration
  def change
    create_table :waves do |t|
      t.references :solution, index: true, foreign_key: true
      t.integer :seq
      t.integer :tasks
      t.integer :state
      t.datetime :completed

      t.timestamps null: false
    end
  end
end
