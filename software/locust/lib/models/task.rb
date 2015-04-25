class Task < ActiveRecord::Base
  enum kind: [ :undefined, :solution, :reduce ]
  enum state: [ :not_ready, :ready, :sent, :received, :provisioned, :started, :processed, :complete, :broken ]

  # validate :status_changed

  scope :undone, -> { where("state < 7") }
  scope :for_provisioning, -> { where("state = 3") }
  scope :for_doing, -> { where("state = 4") }
  scope :for_upload, -> { where("state = 6") }

  def offset
    self.part_num * self.part_size
  end

  def status_changed
    if self.status == 5 && !self.status.changed?
      return false
    end
  end
end
