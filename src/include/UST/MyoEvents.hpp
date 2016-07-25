
// The class is based on PrintMyoEvents in the multiple-myos.cpp from the samples of the Thalmic Labs Inc. Myo SDK
// Please look at the Thalmic Labs website for the original files
// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.

// This sample illustrates how to interface with multiple Myo armbands and distinguish between them.
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <list>
#include <string>
#include <algorithm>
#include <vector>
#include <myo/cxx/Vector3.hpp>
#include <UST/MyoData.hpp>
#include <UST/MyoWall.hpp>
#include <myo/myo.hpp>

class MyoEvents : public myo::DeviceListener {
private:

	// start deprecated variables
	/*int sample = 0;
	int** samples;
	const int samplecount = 10;
	int printnext10 = 0;
	bool gyroOk = true;
	const int gyrothreshold = 75;
	bool accelOk = true;
	const float accelthreshold = 0.5;*/
	// end deprecated variables

	MyoData* right = 0;
	MyoData* left = 0;

	//std::list<MyoWall*> wallData;
	MyoWall* first = 0;
	MyoWall* second = 0;

	bool deleteFlag = false;

	// debug print
	const bool syncprints = true;
	const bool wallmeasurementprints = true;

/*	// not used at the moment
	// only important for the sampling approach (but this does not work in my opinion)
	int average(int* vals){
		double ret = 0.0;
		for (int i = 0; i < samplecount; i++){
			ret += vals[i];
		}
		ret = ret / samplecount;
		return ((int)ret);
	}*/

	MyoWall* searchWallData(myo::Myo* myo){
		/*for (std::list<MyoWall*>::iterator it = wallData.begin(); it != wallData.end(); it++)
		{
			if ((*it)->getMyo() == myo){
				if (wallmeasurementprints){
					std::cout << "wallsearch has found an object.   list size: " << wallData.size() << std::endl;
				}
				return (*it);
			}
		}
		if (wallmeasurementprints){
			std::cout << "wallsearch has not found an object.   list size: " << wallData.size() << std::endl;
		}
		return 0;*/
		if (first != 0 && first->getMyo() == myo){
			if (wallmeasurementprints){
				std::cout << "wallsearch has found an object.   first" << std::endl;
			}
			return first;
		}
		if (second != 0 && second->getMyo() == myo){
			if (wallmeasurementprints){
				std::cout << "wallsearch has found an object.   second" << std::endl;
			}
			return second;
		}
		if (wallmeasurementprints){
			std::cout << "wallsearch has not found an object.   first : " << (first == 0) << "      second: " << (second == 0) << std::endl;
		}
		return 0;
	}

	void addRightArmDevice(myo::Myo* myo){
		MyoWall* wall = searchWallData(myo);
		if (wall != 0){
			right = new MyoData(myo, myo::armRight, wall->getWallData());
			if (left != 0){
				if (left->getMyo() == myo){
					left = 0;
				}
			}
		}
	}

	void addLeftArmDevice(myo::Myo* myo){
		MyoWall* wall = searchWallData(myo);
		if (wall != 0){
			left = new MyoData(myo, myo::armLeft, wall->getWallData());
			if (right != 0){
				if (right->getMyo() == myo){
					right = 0;
				}
			}
		}
	}

	double RadToDeg(double rad){
		return (rad  * ((double)180) / M_PI);
	}

public:

	MyoEvents(){
		/*samples = new int*[8];
		for (int i = 0; i < 8; i++){
			samples[i] = new int[samplecount];
		}*/
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
		myo::Pose p;
		if (pose == p.doubleTap){
			deleteFlag = true;
		}
	}

	void onConnect(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)
	{
		std::cout << "Myo " << identifyMyo(myo) << " has connected.  " << myo << std::endl;
		myo->setStreamEmg(myo->streamEmgEnabled);
		MyoWall* wall = searchWallData(myo);
		if (wall == 0){
			if (first == 0){
				first = new MyoWall(myo);
			}else{
				second = new MyoWall(myo);
			}
		}/*
		else{
			if (first == 0){
				first = new MyoWall(myo);
			}
			else{
				second = new MyoWall(myo);
			}
		}*/
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
/*	void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro){
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
		}*//*
		float f = accel.x() + accel.y() + accel.z();
		if (f > 1 - accelthreshold && f < 1 + accelthreshold){
			accelOk = true;
		}
		else{
			accelOk = false;
		}
	}*/

	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
		myo::WarmupState warmupState)
	{
		if (arm == myo::armRight){
			addRightArmDevice(myo);
			// right->setRoll(180.0 - RadToDeg(((double)rotation))); // TODO
			if (syncprints){
				std::cout << "arm sync: new right arm device 1: " << "rotation: " << rotation << "   direction: " << xDirection << std::endl;
			}
		}
		else{
			if (arm == myo::armUnknown && right == 0){
				addRightArmDevice(myo);
				// right->setRoll(180.0 - RadToDeg(((double)rotation))); // TODO
				if (syncprints){
					std::cout << "arm sync: new right arm device 2: " << "rotation: " << rotation << "   direction: " << xDirection << std::endl;
				}
			}
			else{
				
				addLeftArmDevice(myo);
				// left->setRoll(180.0 - RadToDeg(((double)rotation))); // TODO
				if (syncprints){
					std::cout << "arm sync: new left arm device: " << "rotation: " << rotation << "   direction: " << xDirection << "   arm: " << arm << std::endl;
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
			if (left != 0 && myo == left->getMyo()){
				left->emgData(emg);
			}
		}
	}

	void onOrientationData(myo:: Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat) {
		// from the onOrientationData method in the HelloMyo sample from the myo c++ sdk
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;
		double roll_w, pitch_w, yaw_w;

		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		double roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		double pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		double yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

		// Convert the floating point angles in radians to a scale from 0 to 18.
		/*roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);*/
		roll_w = RadToDeg(roll);
		pitch_w = RadToDeg(pitch);
		yaw_w = RadToDeg(yaw);

		/*for (std::list<MyoWall*>::iterator it = wallData.begin(); it != wallData.end(); it++)
		{
			if ((*it)->getMyo() == myo){
				MyoWall* wall = (*it);
				if (wall->isMeasuring()){
					wall->OrientationData(roll_w, pitch_w, yaw_w);
				}
			}
		}*/
		if (first != 0 && first->isMeasuring() && myo == first->getMyo()){
			first->OrientationData(roll_w, pitch_w, yaw_w);
		}
		else
		{
			if (second != 0 && second->isMeasuring() && myo == second->getMyo()){
				second->OrientationData(roll_w, pitch_w, yaw_w);
			}
		}

		if (right != 0 && myo == right->getMyo()){
			right->OrientationData(roll_w, pitch_w, yaw_w);
		}
		else
		{
			if (left != 0 && myo == left->getMyo()){
				left->OrientationData(roll_w, pitch_w, yaw_w);
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

	bool getHoldingStateRight(){
		if (right != 0){
			return right->getHoldingState();
		}
		return false;
	}

	bool getHoldingStateLeft(){
		if (left != 0){
			return left->getHoldingState();
		}
		return false;
	}
	double* getOrientationDataRight(){
		return getOrientationDataRight(false);
	}
	double* getOrientationDataLeft(){
		return getOrientationDataLeft(false);
	}
	double* getOrientationDataRight(bool noprint){
		if (right != 0){
			return right->getOrientationData(noprint);
		}
		return 0;
	}

	double* getOrientationDataLeft(bool noprint){
		if (left != 0){
			return left->getOrientationData(noprint);
		}
		return 0;
	}

	bool getDeleteState(){
		bool temp = deleteFlag;
		deleteFlag = false;
		return temp;
	}

	bool isMeasuring(){
		/*for (std::list<MyoWall*>::iterator it = wallData.begin(); it != wallData.end(); it++)
		{
			if ((*it)->isMeasuring()){
				return true;
			}
		}*/
		if (first != 0 && first->isMeasuring()){
			return true;
		}
		if (second != 0 && second->isMeasuring()){
			return true;
		}
		return false;
	}

	// We store each Myo pointer that we pair with in this list, so that we can keep track of the order we've seen
	// each Myo and give it a unique short identifier (see onPair() and identifyMyo() above).
	std::vector<myo::Myo*> knownMyos;
};