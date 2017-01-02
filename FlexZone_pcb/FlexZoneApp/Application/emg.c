/*
 * Application Name:	FlexZone (Application)
 * File Name: 			emg.c
 * Group: 				GroupX - FlexZone
 * Description:			Implementation file the EMG Thread.
 */

//**********************************************************************************
// Header Files
//**********************************************************************************
//XDCtools Header Files
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

//SYS/BIOS Header Files
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/BIOS.h>				//required for BIOS_WAIT_FOREVER in Semaphore_pend();
#include <ti/sysbios/hal/Seconds.h>

//TI-RTOS Header Files
#include <ti/drivers/PIN.h>

//CC26XXWARE Header Files
#include <driverlib/aux_adc.h>
#include <driverlib/aux_wuc.h>

//Board Specific Header Files
#include "Board.h"
#include "emg.h"
#include "DigiPot.h"
#include "MPU9250.h"

//Standard Header Files

//**********************************************************************************
// Required Definitions
//**********************************************************************************
#define EMG_TASK_PRIORITY				   	2
#ifndef EMG_TASK_STACK_SIZE
#define EMG_TASK_STACK_SIZE               	400
#endif

#define EMG_PERIOD_IN_MS					30
#define EMG_MOVING_WINDOW					1
#define REP_THRESHHOLD_HIGH 				1600
#define REP_THRESHHOLD_LOW  				800
#define EMG_NUMBER_OF_SAMPLES_READING		4

#define STARTTIME							1412800000
#define SET_TIMEOUT							15	// in seconds
//**********************************************************************************
// Global Data Structures
//**********************************************************************************
//Task Structures
Task_Struct emgTask;
Char emgTaskStack[EMG_TASK_STACK_SIZE];

//Semaphore Structures
Semaphore_Struct emgSemaphore;

//Clock Structures
Clock_Struct emgClock;

//Global data buffer for ADC samples
uint32_t rawAdc[EMG_NUMBER_OF_SAMPLES_SLICE];
//uint32_t adjustedAdc = 0, uvAdc = 0;
uint16_t adcCounter = 0;

//EMG processing
EMG_stats emg_set_stats;
EMG_stats emg_set_statsSend;
double lastAverage=-1;
uint64_t pulseStart=0, pulseEnd=0;
uint64_t deadStart=0, deadEnd=0;
uint8_t processingDone = 1;
uint8_t inRep = 0;
uint8_t setIMUThres = 0;
uint8_t repCount = 0;
uint8_t setCount = 0;

//workout config
Workout_config myWorkoutConfig;

//EMG Pins
PIN_Handle emgPinHandle;
PIN_State emgPinState;
const PIN_Config emgPins[] = {
		Board_CH0_IN | PIN_INPUT_DIS | PIN_GPIO_OUTPUT_DIS,	// AUXIO1
		Board_CH1_IN | PIN_INPUT_DIS | PIN_GPIO_OUTPUT_DIS,	// AUXIO7
		IOID_29 | PIN_INPUT_DIS | PIN_GPIO_OUTPUT_DIS,	// AUXIO1
		PIN_TERMINATE
};

//Analog Circuit Pins
PIN_Handle analogPinHandle;
PIN_State analogPinState;
const PIN_Config analogPinTable[] = {
		Board_ANALOG_EN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
		PIN_TERMINATE
};

//Timing Stuff
uint32_t timeStart, timeEnd, timeProcessing, usProcessing;
uint32_t pulseWidth=0, deadWidth=0;
Types_FreqHz freq;


uint8_t stopEmgRequest = 0;
uint8_t emgRunning = 0;
//**********************************************************************************
// Local Function Prototypes
//**********************************************************************************
static void emg_init(void);
static void emg_taskFxn(UArg a0, UArg a1);
static void emgPoll_SwiFxn(UArg a0);
static void adc_init(void);
static uint32_t read_adc(uint8_t channel);
void analog_init(void);
void sendStructBle(void);
void sendStructBleV2(void);
void gracefulExitEmg(void);
void flushStruct(void);
//**********************************************************************************
// Function Definitions
//**********************************************************************************
/**
 * Creates EMG task and creates required semaphore for deferred interrupt processing of ADC data.
 *
 * @param 	none
 * @return 	none
 */
void emg_createTask(void) {
	Task_Params taskParams;
	Semaphore_Params semaphoreParams;

	// Configure & construct semaphore
	Semaphore_Params_init(&semaphoreParams);
	Semaphore_construct(&emgSemaphore, 0, &semaphoreParams);

	// Configure task
	Task_Params_init(&taskParams);
	taskParams.stack = emgTaskStack;
	taskParams.stackSize = EMG_TASK_STACK_SIZE;
	taskParams.priority = EMG_TASK_PRIORITY;

	//Dynamically construct task
	Task_construct(&emgTask, emg_taskFxn, &taskParams, NULL);
}

/**
 * Initializes hardware for EMG and creates required clock. Starts & configures ADC to run on DIO30 (AUXIO7).
 *
 * @param 	none
 * @return 	none
 */
static void emg_init(void) {
	adc_init();
	analog_init();
	Seconds_set(STARTTIME);

#ifndef USE_UART
	digiPot_spi_init();
#endif //USE_UART

	//Configure clock object
	Clock_Params clockParams;
	Clock_Params_init(&clockParams);
	clockParams.arg = (UArg) 1;
	clockParams.period = EMG_PERIOD_IN_MS * (1000 / Clock_tickPeriod);
	clockParams.startFlag = FALSE;	//Indicates to start immediately

	//Dynamically Construct Clock
//	Clock_construct(&emgClock, emgPoll_SwiFxn, EMG_PERIOD_IN_MS * (1000 / Clock_tickPeriod), &clockParams);
	Clock_construct(&emgClock, emgPoll_SwiFxn, 0, &clockParams);
}

/**
 * Primary EMG task. Calls function to initialize hardware once and responds to ADC samples to turn on LED1.
 *
 * @param 	none
 * @return 	none
 */
static void emg_taskFxn(UArg a0, UArg a1) {
	//Initialize required hardware & clocks for task.
	emg_init();
	uint64_t pulseTickCounter = 0, deadTickCounter = 0;
	Timestamp_getFreq(&freq);
	uint16_t pulsePeak = 0;

	uint32_t t_start, t_end;
	uint32_t lastRepTime = Seconds_get(), pulsePeakTime, pulseEndTime;
	t_start = Seconds_get();

	while (1)
	{
		//Wait for ADC poll and ADC reading
		Semaphore_pend(Semaphore_handle(&emgSemaphore), BIOS_WAIT_FOREVER);
		timeStart = Timestamp_get32();

		pulsePeak = 0;

		int i;
		for(i = 0; i < EMG_NUMBER_OF_SAMPLES_SLICE; ++i)
		{

		    //edge detection for reps
			if (rawAdc[i] >= REP_THRESHHOLD_HIGH)
			{
				//always increment the pulse tick above the threshhold
				//we know we are in a pulse
			    pulseTickCounter++;

				//start of pulse - crosses the threshhold
				if ( !inRep )//&& lastAverage < REP_THRESHHOLD_HIGH)
				{
					inRep = 1;//we are in the rep
					  setIMUThres = 1;
					  if (myWorkoutConfig.imuFeedback)
							Clock_start(Clock_handle(&accelClock));

				    pulseStart = (Timestamp_get32()/1000);

				    if ( repCount > 0 )
				    {
				    	deadWidth = deadTickCounter*EMG_PERIOD_IN_MS;
				    	emg_set_stats.deadWidth[repCount-1] = deadWidth;
				    }

				    pulseTickCounter = 0;
				}

				//calculate local extrema
				if ( rawAdc[i] > pulsePeak )
				{
					pulsePeak = rawAdc[i];
					emg_set_stats.peakIntensity[repCount] = pulsePeak;
					pulsePeakTime = (Timestamp_get32()/1000);
					emg_set_stats.concentricTime[repCount] = pulsePeakTime - pulseStart;
				}

			}

			//between thresholding levels
			//if inRep, will keep going
			//if !inRep, won't start
			else if (rawAdc[i] >= REP_THRESHHOLD_LOW)
			{
				if( inRep )
				{
					pulseTickCounter++;
				}
				else
				{
					deadTickCounter++;
				}
			}

			//below thresholding levels
			else //avg < REP_THRESHHOLD_LOW
			{
				//end of the rep
				if ( inRep )
				{
					pulseWidth = pulseTickCounter*EMG_PERIOD_IN_MS;

					// THIS IS THE END OF A DETECTED REP!
					// if-statement gets rid of questionable reps
					if(pulseWidth > 250)
					{
						inRep = 0;

						deadTickCounter = 0;

						emg_set_stats.pulseWidth[repCount] = pulseWidth;
						pulseEndTime = (Timestamp_get32()/1000);
						emg_set_stats.eccentricTime[repCount] = pulseEndTime - pulsePeakTime;
						repCount++;
						lastRepTime = Seconds_get();

#if defined(USE_UART)
						Log_info1("get big my mans: %u", repCount);
#else
						System_printf("get big my mans: %u\n", repCount);
						System_flush();
#endif // USE_UART

						if (myWorkoutConfig.imuFeedback)
							Clock_stop(Clock_handle(&accelClock));
//						user_sendEmgPacket(&repCount, 4, 0);
					}
				}
				else //!inRep
				{
					deadTickCounter++;
				}
			}
//			lastAverage = rawAdc[i];
		}//for each samples/slice

#if defined(USE_UART)
				Log_info1("\nstruct rep count: %u", emg_set_stats.numReps);
//				for(i=0; i<5; ++i){
//					Log_info1("peak intensity: %u", emg_set_stats.peakIntensity[i]);
//				}
//				for(i=0; i<5; ++i){
//					Log_info1("pulse width: %u", emg_set_stats.pulseWidth[i]);
//				}
//				for(i=0; i<5; ++i){
//					Log_info1("dead width: %u", emg_set_stats.deadWidth[i]);
//				}
//				for(i=0; i<5; ++i){
//					Log_info1("concentric time: %u", emg_set_stats.concentricTime[i]);
//				}
//				for(i=0; i<5; ++i){
//					Log_info1("eccentric time: %u", emg_set_stats.eccentricTime[i]);
//				}
				Log_info0("\n");
#endif
		//SET is DONE
		if ( (repCount > 0 && (Seconds_get() - lastRepTime) > SET_TIMEOUT) || repCount == myWorkoutConfig.targetRepCount ) {

			setCount++;
			Log_info0("set done!!!!!!!!!!!!!!!!!!!!!!!!!!");
			emg_set_stats.numReps = repCount;
			emg_set_stats.setDone = 1;

			// Reset stats, flush the struct
			repCount = 0;

			inRep = 0;

			// if imu activated, stop imu thread
			if (myWorkoutConfig.imuFeedback)
				Clock_stop(Clock_handle(&accelClock));

			//do deep copy, send data, flush struct
			sendStructBleV2();
			flushStruct();

			//haptic feedback on set completion
			if ( 1 == myWorkoutConfig.hapticFeedback)
				buzz(1);
		}//set is done

		emg_set_stats.numReps = repCount;
		processingDone = 1;

		//reset all required items and stop clock on user stop request
		if (stopEmgRequest)
			gracefulExitEmg();

		// all sets done
		if (setCount == myWorkoutConfig.targetSetCount)
		{
			gracefulExitEmg();
		}

	}//while(1)
}


/**
 * Clock callback function that runs in SWI context. Reads ADC value and posts semaphore for ADC data processing.
 *
 * @param 	none
 * @return 	none
 */
static void emgPoll_SwiFxn(UArg a0) {

	if (processingDone)
	{
		uint64_t localSum = 0;
		int i;

		//ADC Sampling
		for (i = 0; i < EMG_NUMBER_OF_SAMPLES_READING; i++)
		{
			//Read ADC
			localSum += read_adc(0);
		}

		rawAdc[adcCounter++] = localSum/EMG_NUMBER_OF_SAMPLES_READING;

//#if defined(USE_UART)
//			Log_info2("adc0: %u \t adc1: %u", rawAdc[adcCounter-1], read_adc(1));
//#else
//			System_printf("adc0: %u \t adc1: %u\n", rawAdc[adcCounter-1], read_adc(1));
//			System_flush();
//#endif // USE_UART

		if (EMG_NUMBER_OF_SAMPLES_SLICE == adcCounter)
		{
			printWorkoutConfig();
//			buzz(1);
			adcCounter = 0;
			processingDone = 0;
			//Post semaphore to emg_taskFxn
			Semaphore_post(Semaphore_handle(&emgSemaphore));
		}
	}
	else
	{
#if defined(USE_UART)
		Log_info0("Missed deadline");
#else
		System_printf("Missed deadline\n");
		System_flush();
#endif //USE_UART
	}
}

//**********************************************************************************
// Low Level Functions
//**********************************************************************************
/**
 * Initialize ADC module
 *
 * @param 	none
 * @return	none
 */
void adc_init() {
	// Set up pins
	emgPinHandle = PIN_open(&emgPinState, emgPins);

	//Initialize AUX, ADI, and ADC Clocks
	AUXWUCClockEnable(AUX_WUC_MODCLKEN0_ANAIF_M | AUX_WUC_MODCLKEN0_AUX_ADI4_M);

	//Configure ADC to use DIO7 (AUXIO7) on manual trigger.
	AUXADCSelectInput(BOARD_CH0_AUX);
}

/**
 * Wrapper function to read ADC on configured Analog Pin.
 *
 * @param 	none
 * @return	none
 */
uint32_t read_adc(uint8_t channel) {
	uint32_t temp;

	if (0 == channel)
		AUXADCSelectInput(BOARD_CH0_AUX);
	else
		AUXADCSelectInput(BOARD_CH1_AUX);

	//Enable ADC
	AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_10P6_US, AUXADC_TRIGGER_MANUAL);

	//Disallow STANDBY mode while reading
	Power_setConstraint(Power_SB_DISALLOW);
	AUXADCGenManualTrigger();
	temp = AUXADCReadFifo();
	Power_releaseConstraint(Power_SB_DISALLOW);

	//Disable ADC when read complete
	AUXADCDisable();

	return temp;
}

/**
 * Sets DIO1 high to power on analog circuit.
 *
 * @param 	none
 * @return	none
 */
void analog_init() {
	analogPinHandle = PIN_open(&analogPinState, analogPinTable);
    PIN_setOutputValue(analogPinHandle, Board_ANALOG_EN, 0);
}


void sendStructBle(void) {
	int i=0;
	for (i=0; i<EMG_MAX_REPS;i++ )
	{
		emg_set_statsSend.pulseWidth[i] = emg_set_stats.pulseWidth[i];
		emg_set_statsSend.deadWidth[i] = emg_set_stats.deadWidth[i];
		emg_set_statsSend.concentricTime[i] = emg_set_stats.concentricTime[i];
		emg_set_statsSend.eccentricTime[i] = emg_set_stats.eccentricTime[i];
		emg_set_statsSend.peakIntensity[i] = emg_set_stats.peakIntensity[i];
		emg_set_statsSend.movedOrNah[i] = emg_set_stats.movedOrNah[i];
	}
	emg_set_statsSend.numReps = emg_set_stats.numReps;
	emg_set_statsSend.setDone = emg_set_stats.setDone;

	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.pulseWidth, 38, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.deadWidth, 38, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.concentricTime, 38, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.eccentricTime, 38, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.peakIntensity, 38, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.movedOrNah, 21, 0);
}

void sendStructBleV2(void) {
	int i=0;
	for (i=0; i<EMG_MAX_REPS;i++ )
	{
		emg_set_statsSend.pulseWidth[i] = emg_set_stats.pulseWidth[i];
		emg_set_statsSend.deadWidth[i] = emg_set_stats.deadWidth[i];
		emg_set_statsSend.concentricTime[i] = emg_set_stats.concentricTime[i];
		emg_set_statsSend.eccentricTime[i] = emg_set_stats.eccentricTime[i];
		emg_set_statsSend.peakIntensity[i] = emg_set_stats.peakIntensity[i];
		emg_set_statsSend.movedOrNah[i] = emg_set_stats.movedOrNah[i];
	}
	emg_set_statsSend.numReps = emg_set_stats.numReps;
	emg_set_statsSend.setDone = emg_set_stats.setDone;

	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.pulseWidth[0], 20, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.pulseWidth[10], 18, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.deadWidth[0], 20, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.deadWidth[10], 18, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.concentricTime[0], 20, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.concentricTime[10], 18, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.eccentricTime[0], 20, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.eccentricTime[10], 18, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.peakIntensity[0], 20, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.peakIntensity[10], 18, 0);
	user_sendEmgPacket((uint8_t*)&emg_set_statsSend.movedOrNah[0], 21, 0);
}

void gracefulExitEmg(void) {

	stopEmgRequest = 0;
	//clear set buffer
	//reset adcCounter and repCount
	adcCounter = 0;
	repCount = 0;
	processingDone = 1;
	emgRunning = 0;
	inRep = 0;
	flushStruct();

	if (myWorkoutConfig.imuFeedback)
		Clock_stop(Clock_handle(&accelClock));
	Clock_stop(Clock_handle(&emgClock));
}

void flushStruct(void) {
	int resetCnt;
	for (resetCnt = 0; resetCnt<EMG_MAX_REPS; resetCnt++){
		emg_set_stats.pulseWidth[resetCnt] = 0;
		emg_set_stats.deadWidth[resetCnt] = 0;
		emg_set_stats.concentricTime[resetCnt] = 0;
		emg_set_stats.eccentricTime[resetCnt] = 0;
		emg_set_stats.peakIntensity[resetCnt] = 0;
	}

	emg_set_stats.numReps = 0;
	emg_set_stats.setDone = 0;
}
