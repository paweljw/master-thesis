class Wave < ActiveRecord::Base
  belongs_to :solution
  has_many :tasks

  enum state: [ :unpublished, :available, :complete, :broken ]

  def decrement_task_number
    self.update(tasks: self.tasks - 1)
    self.reload
    if self.tasks == 0
      puts "TO NIE POWINIEN BYĆ STUB, WEŹ MNIE NAPRAW"
      raise "PJWStubbedFunctionHeShouldNotHave"
    end
  end
end
