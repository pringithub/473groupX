/*
 * Application Name:	FlexZone (Application)
 * File Name: 			heartbeat.c
 * Group: 				GroupX - FlexZone
 * Description:			Implementation file the Heartbeat Thread. Runs at Priority 2.
 */

//===============================================
//================ Header Files =================
//===============================================
//XDCtools Header Files
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

//SYS/BIOS Header Files
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>

//TI-RTOS Header Files
#include <ti/drivers/PIN.h>

//Board Specific Header Files
#include "Board.h"

//Standard Header Files
//#include <stdio.h>	// used for printf()

//===============================================
//============ Required Definitions =============
//===============================================
#define HRB_TASK_PRIORITY				   	2
#ifndef HRB_TASK_STACK_SIZE
#define HRB_TASK_STACK_SIZE              	800
#endif

#define PERIOD_IN_MS						1000

//===============================================
//=========== Global Data Structures ============
//===============================================
Task_Struct hrbTask;
Char hrbTaskStack[HRB_TASK_STACK_SIZE];

//Pin driver handles
static PIN_Handle hrbGpioPinHandle;

//Global memory storage for a PIN_Config table
static PIN_State hrbGpioPinState;

//Initial LED pin configuration table
PIN_Config hrbGpioPinTable[] = {
Board_DIO0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
PIN_TERMINATE };

// Clock object for heartbeat
static Clock_Struct heartbeatClock;

//===============================================
//======== Local Function Prototypes ============
//===============================================
static void heartbeat_init(void);
static void greenHeartbeat_taskFxn(UArg a0, UArg a1);
static void redHeartbeat_SwiFxn(UArg a0);

//===============================================
//=========== Function Definitions ==============
//===============================================
/**
 * Creates Heartbeat task running at Priority 2.
 *
 * @param 	none
 * @return 	none
 */
void heartbeat_createTask(void) {
	Task_Params taskParams;

	// Configure task
	Task_Params_init(&taskParams);
	taskParams.stack = hrbTaskStack;
	taskParams.stackSize = HRB_TASK_STACK_SIZE;
	taskParams.priority = HRB_TASK_PRIORITY;
	taskParams.arg0 = 1000 * (1000 / Clock_tickPeriod);

	//Dynamically construct task
	Task_construct(&hrbTask, greenHeartbeat_taskFxn, &taskParams, NULL);
}

/**
 * Initializes hardware for heartbeat and creates required clock.
 *
 * @param 	none
 * @return 	none
 */
static void heartbeat_init(void) {
	// Open GPIO pins
	hrbGpioPinHandle = PIN_open(&hrbGpioPinState, hrbGpioPinTable);
	if (!hrbGpioPinHandle) {
		Log_error0("Error initializing board Heartbeat GPIO pins");
		Task_exit();
	}

	// Create the clock object
	Clock_Params clockParams;
	Clock_Params_init(&clockParams);
	clockParams.arg = (UArg) 1;
	clockParams.period = PERIOD_IN_MS * (1000 / Clock_tickPeriod);
	clockParams.startFlag = TRUE;

	//Dynamically construct task and assign callback function
	Clock_construct(&heartbeatClock, redHeartbeat_SwiFxn,
			PERIOD_IN_MS * (1000 / Clock_tickPeriod), &clockParams);
}


/**
 * Primary heartbeat task. Calls function to initialize hardware once and blinks green LED at specified period.
 *
 * @param 	none
 * @return 	none
 */
static void greenHeartbeat_taskFxn(UArg a0, UArg a1) {
	heartbeat_init();

	while (1) {
		Task_sleep((UInt) a0);
		PIN_setOutputValue(hrbGpioPinHandle, Board_DIO0,
				!PIN_getOutputValue(Board_DIO0));
	}
}

/**
 * Clock callback function that runs in SWI context. Does nothing in this case.
 *
 * @param 	none
 * @return 	none
 */
static void redHeartbeat_SwiFxn(UArg a0) {
	//PIN_setOutputValue(ledPinHandle, Board_LED0, !PIN_getOutputValue(Board_LED0));
}