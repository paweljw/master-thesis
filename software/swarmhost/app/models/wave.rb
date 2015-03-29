class Wave < ActiveRecord::Base
  belongs_to :solution
  has_many :tasks
end
