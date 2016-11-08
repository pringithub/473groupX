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
#include <xdc/runtime/Diags.h>

//SYS/BIOS Header Files
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
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
#define EMG_TASK_PRIORITY				   	1
#ifndef EMG_TASK_STACK_SIZE
#define EMG_TASK_STACK_SIZE               	800
#endif

#define EMG_PERIOD_IN_MS					200

//===============================================
//=========== Global Data Structures ============
//===============================================
//Pin driver handles
static PIN_Handle emgGpioPinHandle;
static PIN_Handle ledGreenPinHandle;

//Global memory storage for a PIN_Config table
static PIN_State emgGpioPinState;
static PIN_State ledGreenPinState;

//Initial LED pin configuration table
//Initial onboard LED pin configuration table
PIN_Config ledGreenPinTable[] = {
Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
PIN_TERMINATE };

PIN_Config emgGpioPinTable[] = {
Board_DIO30_ANALOG | PIN_INPUT_DIS | PIN_GPIO_OUTPUT_DIS,
PIN_TERMINATE };

//Task Structures
Task_Struct emgTask;
Char emgTaskStack[EMG_TASK_STACK_SIZE];

//Semaphore Structures
Semaphore_Struct emgSemaphore;

//Clock Structures
static Clock_Struct emgClock;

//Global data buffer for ADC samples
uint32_t adc = 0;

//**********************************************************************************
// Local Function Prototypes
//**********************************************************************************
static void emg_init(void);
static void emg_taskFxn(UArg a0, UArg a1);
static void emgPoll_SwiFxn(UArg a0);

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
	// Open GPIO pins
	emgGpioPinHandle = PIN_open(&emgGpioPinState, emgGpioPinTable);
	if (!emgGpioPinHandle) {
		Log_error0("Error initializing board EMG GPIO pins");
		Task_exit();
	}

//	ledGreenPinHandle = PIN_open(&ledGreenPinState, ledGreenPinTable);
//	if (!ledGreenPinHandle) {
//		Log_error0("Error initializing onboard LED pins");
//		Task_exit();
//	}

	//Initialize AUX, ADI, and ADC Clocks
	AUXWUCClockEnable(AUX_WUC_SOC_CLOCK);
	AUXWUCClockEnable(AUX_WUC_ADI_CLOCK);
	AUXWUCClockEnable(AUX_WUC_ADC_CLOCK);

	//Wait for ADC clock to be ready
	while (AUX_WUC_CLOCK_READY != AUXWUCClockStatus(AUX_WUC_ADC_CLOCK)) {
	}

	//Configure ADC to use DIO30 (AUXIO7) on manual trigger.
	AUXADCSelectInput(ADC_COMPB_IN_AUXIO7);
	AUXADCEnableSync(AUXADC_REF_FIXED, AUXADC_SAMPLE_TIME_10P9_MS, AUXADC_TRIGGER_MANUAL);
	//AUXADCFlushFifo();

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

	while (1) {
		//Wait for ADC poll and ADC reading
		Semaphore_pend(Semaphore_handle(&emgSemaphore), BIOS_WAIT_FOREVER);
        Log_info1("EMG Thread: ADC result = %d", (IArg)adc);
//		//blink LED based on ADC value
//		if (adc > 2000) {
//			PIN_setOutputValue(ledGreenPinHandle, Board_LED1, 1);
//		} else {
//			PIN_setOutputValue(ledGreenPinHandle, Board_LED1, 0);
//		}
	}
}

/**
 * Clock callback function that runs in SWI context. Reads ADC value and posts semaphore for ADC data processing.
 *
 * @param 	none
 * @return 	none
 */
static void emgPoll_SwiFxn(UArg a0) {
	//Generate manual ADC trigger
	AUXADCGenManualTrigger();
	adc = AUXADCReadFifo();

	//Post semaphore to emg_taskFxn
	Semaphore_post(Semaphore_handle(&emgSemaphore));
}
