class TasksController < ApplicationController
  before_action :check_key
  protect_from_forgery except: :update

  def index
    requested_number = params[:num].to_i
    @collection = Task.to_send.limit(requested_number)
    p "collection count: #{@collection.count}"
    respond_to do |format|
      format.json { render json: @collection }
    end
    @collection.update_all(state: 2, node_id: @node.id)
  end

  def show
    @resource = Task.find(params[:id])
    redirect_to @resource.metafile.url
  end

  def update
    @resource = Task.find(params[:id])
    @resource.update( params.require(:task).permit! )
    @resource.reload
    if @resource.state == :complete
      @resource.wave.decrement_task_number
    end
    render text: 'OK'
  end
end
