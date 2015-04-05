require 'devise'

Rails.application.routes.draw do
  devise_for :users

  root to: "nodes#index"

  resources :nodes, only: :index do
    get :heartbeat, on: :collection
  end

  resources :tasks, only: [ :index, :show, :update ]

  namespace :admin do
    resources :nodes
    resources :solutions, only: [ :index, :create, :new, :destroy ]
  end
end
