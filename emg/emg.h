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

EMG_stats* getSetStats();
void setRepThreshhold(double threshhold);
void setMovingAverageWindow(int windowSize);//do I even want this??
//void setGain(int gain);


#endif //EMG_H