require_relative '../../environment.rb'

class AddRemoteUrlToTask < ActiveRecord::Migration
  def change
    add_column :tasks, :remote_url, :string
  end
end

AddRemoteUrlToTask.migrate(:up)
