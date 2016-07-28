
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

		/*std::cout << "wait for the first myo to connect" << std::endl;
		hub.waitForMyo(120000);
		std::cout << "first myo connected ... measuring direction of the wall" << std::endl;*/

		/*while (printer.isMeasuring()){
			hub.run(10);
		}*/
		/*
		bool measuring = true;
		while (true){
			if (!printer.isMeasuring() && measuring){
				measuring = false;
				std::cout << "measuring for the first myo complete" << std::endl;
				std::cout << "now sync the myo with the arm" << std::endl;
			}
			if (printer.getOrientationDataRight() != 0 || printer.getOrientationDataLeft() != 0){
				break;
			}
			hub.run(10);
		}
		std::cout << "sync for the first myo complete" << std::endl;

		std::cout << "wait for the second myo to connect" << std::endl;
		hub.waitForMyo(120000);
		std::cout << "second myo connected ... measuring direction of the wall" << std::endl;
		measuring = true;
		while (true){
			if (!printer.isMeasuring() && measuring){
				measuring = false;
				std::cout << "measuring for the second myo complete" << std::endl;
				std::cout << "now sync the myo with the arm" << std::endl;
			}
			if (printer.getOrientationDataRight() != 0 && printer.getOrientationDataLeft() != 0){
				break;
			}
			hub.run(10);
		}
		std::cout << "sync for the second myo complete" << std::endl;

		*/



		// calibration phase
/*		std::cout << "wait for the first myo to connect" << std::endl;
		int phase = -1;
		while (printer.getOrientationDataRight(true) == 0 || printer.getOrientationDataLeft(true) == 0){
			if (printer.isMeasuring() && phase == -1){
				phase = 0;
				std::cout << "first myo connected ... measuring direction of the wall" << std::endl;
			}
			if (!printer.isMeasuring() && phase == 0){
				phase = 1;
				std::cout << "measuring for the first myo complete" << std::endl;
				std::cout << "now sync the myo with the arm" << std::endl;
			}
			if ((printer.getOrientationDataRight(true) != 0 || printer.getOrientationDataLeft(true) != 0) &&  phase == 1){
				phase = 2;
				std::cout << "sync for the first myo complete" << std::endl;
				std::cout << "wait for the second myo to connect" << std::endl;
			}
			if (printer.isMeasuring() && phase == 2 ){
				phase = 3;
			}
			if (!printer.isMeasuring() && phase == 3){
				phase = 4;
				std::cout << "measuring for the second myo complete" << std::endl;
				std::cout << "now sync the myo with the arm" << std::endl;
			}
			hub.run(10);
		}
		std::cout << "finished calibration" << std::endl;
		// end of calibration phase
		*/


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
						double* orientationData = printer.getOrientationDataRight();
						if (orientationData == 0){
							stillConnected = stillConnected && server.signalGrab(HAND_RIGHT, 850, 0.0, 0.0, 0.0);
						}
						else
						{
							stillConnected = stillConnected && server.signalGrab(HAND_RIGHT, 850, 0.0, 0.0, 0.0);// orientationData[0], orientationData[1], 0.0);
						}
						// actually getOffSetData would a more precise name
						// I actually 3 values but I have to project the values from 3D to 2D space so
						// sending 2 double values is enough 
						// query the orientationdata from the printer with printer.getOrientationDataRight();
					}
				}
				if (printer.getHoldingStateLeft() != prevgrabLeft){
					if (debugprints){
						std::cout << "left" << std::endl;
					}
					prevgrabLeft = printer.getHoldingStateLeft();
					if (prevgrabLeft){
						double* orientationData = printer.getOrientationDataLeft();
						if (orientationData == 0){
							stillConnected = stillConnected && server.signalGrab(HAND_LEFT, 850, 0.0, 0.0, 0.0);
						}
						else
						{
							stillConnected = stillConnected && server.signalGrab(HAND_LEFT, 850, 0.0, 0.0, 0.0); // orientationData[0], orientationData[1], 0.0);
						}
						// actually getOffSetData would a more precise name
						// I actually 3 values but I have to project the values from 3D to 2D space so
						// sending 2 double values is enough 
						// query the orientationdata from the printer with printer.getOrientationDataLeft();
					}
				}

				if (printer.getDeleteState()){
					stillConnected = stillConnected && server.signalDelete();
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