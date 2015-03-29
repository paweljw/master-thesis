class Admin::NodesController < Admin::ApplicationController
  include CrudController

  def resource_class
    Node
  end
end
