/*
 * Application Name:	FlexZone (Application)
 * File Name: 			vibe.c
 * Group: 				GroupX - FlexZone
 * Description:			Implementation file for the vibe motor thread.
 */

//**********************************************************************************
// Header Files
//**********************************************************************************
//XDCtools Header Files

//SYS/BIOS Header Files
#include <ti/sysbios/BIOS.h>				//required for BIOS_WAIT_FOREVER in Semaphore_pend();

//TI-RTOS Header Files
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

//Board Specific Header Files
#include "Board.h"

//Home brewed Header Files
#include "vibe.h"

//Standard Header Files

//**********************************************************************************
// Required Definitions
//**********************************************************************************
#define VIBE_TASK_PRIORITY					1
#ifndef VIBE_TASK_STACK_SIZE
#define VIBE_TASK_STACK_SIZE              200
#endif

//**********************************************************************************
// Global Data Structures
//**********************************************************************************
//Task Structures
Task_Struct vibeTask;
Char vibeTaskStack[VIBE_TASK_STACK_SIZE];

//Semaphore Structures
Semaphore_Struct vibeSemaphore;

//Pin stuff
PIN_Handle vibePinHandle;
PIN_State vibePinState;
PIN_Config vibePinTable[] = {
		Board_VIBE_MOTOR | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
		PIN_TERMINATE
};

uint8_t numberOfBuzz;
//**********************************************************************************
// Local Function Prototypes
//**********************************************************************************
void vibe_init();
static void vibe_taskFxn(UArg a0, UArg a1);

//**********************************************************************************
// Function Definitions
//**********************************************************************************
void buzz(uint8_t numTimes)
{
	numberOfBuzz = numTimes;
	Semaphore_post(Semaphore_handle(&vibeSemaphore));
}

/**
 * Creates Vibration Motor task.
 *
 * @param 	none
 * @return 	none
 */
void vibe_createTask()
{
    Task_Params taskParams;
	Semaphore_Params semaphoreParams;

	// Configure & construct semaphore
	Semaphore_Params_init(&semaphoreParams);
	Semaphore_construct(&vibeSemaphore, 0, &semaphoreParams);

    Task_Params_init(&taskParams);
    taskParams.stackSize = VIBE_TASK_STACK_SIZE;
    taskParams.stack = &vibeTaskStack;
    taskParams.priority = VIBE_TASK_PRIORITY;

    Task_construct(&vibeTask, (Task_FuncPtr)vibe_taskFxn, &taskParams, NULL);
}

/**
 * Initializes Vibe Motor pins.
 *
 * @param 	none
 * @return 	none
 */
void vibe_init(void)
{
	// Open GPIO pins
	vibePinHandle = PIN_open(&vibePinState, vibePinTable);
	if (!vibePinHandle) {
#if defined(USE_UART)
		Log_error0("Error initializing vibe motor pin.");
#else
		System_printf("Error initializing vibe motor pin.");
		System_flush();
#endif
	}
}

/**
 * Primary Vibe Motor task.
 *
 * @param 	none
 * @return 	none
 */
void vibe_taskFxn(UArg a0, UArg a1)
{
	int i;
	while (1)
	{
		Semaphore_pend(Semaphore_handle(&vibeSemaphore), BIOS_WAIT_FOREVER);
		for (i = 0; i < numberOfBuzz; i++) {
			PIN_setOutputValue(vibePinHandle, Board_VIBE_MOTOR, 1);
			Task_sleep((UInt) 400 * (1000 / Clock_tickPeriod));
			PIN_setOutputValue(vibePinHandle, Board_VIBE_MOTOR, 0);
			Task_sleep((UInt) 200 * (1000 / Clock_tickPeriod));
		}
	}
}
