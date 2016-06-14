
// The project is based on multiple-myos.cpp from the samples of the Thalmic Labs Inc. Myo SDK
// Please look at the Thalmic Labs website for the original files
// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// This sample illustrates how to interface with multiple Myo armbands and distinguish between them.


#define BOOST_LIB_DIAGNOSTIC

#include <iostream>
#include <stdexcept>
#include <UST/MyoEvents.hpp>
#include <UST/server.hpp>


int main(int argc, char** argv)
{
    try {
		GrabServer server;

		myo::Hub hub("com.example.multiple-myos");

		// Instantiate the PrintMyoEvents class we defined above, and attach it as a listener to our Hub.
		MyoEvents printer;
		hub.addListener(&printer);

		while (true) {
			server.acceptConnection();

			bool stillConnected = true;

			bool prevgrabRight = false;
			bool prevgrabLeft = false;

			while (stillConnected) {
				// Process events for 10 milliseconds at a time.
				hub.run(10);

				// use this calls to get the holdingstate for each arm
				if (printer.getHoldingStateRight() != prevgrabRight){
					std::cout << "right" << std::endl;
					if (prevgrabRight = printer.getHoldingStateRight())
						stillConnected = stillConnected && server.signalGrab(HAND_RIGHT, 850);
				}
				if (printer.getHoldingStateLeft() != prevgrabLeft){
					std::cout << "left" << std::endl;
					if (prevgrabLeft = printer.getHoldingStateLeft())
						stillConnected = stillConnected && server.signalGrab(HAND_LEFT, 850);
				}
			}
		}
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}