class Task < ActiveRecord::Base
  belongs_to :wave
  belongs_to :node
end
