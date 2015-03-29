class Solution < ActiveRecord::Base
  has_many :waves

  enum state: [ :unpublished, :available, :complete, :broken ]
end
