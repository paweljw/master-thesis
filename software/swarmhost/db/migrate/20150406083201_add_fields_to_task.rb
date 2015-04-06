class AddFieldsToTask < ActiveRecord::Migration
  def change
    add_column :tasks, :time, :decimal
    add_column :tasks, :backend, :string
  end
end
