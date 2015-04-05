#!/usr/bin/env ruby

require './environment'

logger = Logger.new("log/locust.log")
logger.info 'Starting Locust'

t = Task.new

server_key = LOCUST_CONFIG['server']['key']
heartbeat_uri = URI.join(LOCUST_CONFIG['server']['address'], '/nodes/heartbeat')

begin
  while true do
    logger.info 'Sending heartbeat...'
    response = RestClient.get heartbeat_uri.to_s, { params: { auth: server_key }}
    logger.info "Server responded #{response}"

    sleep 5
  end
rescue SignalException => e
  p "Graceful exit begin"
  sleep 3
  p "Graceful exit end"
end
