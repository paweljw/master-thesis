class Wave < ActiveRecord::Base
  belongs_to :solution
  has_many :tasks

  enum state: [ :unpublished, :available, :complete, :broken ]
end
