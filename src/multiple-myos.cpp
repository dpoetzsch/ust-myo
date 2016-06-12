
// The project is based on multiple-myos.cpp from the samples of the Thalmic Labs Inc. Myo SDK
// Please look at the Thalmic Labs website for the original files
// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// This sample illustrates how to interface with multiple Myo armbands and distinguish between them.

#include <iostream>
#include <stdexcept>
#include <UST/MyoEvents.hpp>

int main(int argc, char** argv)
{
    try {
        myo::Hub hub("com.example.multiple-myos");

        // Instantiate the PrintMyoEvents class we defined above, and attach it as a listener to our Hub.
        MyoEvents printer;
        hub.addListener(&printer);

        while (1) {
            // Process events for 10 milliseconds at a time.
            hub.run(10);


			// use this calls to get the holdingstate for each arm
			printer.getHoldingStateRight();
			printer.getHoldingStateLeft();
			//

        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}