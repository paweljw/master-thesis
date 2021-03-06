class Task < ActiveRecord::Base
  belongs_to :wave
  belongs_to :node, class_name: 'Node'

  enum kind: [ :undefined, :solution, :reduce ]
  enum state: [ :not_ready, :ready, :sent, :received, :provisioned, :started, :processed, :complete, :broken ]

  default_scope -> { order(created_at: :asc).order(state: :desc) }
  scope :to_send, -> { where("state = 1") }
  scope :in_field, -> { where("state > 1 AND state < 7") }
  scope :returned_done, -> { where("state >= 7") }

  mount_uploader :metafile, MetafileUploader

  def part_size
    self.wave.solution.part_size
  end

  def global_size
    self.wave.solution.dim
  end

  def local_size
    self.wave.solution.local_size
  end
end
