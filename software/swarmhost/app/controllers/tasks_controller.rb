class TasksController < ApplicationController
  before_action :check_key
  protect_from_forgery except: :update

  def index
    requested_number = params[:num].to_i
    @collection = Task.to_send.limit(requested_number)
    
    @collection.each do |t| 
      t.wave.solution.update(started: DateTime.now) if t.wave.solution.started.nil?
    end

    respond_to do |format|
      format.json { render json: @collection }
    end
    @collection.update_all(state: 2, node_id: @node.id, started_at: DateTime.now)
  end

  def show
    @resource = Task.find(params[:id])
    redirect_to @resource.metafile.url
  end

  def update
    @resource = Task.find(params[:id])
    @resource.update( params.require(:task).permit! )
    @resource.reload
    if @resource.state == "complete"
      @resource.update completed: DateTime.now
      @resource.wave.decrement_task_number
    end
    render text: 'OK'
  end
end
