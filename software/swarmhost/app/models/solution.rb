class Solution < ActiveRecord::Base
  extend CarrierWave::Mount

  has_many :waves, class_name: 'Wave'

  STORAGE_DIR = "tmp"
  TMP_DIR = "tmp"

  mount_uploader :mtx, MetafileUploader
  mount_uploader :rhs, MetafileUploader

  before_save :set_name

  enum state: [ :unpublished, :available, :complete, :broken ]

  def set_name
    self.name = File.basename(mtx.identifier, ".*")
    puts self.name
  end
end
