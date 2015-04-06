class Solution < ActiveRecord::Base
  extend CarrierWave::Mount

  has_many :waves, class_name: 'Wave'
  has_many :tasks, through: :waves

  STORAGE_DIR = "tmp"
  TMP_DIR = "tmp"

  mount_uploader :mtx, MetafileUploader
  mount_uploader :rhs, MetafileUploader

  mount_uploader :metafile, MetafileUploader

  before_save :set_name

  def self.states
    [ :unpublished, :available, :complete, :broken ]
  end

  enum state: states

  def set_name
    self.name = File.basename(mtx.identifier, ".*")
    puts self.name
  end
end
