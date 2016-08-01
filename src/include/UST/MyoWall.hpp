
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
	const int samplecount = 100;

	double pitch_wall = 0.0;
	double yaw_wall = 0.0;
	double roll_wall = 0.0;

	const bool orientationprint = true;
	const bool otherprints = false;

	double distance(double val1, double val2){
		double ret = val1 - val2;
		if (val1 <= 0.0 && val2 <= 0.0){
			return ret;
		}
		if (val1 >= 0.0 && val2 >= 0.0){
			return ret;
		}
		if (ret <= 180.0 && ret >= -180.0){
			return ret;
		}
		return val2 - val1;
	}

	double average(double* vals){
		double ret = 0.0;
		for (int i = 1; i < samplecount; i++){
			ret += distance(vals[0],vals[i]);
		}
		ret = ret / (((double)samplecount) - 1.0);
		return vals[0] + ret;
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
		// ret[0] = 60.0; // TODO maybe use this line if we use the arm sync to estimate the roll angle as rotation of the myo on the arm
		ret[1] = average(samples[1]);
		ret[2] = average(samples[2]);
		if (orientationprint){
			std::cout << ": orientationdata: roll: " << ret[0] << "; pitch: " << ret[1] << "; yaw: " << ret[2] << ";" << std::endl;
		}
		return ret;
	}

};