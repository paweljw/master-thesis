require_relative '../../environment.rb'

class CreateTasks < ActiveRecord::Migration
  def change
    create_table :tasks do |t|
      t.integer :kind
      t.integer :state
      t.string :name
      t.integer :swarmhost_id
    end
  end
end

CreateTasks.migrate(:up)
