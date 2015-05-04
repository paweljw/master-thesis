module CrudController
  include ActiveSupport::Concern

  def index
    @collection = resource_class.all
  end

  def new
    @resource = resource_class.new
  end

  def update
    @resource = resource_class.find(params[:id])
    @resource.update(safe_params)
    redirect_to action: :index
  end

  def edit
    @resource = resource_class.find(params[:id])
  end

  def create
    @resource = resource_class.create(safe_params)
    redirect_to action: :index
  end

  def show
    @resource = resource_class.find(params[:id])
  end

  def destroy
    @resource = resource_class.find(params[:id])
    @resource.delete
    redirect_to action: :index
  end

  def safe_params
    params.require(resource_class.name.demodulize.underscore).permit!
  end
end