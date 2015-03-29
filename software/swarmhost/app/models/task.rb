class Task < ActiveRecord::Base
  belongs_to :wave
  belongs_to :node

  enum kind: [ :undefined, :solution, :reduce ]
  enum state: [ :not_ready, :ready, :sent, :received, :provisioned, :started, :processed, :complete, :broken ]
end
