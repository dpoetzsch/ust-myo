#!/usr/bin/env ruby

require 'socket'

server = TCPServer.new "192.168.0.105", 2000 # Server bind to port 2000
loop do
  client = server.accept    # Wait for a client to connect


  loop do
    4.times do |i|
      client.puts "{ \"type\": \"grab\", \"extremity\": #{i+1}, \"likelyhood\": 850, \"time\": \"#{Time.now}\", \"angleX\": #{rand}, \"angleY\": #{rand}, \"angleZ\": #{rand} }"
      sleep 3
    end
    client.puts "{ \"type\": \"delete\", \"time\": \"#{Time.now}\" }"
    sleep 3
  end

  client.close
end
