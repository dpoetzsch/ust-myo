
#include <iostream>
#include <stdexcept>
#include <vector>
#include <myo/cxx/Vector3.hpp>
#include <myo/myo.hpp>

class MyoData {

private:

	// debug prints
	const bool emgprint = false;
	const bool countprint = false;
	const bool otherprints = false;

	// threshold values for hold detection
	// these 3 values are experimental, they may or may not be correct (but this worked best for me), probably they need some recalibration if someone else uses the myo
	const int muscleactvitythreshold = 150; // maybe 150 is best choice, but 100 and 125 also worked well
	const int releasethreshold = 5;
	const int holdingthreshold = 10;

	myo::Myo* myoDevice;
	myo::Arm arm;

	char* armString;

	bool holding = false; // bool flag shows if the person grabs a hold at the moment
	// used for counting the times of high low muscle actvity in pending and release phase
	int releasecount = 0;
	int holdingcount = -1;

	// adds up the absolute values of the emg data to have a value for the overall muscle activity
	// use only for emg data because of hardcoded loop counter
	int addVals(const int8_t* vals){
		int val = 0;
		for (int i = 0; i < 8; i++){
			if (vals[i] < 0){
				val = val - vals[i];
			}
			else{
				val += vals[i];
			}
		}
		return val;
	}

	char* armToString(myo::Arm arm){
		if (myo::armLeft == arm){
			return "(left Arm)";
		}
		else{
			if (myo::armRight == arm){
				return "(right Arm)";
			}
			else{
				return "(unknown Arm)";
			}
		}
	}

public:
	MyoData(myo::Myo* dev, myo::Arm a){
		myoDevice = dev;
		arm = a;
		armString = armToString(arm);
	}

	myo::Myo* getMyo(){
		return myoDevice;
	}

	myo::Arm getArm(){
		return arm;
	}

	// this method returns the boolean flag if the person grabs a hold with the myo related to this MyoData Object
	bool getHoldingState(){
		return holding;
	}

	void emgData(const int8_t* emg){
		/*		for (int i = 0; i < 8; i++){
		samples[i][sample] = (int)emg[i];
		}
		sample++;
		if (printnext10 >= 0) {
		if (sample == samplecount){
		std::cout << "Myo " << identifyMyo(myo) << " has EmgData: " << "  " << average(samples[0]) << "  " << average(samples[1]) << "  " << average(samples[2]) << "  " << average(samples[3]) << "  " << average(samples[4]) << "  " << average(samples[5]) << "  " << average(samples[6]) << "  " << average(samples[7]) << std::endl;
		printnext10++;
		sample = 0;
		}
		}*/
		int addedemg = addVals(emg);
		// get the initial pulse of muscle activity when the person maybe grabs a hold or does something else 
		if (emg[1] > 70 && emg[2] < -10 && emg[3] < 0 && !holding && holdingcount == -1 && addedemg >= muscleactvitythreshold){
			if (otherprints){
				std::cout << armString << "detected holding" << std::endl;
			}
			releasecount = 0;
			holdingcount = 0; // set to 0 to start pending phase
		}

		// pending phase
		// hold is maybe grabbed ... wait for further data to decide if the hold is really grabbed
		if (!holding && holdingcount >= 0){
			if (addedemg >= muscleactvitythreshold){
				if (holdingcount >= holdingthreshold){
					if (otherprints){
						std::cout << armString << "holding flag is set to true" << std::endl;
					}
					holding = true;
					holdingcount = -1;
				}
				else{
					holdingcount++;
				}
			}
			else{
				if (releasecount >= releasethreshold){
					holding = false;
					holdingcount = -1;
					releasecount = 0;
				}
				else{
					releasecount++;
				}
			}
			if (countprint){
				std::cout << armString << "pending: " << "hold: " << holdingcount << "\t release: " << releasecount << "\t sum: " << addedemg << std::endl;
			}
		}

		// release phase
		// hold is grabbed waiting for release
		if (holding){
			if (addedemg < muscleactvitythreshold){
				if (releasecount < releasethreshold){
					releasecount++;
				}
				else{
					if (otherprints){
						std::cout << armString << "released" << std::endl;
					}
					releasecount = 0;
					holdingcount = -1;
					holding = false;
				}
			}
			else{
				releasecount = 0;
			}
		}

		// print data while hold is grabbed
		if (holding || holdingcount >= 0){
			if (emgprint){
				std::cout << armString << "holding EmgData: " << " \t" << (int)emg[0] << "\t" << (int)emg[1] << "\t" << (int)emg[2] << "\t" << (int)emg[3] << "\t" << (int)emg[4] << "\t" << (int)emg[5] << "\t" << (int)emg[6] << "\t" << (int)emg[7] << "\t" << "sum: " << addedemg << std::endl;
			}
		}
	}
};