#!/usr/bin/env ruby

require 'socket'

server = TCPServer.new 2000 # Server bind to port 2000
loop do
  client = server.accept    # Wait for a client to connect


  loop do
    4.times do |i|
      client.puts "{ \"extremity\": #{i+1}, \"likelyhood\": 850, \"time\": \"#{Time.now}\" }"
      sleep 3
    end
  end

  client.close
end
