require 'devise'

Rails.application.routes.draw do
  devise_for :users

  root to: "nodes#index"

  resources :nodes, only: :index

  namespace :admin do
    resources :nodes
  end
end
