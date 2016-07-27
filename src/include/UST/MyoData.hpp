
#include <iostream>
#include <stdexcept>
#include <vector>
#include <myo/cxx/Vector3.hpp>
#include <myo/myo.hpp>

class MyoData {

private:

	// debug prints
	const bool emgprint = false;
	const bool orientationprint = false;
	const bool countprint = false;
	const bool otherprints = true;
	const bool sampleprint = true;

	// threshold values for hold detection
	// these 3 values are experimental, they may or may not be correct (but this worked best for me), probably they need some recalibration if someone else uses the myo
	const int muscleactvitythreshold = 150; // maybe 150 is best choice, but 100 and 125 also worked well
	const int releasethreshold = 5;
	const int holdingthreshold = 10;

	const double armlength = 25.0; //[cm] distance from myo on the arm to the middle of the hand

	myo::Myo* myoDevice;
	myo::Arm arm;

	char* armString;

	bool holding = false; // bool flag shows if the person grabs a hold at the moment
	// used for counting the times of high low muscle actvity in pending and release phase
	int releasecount = 0;
	int holdingcount = -1;

	int samplepointer = 0;
	double** samples;
	const int samplecount = 10;

	double pitch_wall = 0.0;
	double yaw_wall = 0.0;
	double roll_wall = 0.0;

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

	double DegToRad(double deg){
		return (deg * (M_PI / ((double)180.0)));
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

	double average(double* vals, bool noprint){
		double ret = 0.0;
		for (int i = 0; i < samplecount; i++){
			if (!noprint && sampleprint){
				std::cout << "sampling data print: " << vals[i] << std::endl;
			}
			ret += vals[i];
		}
		ret = ret / ((double)samplecount);
		return ret;
	}

	double modulo(double d){
		if (d > 180.0){
			return d - 360.0;
		}
		if (d < -180.0){
			return d + 360.0;
		}
		return d;
	}

public:
	MyoData(myo::Myo* dev, myo::Arm a, double* dArr){
		myoDevice = dev;
		arm = a;
		armString = armToString(arm);

		roll_wall = dArr[0];
		pitch_wall = dArr[1];
		yaw_wall = dArr[2],

		samples = new double*[3];
		for (int i = 0; i < 3; i++){
			samples[i] = new double[samplecount];
		}
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
					myoDevice->vibrate(myoDevice->vibrationShort);
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

	// TODO not sure if we need this method
	void setRoll(double r){
		roll_wall = r;
	}
	//
	
	void OrientationData(double roll, double pitch, double yaw){
		samples[0][samplepointer] = roll;
		samples[1][samplepointer] = pitch;
		samples[2][samplepointer] = yaw;
		if (samplepointer == samplecount - 1){
			samplepointer = 0;
			std::cout << armString << ": orientationdata: roll: " << roll << "; pitch: " << pitch << "; yaw: " << yaw << ";" << std::endl;
		}
		else{
			samplepointer++;
		}
	}
	double* getOrientationData(){
		return getOrientationData(false);
	}
	double* getOrientationData(bool noprint){
		double* ret = new double[3];
		if (!noprint && sampleprint){
			std::cout << "printing roll samples" << std::endl;
		}
		ret[0] = modulo(average(samples[0], noprint) - roll_wall);
		if (!noprint && sampleprint){
			std::cout << "printing pitch samples" << std::endl;
		}
		ret[1] = modulo(average(samples[1], noprint) - pitch_wall);
		/*if (ret[1] < 0){
			ret[0] = ret[0] + 2 * roll_wall;
		}*/
		if (!noprint && sampleprint){
			std::cout << "printing yaw samples" << std::endl;
		}
		ret[2] = modulo(average(samples[2], noprint) - yaw_wall);
		using std::atan2;
		using std::sin;
		using std::cos;
		using std::sqrt;
		//double offsetx = sin(DegToRad(ret[2])) * armlength;
		double x = 0.0;
		double y = 0.0;
		if (ret[1] > 80){
			x = cos(DegToRad(ret[0] - ret[2]));
			y = -sin(DegToRad(ret[0] - ret[2]));
		}
		else if(ret[1] < -80){
			x = -cos(DegToRad(ret[0] + ret[2]));
			y = -sin(DegToRad(ret[0] + ret[2]));
		}
		else{
			// yaw
			double sinAlpha = sin(DegToRad(ret[2]));
			double cosAlpha = cos(DegToRad(ret[2]));
			// pitch
			double sinBeta = sin(DegToRad(ret[1]));
			double cosBeta = cos(DegToRad(ret[1]));
			// roll
			double sinGamma = sin(DegToRad(ret[0]));
			double cosGamma = cos(DegToRad(ret[0]));

			//double x = armlength * (sinAlpha * cosBeta * cosGamma + sinBeta * sinGamma);
			//double y = armlength * (sinAlpha * cosBeta * sinGamma - sinBeta * cosGamma);
			//double z = armlength * (cosAlpha * cosBeta);

			x = armlength * (cosAlpha * sinBeta * cosGamma + sinAlpha * sinGamma);
			y = armlength * (sinAlpha * sinBeta * cosGamma - cosAlpha * sinGamma);
			double z = armlength * (cosBeta * cosGamma);

			double xpart1 = sin(DegToRad(ret[2])) * armlength;
			double xpart2 = sin(DegToRad(ret[0])) * armlength;
			double cosPitch = (cos(DegToRad(((double)2.0) * ret[1])) + ((double)1.0)) / ((double)2.0);
			double offsetx = (cosPitch * xpart1) + ((((double)1.0) - cosPitch) * xpart2);

			double offsety = sin(DegToRad(ret[1])) * armlength;
			//ret2[0] = offsetx;
			//ret2[1] = offsety;
		}
		double* ret2 = new double[2];
		ret2[0] = x;
		ret2[1] = y;
/*		double distance = sqrt((offsety * offsety) + (offsetx * offsetx));
		double angle = 0.0;
		if (offsety >= 0){
			if (offsetx >= 0){
				angle = atan2(offsety, offsetx);
			}
			else{
				angle = 90.0 + atan2(offsety, -offsetx);
			}
		}
		else{
			if (offsetx >= 0){
				angle = 270 + atan2(-offsety, offsetx);
			}
			else{
				angle = 180.0 + atan2(-offsety, -offsetx);
			}
		}*/
		if (orientationprint){
			std::cout << armString << ": orientationdata: roll: " << ret[0] << "; pitch: " << ret[1] << "; yaw: " << ret[2] << ";" << std::endl;
			//std::cout << armString << ": orientationdata offset: sp: " << offsety << "; sy: " << offsetx << ";" << std::endl;
			std::cout << armString << ": orientationdata offset: x: " << x << "; y: " << y << ";" << std::endl;
		}
		return ret2;
	}

};