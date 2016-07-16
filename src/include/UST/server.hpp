//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 David Poetzsch-Heffter <davidpoetzsch@gmx.net>
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  std::string s = ctime(&now);
  boost::algorithm::trim_right(s);
  return s;
}

enum Extremity {
  HAND_LEFT,
  HAND_RIGHT,
  FOOT_LEFT,
  FOOT_RIGHT
};

std::string extremityToJson(Extremity extremity) {
  switch (extremity) {
  case HAND_LEFT:
    return "1";
  case HAND_RIGHT:
    return "2";
  case FOOT_LEFT:
    return "3";
  case FOOT_RIGHT:
    return "4";
  }
}

class GrabServer {
public:
  static const int SERVER_PORT = 2000;

  GrabServer()
      : acceptor(io_service, tcp::endpoint(tcp::v4(), SERVER_PORT)),
        socket(NULL) {}
  ~GrabServer() {
    if (socket != NULL) {
      delete socket;
    }
  }

  void acceptConnection() {
    std::cout << "Waiting for connection... ";
    std::cout.flush();
    if (socket != NULL) {
      delete socket;
    }
    socket = new tcp::socket(io_service);
    acceptor.accept(*socket);
    std::cout << "Connected.\n";
  }

  /**
   * @return true if sending was successful, false if the connection was closed.
   */
  bool signalGrab(Extremity extremity, int likelyhood, double angleX, double angleY, double angleZ) {
    return sendMessage("{ \"type\": \"grab\", \"likelyhood\": " + boost::lexical_cast<std::string>(likelyhood) +
      ", \"extremity\": " + extremityToJson(extremity) +
      ", \"time\": \"" + make_daytime_string() +
      "\", \"angleX\": " + boost::lexical_cast<std::string>(angleX) +
      ", \"angleY\": " + boost::lexical_cast<std::string>(angleY) +
      ", \"angleZ\": " + boost::lexical_cast<std::string>(angleZ) + " }");
  }

  /**
   * @return true if sending was successful, false if the connection was closed.
   */
  bool signalDelete() {
    return sendMessage("{ \"type\": \"delete\", \"time\": \"" + make_daytime_string() + "\" }");
  }

private:
  /**
   * @return true if sending was successful, false if the connection was closed.
   */
  bool sendMessage(std::string msg) {
    if ((socket == NULL) || !socket->is_open()) {
      std::cout << "Connection was closed.\n";
      return false;
    } else {
      std::cout << "Sending message: '" << msg << "'\n";
      boost::system::error_code error;
      boost::asio::write(*socket, boost::asio::buffer(msg + "\n"), error);
      if (error != 0) {
        std::cout << "Connection was closed.\n";
        return false;
      } else {
        return true;
      }
    }
  }

  boost::asio::io_service io_service;
  tcp::acceptor acceptor;
  tcp::socket *socket;
};
