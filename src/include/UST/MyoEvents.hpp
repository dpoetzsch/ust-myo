
// The class is based on PrintMyoEvents in the multiple-myos.cpp from the samples of the Thalmic Labs Inc. Myo SDK
// Please look at the Thalmic Labs website for the original files
// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// This sample illustrates how to interface with multiple Myo armbands and distinguish between them.

#include <iostream>
#include <stdexcept>
#include <vector>
#include <myo/cxx/Vector3.hpp>
#include <UST/MyoData.hpp>
#include <myo/myo.hpp>

class MyoEvents : public myo::DeviceListener {
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

	MyoEvents(){
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
				std::cout << "arm sync: new right arm device 1: " << "rotation: " << rotation << "   direction: " << xDirection << std::endl;
			}
		}
		else{
			if (arm == myo::armUnknown && right == 0){
				right = new MyoData(myo, myo::armRight);
				if (syncprints){
					std::cout << "arm sync: new right arm device 2: " << "rotation: " << rotation << "   direction: " << xDirection << std::endl;
				}
			}
			else{
				left = new MyoData(myo, arm);
				if (syncprints){
					std::cout << "arm sync: new left arm device: " << "rotation: " << rotation << "   direction: " << xDirection << std::endl;
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