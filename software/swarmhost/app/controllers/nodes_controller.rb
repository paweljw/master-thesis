class NodesController < ApplicationController
  def index
    @nodes = Node.all
  end

  def heartbeat
    check_key
    @node.update active_at: DateTime.now
    render text: "OK"
  end
end
