//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 David Poetzsch-Heffter <davidpoetzsch@gmx.net>
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <unistd.h>

using boost::asio::ip::tcp;

const int SERVER_PORT = 2000;

std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

int main() {
  try {
    boost::asio::io_service io_service;

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), SERVER_PORT));

    for (;;) {
      tcp::socket socket(io_service);
      acceptor.accept(socket);

      while (socket.is_open()) {
        boost::system::error_code ignored_error;
        boost::asio::write(socket, boost::asio::buffer(make_daytime_string()), ignored_error);
        usleep(1000000); // sleep 1 sec
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}