class Solution < ActiveRecord::Base
  extend CarrierWave::Mount

  has_many :waves, class_name: 'Wave', dependent: :delete_all
  has_many :tasks, through: :waves

  STORAGE_DIR = "tmp"
  TMP_DIR = "tmp"

  mount_uploader :mtx, MetafileUploader
  mount_uploader :rhs, MetafileUploader

  mount_uploader :metafile, MetafileUploader

  before_create :set_name

  def self.states
    [ :unpublished, :available, :complete, :broken ]
  end

  enum state: states

  def set_name
    self.name ||= File.basename(mtx.identifier, ".*") 
  end

  def last_reduction_wave
    waves.where("reductions > 0").order(seq: :desc).first
  end

  def percent_complete
    return 0.0 if last_reduction_wave.nil?
    reverse = dim.to_f - last_reduction_wave.reductions.to_f
    reverse / ( dim.to_f / 100.0 )
  end

  def elapsed
    start = self.tasks.all.order(started_at: :desc).first
    stop = self.tasks.all.order(started_at: :desc).last

    return (stop.completed - start.started_at).round(4) if start.started_at.present? && stop.completed.present?
    return (self.completed - self.started).round(4) if self.completed.present? && self.started.present?
    "---"
  end
end
