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
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

//SYS/BIOS Header Files
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/BIOS.h>				//required for BIOS_WAIT_FOREVER in Semaphore_pend();

//TI-RTOS Header Files
#include <ti/drivers/PIN.h>

//CC26XXWARE Header Files
#include <driverlib/aux_adc.h>
#include <driverlib/aux_wuc.h>

//Board Specific Header Files
#include "Board.h"
#include "emg.h"

//Standard Header Files
//#include <stdio.h>	// used for printf()
//**********************************************************************************
// Required Definitions
//**********************************************************************************
#define EMG_TASK_PRIORITY				   	2
#ifndef EMG_TASK_STACK_SIZE
#define EMG_TASK_STACK_SIZE               	800
#endif

#define EMG_PERIOD_IN_MS					20
#define EMG_MOVING_WINDOW					1
#define REP_THRESHHOLD_HIGH 				1900
#define REP_THRESHHOLD_LOW  				900
#define EMG_NUMBER_OF_SAMPLES_READING		4
//**********************************************************************************
// Global Data Structures
//**********************************************************************************
//Task Structures
Task_Struct emgTask;
Char emgTaskStack[EMG_TASK_STACK_SIZE];

//Semaphore Structures
Semaphore_Struct emgSemaphore;

//Clock Structures
static Clock_Struct emgClock;

//Global data buffer for ADC samples
uint32_t rawAdc[EMG_NUMBER_OF_SAMPLES_SLICE];
//uint32_t adjustedAdc = 0, uvAdc = 0;
uint16_t adcCounter = 0;

//EMG processing
EMG_stats emgSets[10];
EMG_stats emg_set_stats;
double lastAverage=-1;
uint64_t pulseStart=0, pulseEnd=0;
uint64_t deadStart=0, deadEnd=0;
uint8_t processingDone = 1;
uint8_t inRep = 0;

//EMG Pins
PIN_Handle emgPinHandle;
PIN_State emgPinState;
const PIN_Config emgPins[] = {
Board_DIO23_ANALOG | PIN_INPUT_DIS | PIN_GPIO_OUTPUT_DIS,
PIN_TERMINATE };

//Timing Stuff
uint32_t timeStart, timeEnd, timeProcessing, usProcessing;
uint32_t pulseWidth=0, deadWidth=0;
Types_FreqHz freq;

//**********************************************************************************
// Local Function Prototypes
//**********************************************************************************
static void emg_init(void);
static void emg_taskFxn(UArg a0, UArg a1);
static void emgPoll_SwiFxn(UArg a0);
static void adc_init(void);
static uint32_t read_adc(void);

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

	//Configure clock object
	Clock_Params clockParams;
	Clock_Params_init(&clockParams);
	clockParams.arg = (UArg) 1;
	clockParams.period = EMG_PERIOD_IN_MS * (1000 / Clock_tickPeriod);
	clockParams.startFlag = TRUE;	//Indicates to start immediately

	//Dynamically Construct Clock
	Clock_construct(&emgClock, emgPoll_SwiFxn, EMG_PERIOD_IN_MS * (1000 / Clock_tickPeriod), &clockParams);
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
	uint32_t repCount = 0;
	uint16_t pulsePeak = 0;

	while (1)
	{
		//Wait for ADC poll and ADC reading
		Semaphore_pend(Semaphore_handle(&emgSemaphore), BIOS_WAIT_FOREVER);
		timeStart = Timestamp_get32();
		int i;

		for(i = 0; i < EMG_NUMBER_OF_SAMPLES_SLICE; ++i)
		{
		    //edge detection for reps
			if (rawAdc[i] >= REP_THRESHHOLD_HIGH)
			{
				//start of pulse
				if (lastAverage >= REP_THRESHHOLD_LOW)
				{
				  inRep = 1;

				  deadWidth = deadTickCounter*EMG_PERIOD_IN_MS;
				  emg_set_stats.deadWidth[repCount] = deadWidth;
				  deadTickCounter = 0;

				  pulseTickCounter++;
				}

				//calculate local extrema
				if ( rawAdc[i] > pulsePeak )
				{
//					pulsePeak = rawAdc[i];
				}
				else
				{
//					emg_set_stats.concentricTime[repCount] = millis() - pulseStart;
				}
			}
			//still above thresholding levels
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
			else //avg < REP_THRESHHOLD_LOW
			{
				//end of pulse
				if(lastAverage > 0)
				{
					inRep = 0;

					pulseWidth = pulseTickCounter*EMG_PERIOD_IN_MS;
					pulseTickCounter = 0;

					deadTickCounter++;

					//get rid of questionable reps
					if(pulseWidth > 250)
					{
						emg_set_stats.pulseWidth[repCount] = pulseWidth;
						repCount++;

						Log_info1("get big my mans: %u", (IArg) repCount);
						user_sendEmgPacket(&repCount, 4, 0);
						PIN_setOutputValue(ledPinHandle, Board_LED1, !PIN_getOutputValue(Board_LED1));
					}
				}
				rawAdc[i] = 0;
			}
			lastAverage = rawAdc[i];
		}

		emg_set_stats.numReps = repCount;
		processingDone = 1;

		timeEnd = Timestamp_get32();
		timeProcessing = timeEnd - timeStart;
		usProcessing = (timeProcessing*1000000)/freq.lo;
		Log_info1("EMG Thread: Processing Time = %u", (IArg)usProcessing);


//		// Calculate adjusted & microvolt ADC values
//		//adjustedAdc = AUXADCAdjustValueForGainAndOffset(rawAdc, AUXADCGetAdjustmentGain(AUXADC_REF_FIXED), AUXADCGetAdjustmentOffset(AUXADC_REF_FIXED));
//		//uvAdc = AUXADCValueToMicrovolts(AUXADC_FIXED_REF_VOLTAGE_NORMAL, adjustedAdc);
////		Log_info1("EMG Thread: ADC result = %u", (IArg)uvAdc);
	}
}

/**
 * Clock callback function that runs in SWI context. Reads ADC value and posts semaphore for ADC data processing.
 *
 * @param 	none
 * @return 	none
 */
static void emgPoll_SwiFxn(UArg a0) {
	//Log_info1("EMG Thread: ADC result = %u", 100);

	if (processingDone)
	{
		uint64_t localSum = 0;
		int i;

		for (i = 0; i < EMG_NUMBER_OF_SAMPLES_READING; i++)
		{
			//Read ADC
			localSum += read_adc();
		}

		rawAdc[adcCounter++] = localSum/EMG_NUMBER_OF_SAMPLES_READING;
		//Log_info1("EMG Thread: ADC result = %u", (IArg)rawAdc[adcCounter-1]);

		if (EMG_NUMBER_OF_SAMPLES_SLICE == adcCounter)
		{
			adcCounter = 0;
			processingDone = 0;
			//Post semaphore to emg_taskFxn
			Semaphore_post(Semaphore_handle(&emgSemaphore));
		}
	}
	else
	{
		Log_info0("Missed deadline");
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

	//Old clock stuff
//	AUXWUCClockEnable(AUX_WUC_SOC_CLOCK | AUX_WUC_ADI_CLOCK | AUX_WUC_ADC_CLOCK);
//
//	//Wait for ADC clock to be ready
//	while (AUX_WUC_CLOCK_READY != AUXWUCClockStatus(AUX_WUC_ADC_CLOCK));

	//Configure ADC to use DIO23 (AUXIO7) on manual trigger.
	AUXADCSelectInput(ADC_COMPB_IN_AUXIO7);
}

/**
 * Wrapper function to read ADC on configured Analog Pin.
 *
 * @param 	none
 * @return	none
 */
uint32_t read_adc() {
	uint32_t temp;

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
