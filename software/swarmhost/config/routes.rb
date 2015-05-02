require 'devise'

Rails.application.routes.draw do
  devise_for :users

  root to: "nodes#index"

  resources :nodes, only: :index do
    get :heartbeat, on: :collection
  end

  resources :tasks, only: [ :index, :show, :update ]

  namespace :admin do
    root to: 'solutions#index'

    resources :nodes do
      get :locust, on: :member
    end

    resources :solutions
    resources :tasks, only: [ :index ]
  end
end
