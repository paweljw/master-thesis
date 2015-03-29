class Node < ActiveRecord::Base
  require 'securerandom'

  has_many :tasks

  after_create :generate_key

  def generate_key
    while true
      random_string = SecureRandom.hex
      if Node.find_by(key: random_string).nil?
        self.update(key: random_string)
        break
      end
    end
  end
end
