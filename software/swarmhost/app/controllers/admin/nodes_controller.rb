class Admin::NodesController < Admin::ApplicationController
  include CrudController

  def locust
    @node = Node.find(params[:id])
    respond_to do |format|
      format.yml do
        response.headers['Content-Disposition'] = 'attachment; filename="locust.yml"'
        render text: { 'server' => { 'address' => root_url, 'key' => @node.key }, 'client' => { 'platforms' => 'all', 'min_tasks' => 5 } }.to_yaml
      end
    end
  end

  def resource_class
    Node
  end
end
