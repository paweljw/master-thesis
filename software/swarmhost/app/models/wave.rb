class Wave < ActiveRecord::Base
  belongs_to :solution
  has_many :tasks

  enum state: [ :unpublished, :available, :complete, :broken ]

  def decrement_task_number
    update(tasks_number: tasks_number - 1)
    self.reload
    if self.tasks_number == 0
      ProcessWaveJob.perform_later(self)
    end
  end
end
