class TasksController < ApplicationController
  before_action :check_key

  def index
    requested_number = params[:num]
    @collection = Task.to_send.limit(requested_number)
    @collection.update_all(status: :sent)
    respond_to do |format|
      format.json { render json: @collection }
    end
  end

  def show
    @resource = Task.find(params[:id])
    redirect_to @resource.metafile.url
  end

  def update
    @resource = Task.find(params[:id])
    @resource.update( params.require(:task).permit! )
    render text: 'OK'
  end
end
