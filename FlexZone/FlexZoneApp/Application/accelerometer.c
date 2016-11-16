/*
 * Application Name:	FlexZone (Application)
 * File Name: 			accelerometer.c
 * Group: 				GroupX - FlexZone
 * Description:			Implementation file for the accelerometer thread.
 */

//**********************************************************************************
// Header Files
//**********************************************************************************
//XDCtools Header Files
#include <xdc/runtime/Log.h>

//SYS/BIOS Header Files
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>				//required for BIOS_WAIT_FOREVER in Semaphore_pend();

//TI-RTOS Header Files
#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>
#include <ti/drivers/dma/UDMACC26XX.h>

//Board Specific Header Files
#include "Board.h"

//Home brewed Header Files
#include "accelerometer.h"
#include "MPU9250.h"

//Standard Header Files
#include <string.h>
//#include <stdio.h>

//**********************************************************************************
// Required Definitions
//**********************************************************************************
#define ACCEL_TASK_PRIORITY					1
#ifndef ACCEL_TASK_STACK_SIZE
#define ACCEL_TASK_STACK_SIZE              800
#endif

#define ACCEL_PERIOD_IN_MS					200


//**********************************************************************************
// Global Data Structures
//**********************************************************************************
//Task Structures
Task_Struct accelTask;
Char accelTaskStack[ACCEL_TASK_STACK_SIZE];

//Semaphore Structures
Semaphore_Struct accelSemaphore;

//Clock Structures
static Clock_Struct accelClock;

Accel_State myAccel;

//**********************************************************************************
// Local Function Prototypes
//**********************************************************************************
void accel_init();
static void accel_taskFxn(UArg a0, UArg a1);
static void accel_SwiFxn(UArg a0);

//**********************************************************************************
// Function Definitions
//**********************************************************************************
/**
 * Creates Accelerometer task running at and creates required semaphore for deferred interrupt processing of accelerometer data.
 *
 * @param 	none
 * @return 	none
 */
void accel_createTask(void) {
	Task_Params taskParams;
	Semaphore_Params semaphoreParams;

	// Configure & construct semaphore
	Semaphore_Params_init(&semaphoreParams);
	Semaphore_construct(&accelSemaphore, 0, &semaphoreParams);

	// Configure task
	Task_Params_init(&taskParams);
	taskParams.stack = accelTaskStack;
	taskParams.stackSize = ACCEL_TASK_STACK_SIZE;
	taskParams.priority = ACCEL_TASK_PRIORITY;

	//Dynamically construct task
	Task_construct(&accelTask, accel_taskFxn, &taskParams, NULL);
}

/**
 * Initialize Accelerometer module
 *
 * @param 	none
 * @return	none
 */
void accel_init()
{
	mpu_spi_init();

	//Configure clock object
	Clock_Params clockParams;
	Clock_Params_init(&clockParams);
	clockParams.arg = (UArg) 1;
	clockParams.period = ACCEL_PERIOD_IN_MS * (1000 / Clock_tickPeriod);
	clockParams.startFlag = TRUE;	//Indicates to start immediately

	//Dynamically Construct Clock
	Clock_construct(&accelClock, accel_SwiFxn, ACCEL_PERIOD_IN_MS * (1000 / Clock_tickPeriod), &clockParams);
}

/**
 * Primary Accelerometer task. Calls function to initialize hardware once and samples Accelerometer via SPI0.
 *
 * @param 	none
 * @return 	none
 */
static void accel_taskFxn(UArg a0, UArg a1) {
	//Initialize required hardware & clocks for task.
	accel_init();

	while (1) {
		//Wait for SPI Semaphore
		Semaphore_pend(Semaphore_handle(&accelSemaphore), BIOS_WAIT_FOREVER);
		myAccel.ACCEL_X = read_MPU(X_AXIS, ACCEL);
		myAccel.ACCEL_Y = read_MPU(Y_AXIS, ACCEL);
		myAccel.ACCEL_Z = read_MPU(Z_AXIS, ACCEL);
		myAccel.GYRO_X = read_MPU(X_AXIS, GYRO);
		myAccel.GYRO_Y = read_MPU(Y_AXIS, GYRO);
		myAccel.GYRO_Z = read_MPU(Z_AXIS, GYRO);
        Log_info3("Accel Thread: ACCEL (XYZ) \t%d\t%d\t%d", (IArg)myAccel.ACCEL_X, (IArg)myAccel.ACCEL_Y, (IArg)myAccel.ACCEL_Z);
//        Log_info3("Accel Thread:  GYRO (XYZ) \t%d\t%d\t%d", (IArg)xg_res, (IArg)yg_res, (IArg)zg_res);
	}
}

/**
 * Clock callback function that runs in SWI context.
 *
 * @param 	none
 * @return 	none
 */
static void accel_SwiFxn(UArg a0) {
	//Post semaphore to accel_taskFxn
	Semaphore_post(Semaphore_handle(&accelSemaphore));
}

