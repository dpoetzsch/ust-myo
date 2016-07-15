
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

		bool debugprints = true;

		// Instantiate the PrintMyoEvents class we defined above, and attach it as a listener to our Hub.
		MyoEvents printer;
		hub.addListener(&printer);

		std::cout << "wait for the first myo to connect" << std::endl;
		hub.waitForMyo(120000);
		std::cout << "first myo connected ... measuring direction of the wall" << std::endl;
		while (printer.isMeasuring()){
			hub.run(10);
		}
		std::cout << "measuring for the first myo complete" << std::endl;

		std::cout << "wait for the second myo to connect" << std::endl;
		hub.waitForMyo(120000);
		std::cout << "second myo connected ... measuring direction of the wall" << std::endl;
		while (printer.isMeasuring()){
			hub.run(10);
		}
		std::cout << "measuring for the second myo complete" << std::endl;

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
					if (debugprints){
						std::cout << "right" << std::endl;
					}
					prevgrabRight = printer.getHoldingStateRight();
					if (prevgrabRight){
						stillConnected = stillConnected && server.signalGrab(HAND_RIGHT, 850);
						double* orientationData = printer.getOrientationDataRight(); // TODO send data to the betaCube
						// query the orientationdata from the printer with printer.getOrientationDataRight();
					}
				}
				if (printer.getHoldingStateLeft() != prevgrabLeft){
					if (debugprints){
						std::cout << "left" << std::endl;
					}
					prevgrabLeft = printer.getHoldingStateLeft();
					if (prevgrabLeft){
						stillConnected = stillConnected && server.signalGrab(HAND_LEFT, 850);
						double* orientationData = printer.getOrientationDataLeft(); // TODO send data to the betaCube
						// query the orientationdata from the printer with printer.getOrientationDataLeft();
					}
				}

				if (printer.getDeleteState()){
					stillConnected = stillConnected;// && server.signalDelete();
					// the boolean flag automatically resets itself in the printer so getDeleteState() returns true only once
					// when the doubleTap gesture was made
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