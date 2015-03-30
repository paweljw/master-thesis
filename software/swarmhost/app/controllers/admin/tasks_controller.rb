class Admin::TasksController < Admin::ApplicationController
  include CrudController

  def resource_class
    Task
  end
end
