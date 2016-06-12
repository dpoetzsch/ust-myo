//
// main.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 David Poetzsch-Heffter <davidpoetzsch@gmx.net>
//

#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "server.hpp"

int main() {
  try {
    GrabServer server;

    for (;;) {
      server.acceptConnection();

      while (server.signalGrab(85)) {
        boost::this_thread::sleep(boost::posix_time::millisec(1000));
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}