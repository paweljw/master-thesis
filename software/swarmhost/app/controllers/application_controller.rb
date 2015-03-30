class ApplicationController < ActionController::Base
  # Prevent CSRF attacks by raising an exception.
  # For APIs, you may want to use :null_session instead.
  protect_from_forgery with: :exception

  def check_key
    raise "Bad Key" if params[:auth].blank?
    raise "Bad Key" if Node.find_by(key: params[:auth]).nil?

    @node = Node.find_by(key: params[:auth]).first
  end
end
