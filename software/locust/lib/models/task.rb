class Task < ActiveRecord::Base
  enum kind: [ :undefined, :solution, :reduce ]
  enum state: [ :not_ready, :ready, :sent, :received, :provisioned, :started, :processed, :complete, :broken ]

  scope :undone, -> { where("state <= 5") }
  scope :for_provisioning, -> { where("state = 3") }
end
