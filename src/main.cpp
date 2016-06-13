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

      // TODO check the myo for data
      // TODO analyse that data
      // TODO if grab detected: use server.signalGrab(likelyhood)
      
      // DEMO: continuously send the grab signal with 85.0% likelyhood
      // until the client closes the connection
      while (server.signalGrab(HAND_LEFT, 850)) {
        boost::this_thread::sleep(boost::posix_time::millisec(1000));
      }
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}