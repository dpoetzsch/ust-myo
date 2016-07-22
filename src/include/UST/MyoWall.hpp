
#include <iostream>
#include <stdexcept>
#include <vector>
#include <myo/cxx/Vector3.hpp>
#include <myo/myo.hpp>

class MyoWall {

private:
	myo::Myo* myoDevice;

	int samplepointer = 0;
	double** samples;
	const int samplecount = 10;

	double pitch_wall = 0.0;
	double yaw_wall = 0.0;
	double roll_wall = 0.0; // roll is not needed at the moment because it will be projected from 3D to a 2D surface

	const bool orientationprint = true;
	const bool otherprints = false;

	double average(double* vals){
		double ret = 0.0;
		for (int i = 0; i < samplecount; i++){
			ret += vals[i];
		}
		ret = ret / ((double)samplecount);
		return ret;
	}

public:
	MyoWall(myo::Myo* dev){
		myoDevice = dev;

		samples = new double*[3];
		for (int i = 0; i < 3; i++){
			samples[i] = new double[samplecount];
		}
	}

	myo::Myo* getMyo(){
		return myoDevice;
	}
	
	void OrientationData(double roll, double pitch, double yaw){
		if (samplepointer < samplecount){
			samples[0][samplepointer] = roll;
			samples[1][samplepointer] = pitch;
			samples[2][samplepointer] = yaw;
			samplepointer++;
			if (orientationprint && (samplepointer % 5 == 0)){
				std::cout << ": orientationdata: roll: " << roll << "; pitch: " << pitch << "; yaw: " << yaw << ";" << std::endl;
			}
		}
	}

	bool isMeasuring(){
		return samplepointer < samplecount;
	}

	double* getWallData(){
		double* ret = new double[3];
		if (otherprints){
			std::cout << ": orientationdata: samples[0]: " << (samples[0] == 0) << "; samples[1]: " << (samples[1] == 0) << "; samples[2]: " << (samples[2] == 0) << ";" << std::endl;
		}
		ret[0] = average(samples[0]);
		ret[1] = average(samples[1]);
		ret[2] = average(samples[2]);
		if (orientationprint){
			std::cout << ": orientationdata: roll: " << ret[0] << "; pitch: " << ret[1] << "; yaw: " << ret[2] << ";" << std::endl;
		}
		return ret;
	}

};