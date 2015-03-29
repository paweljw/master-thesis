class CreateSolutions < ActiveRecord::Migration
  def change
    create_table :solutions do |t|
      t.integer :state
      t.references :wave, index: true, foreign_key: true
      t.string :name
      t.string :solution
      t.datetime :completed

      t.timestamps null: false
    end
  end
end
