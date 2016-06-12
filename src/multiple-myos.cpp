
// The project is based on multiple-myos.cpp from the samples of the Thalmic Labs Inc. Myo SDK
// Please look at the Thalmic Labs website for the original files
// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// This sample illustrates how to interface with multiple Myo armbands and distinguish between them.

#include <iostream>
#include <stdexcept>
#include <vector>
#include <myo/cxx/Vector3.hpp>

#include <myo/myo.hpp>

class MyoData {
private:

	// debug prints
	const bool emgprint = true;
	const bool countprint = true;
	const bool otherprints = true;

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
			return "left Arm";
		}
		else{
			if (myo::armRight == arm){
				return "right Arm";
			}
			else{
				return "unknown Arm";
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
				std::cout << armString <<"detected holding" << std::endl;
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

class PrintMyoEvents : public myo::DeviceListener {
private:

	// start deprecated variables
	int sample = 0;
	int** samples;
	const int samplecount = 10;
	int printnext10 = 0;
	bool gyroOk = true;
	const int gyrothreshold = 75;
	bool accelOk = true;
	const float accelthreshold = 0.5;
	// end deprecated variables

	MyoData* right = 0;
	MyoData* left = 0;

	// debug print
	const bool syncprints = true;

	// not used at the moment
	// only important for the sampling approach (but this does not work in my opinion)
	int average(int* vals){
		double ret = 0.0;
		for (int i = 0; i < samplecount; i++){
			ret += vals[i];
		}
		ret = ret / samplecount;
		return ((int)ret);
	}

public:

	PrintMyoEvents(){
		samples = new int*[8];
		for (int i = 0; i < 8; i++){
			samples[i] = new int[samplecount];
		}
	}

    // Every time Myo Connect successfully pairs with a Myo armband, this function will be called.
    //
    // You can rely on the following rules:
    //  - onPair() will only be called once for each Myo device
    //  - no other events will occur involving a given Myo device before onPair() is called with it
    //
    // If you need to do some kind of per-Myo preparation before handling events, you can safely do it in onPair().
    void onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)
    {
        // Print out the MAC address of the armband we paired with.

        // The pointer address we get for a Myo is unique - in other words, it's safe to compare two Myo pointers to
        // see if they're referring to the same Myo.

        // Add the Myo pointer to our list of known Myo devices. This list is used to implement identifyMyo() below so
        // that we can give each Myo a nice short identifier.
        knownMyos.push_back(myo);

        // Now that we've added it to our list, get our short ID for it and print it out.
        std::cout << "Paired with " << identifyMyo(myo) << "." << std::endl;
    }

    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        std::cout << "Myo " << identifyMyo(myo) << " switched to pose " << pose.toString() << "." << std::endl;
    }

    void onConnect(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)
    {
        std::cout << "Myo " << identifyMyo(myo) << " has connected." << std::endl;
		myo->setStreamEmg(myo->streamEmgEnabled);
    }

    void onDisconnect(myo::Myo* myo, uint64_t timestamp)
    {
        std::cout << "Myo " << identifyMyo(myo) << " has disconnected." << std::endl;
		if (right != 0 && myo == right->getMyo()){
			delete right;
			right = 0;
		}
		if (left != 0 && myo == left->getMyo()){
			delete left;
			left = 0;
		}
    }

	// not used for muscle activity
	void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro){
		if (gyro.x() < gyrothreshold && gyro.x() > -gyrothreshold && gyro.y() < gyrothreshold && gyro.y() > -gyrothreshold && gyro.z() < gyrothreshold && gyro.z() > -gyrothreshold){
			gyroOk = true;
			//std::cout << "holding gyroscopicData: " << "\t" << gyro.x() << "\t" << gyro.y() << "\t" << gyro.z() << std::endl;
		}
		else{
			gyroOk = false;
		}
	}

	// not used for muscle activity
	void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel) {
		/*if (holding){
			std::cout << "holding accelData: " << "\t" << accel.x() << "\t" << accel.y() << "\t" << accel.z() << std::endl;
		}*/
		float f = accel.x() + accel.y() + accel.z();
		if (f > 1 - accelthreshold && f < 1 + accelthreshold){
			accelOk = true;
		}
		else{
			accelOk = false;
		}
	}

	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
		myo::WarmupState warmupState) 
	{
		if (arm == myo::armRight){
			right = new MyoData(myo, arm);
			if (syncprints){
				std::cout << "arm sync: new right arm device" << std::endl;
			}
		}
		else{
			if (arm == myo::armUnknown && right == 0){
				right = new MyoData(myo, myo::armRight);
				if (syncprints){
					std::cout << "arm sync: new right arm device" << std::endl;
				}
			}
			else{
				left = new MyoData(myo, arm);
				if (syncprints){
					std::cout << "arm sync: new left arm device" << std::endl;
				}
			}
		}
	}

	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
    {
		if (right != 0 && myo == right->getMyo()){
			right->emgData(emg);
		}
		else{
			if (left != 0){
				left->emgData(emg);
			}
		}
	}

    // This is a utility function implemented for this sample that maps a myo::Myo* to a unique ID starting at 1.
    // It does so by looking for the Myo pointer in knownMyos, which onPair() adds each Myo into as it is paired.
    size_t identifyMyo(myo::Myo* myo) {
        // Walk through the list of Myo devices that we've seen pairing events for.
        for (size_t i = 0; i < knownMyos.size(); ++i) {
            // If two Myo pointers compare equal, they refer to the same Myo device.
            if (knownMyos[i] == myo) {
                return i + 1;
            }
        }

        return 0;
    }

    // We store each Myo pointer that we pair with in this list, so that we can keep track of the order we've seen
    // each Myo and give it a unique short identifier (see onPair() and identifyMyo() above).
    std::vector<myo::Myo*> knownMyos;
};

int main(int argc, char** argv)
{
    try {
        myo::Hub hub("com.example.multiple-myos");

        // Instantiate the PrintMyoEvents class we defined above, and attach it as a listener to our Hub.
        PrintMyoEvents printer;
        hub.addListener(&printer);

        while (1) {
            // Process events for 10 milliseconds at a time.
            hub.run(10);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}