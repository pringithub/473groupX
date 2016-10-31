#ifndef EMG_H
#define EMG_H



struct EMG_stats {
	double pulseWidth[20];
	double deadWidth[20];
	//time from start of rep to peak
	double concentricTime[20]; 
	//time from peak of rep to end
	double eccentricTime[20];
	int numReps;
};



static struct EMG_params {
	double upThreshhold;
	double downThreshhold;
	int movingWindowSize; 
};



EMG_stats* getSetStats(EMG_params& my_params);
void sendSetPacket(EMG_params& my_params);
//void setGain(int gain);


#endif //EMG_H