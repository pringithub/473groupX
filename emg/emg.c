/* Interface for the EMG 


  As EMG is a wily beast and not exactly linear wrt
    force exerted, the rectified signal we get is not 
    completely sinusoidal.

  We attempt to get a more perfectly sinusoidal signal
    by applying a smoothing filter (moving average filter).



  TODO: make sure timing works
  		figure out how to get the analog value

*/


#include <emg.h>
#include <time.h>

EMG_stats my_stats;

#define EMG_addr 0x00 //fill in later
#define TIME_AFTER_SET 1000 //ms after set completes


static double lastAverage = -1;


EMG_stats* getSetStats() {
	double pulsePeak;
	clock_t pulseStart, pulseEnd;
	clock_t deadStart, deadEnd;
	int repCount=0;


	//this ends after a determined amount of time after the 
	//user has not lifted
	while ( my_stats.deadWidth > TIME_AFTER_SET ) {

		pulsePeak=0;

		double average = getFilteredAverage();


		//edge detection for repCount
		if ( average >= repThreshhold ) {

			//start of pulse
			if ( lastAverage == -1 ) {
				pulseStart = clock();
				my_stats.deadWidth[repCount] = 
						( repCount > 0 ) ? pulseStart - pulseEnd : 0;
				
			}

			//get the max for the set
			if ( average > pulsePeak ) {
				pulsePeak = average;
			}
			else { //average < pulsePeak, starting to go down
				concentricTime[repCount] = clock() - pulseStart;
			}

		}

		else {//average < REP_THRESHHOLD

			//end of pulse
			if ( lastAverage > -1 ) {
				pulseEnd = clock();
				my_stats.pulseWidth = pulseEnd - pulseStart;
			}

			average = -1;
			repCount++;
		}
	}
	

	my_stats.numReps = repCount;

	return &my_stats;	
}


double getFilteredAverage() {

	double timeslice[movingWindowSize];

	for ( int i=0; i<movingWindowSize; ++i ) {
		//read input value somehow
		int sensorVal = ?????;
		// convert analog readin somehow
		double voltage = sensorVal * ????;
		timeslice[i] = voltage;
	}

	//find the average
	double average = 0;
	for ( int i=0; i<movingWindowSize; ++i ) {
		average += timelice[i];
	}
	average /= movingWindowSize;

	return average;
}