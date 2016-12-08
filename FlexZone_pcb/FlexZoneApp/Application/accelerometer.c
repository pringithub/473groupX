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

//SYS/BIOS Header Files
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>				//required for BIOS_WAIT_FOREVER in Semaphore_pend();

//TI-RTOS Header Files

//Board Specific Header Files
#include "Board.h"

//Home brewed Header Files
#include "accelerometer.h"
#include "FlexZoneGlobals.h"
#include "MPU9250.h"

//Standard Header Files

//**********************************************************************************
// Required Definitions
//**********************************************************************************
#define ACCEL_TASK_PRIORITY					1
#ifndef ACCEL_TASK_STACK_SIZE
#define ACCEL_TASK_STACK_SIZE              400
#endif

#define ACCEL_PERIOD_IN_MS					300


//**********************************************************************************
// Global Data Structures
//**********************************************************************************
//Task Structures
Task_Struct accelTask;
Char accelTaskStack[ACCEL_TASK_STACK_SIZE];

//Semaphore Structures
Semaphore_Struct accelSemaphore;

//Clock Structures
Clock_Struct accelClock;

//Accel_State myAccel;
Accel_State reset_myAccel;

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
	mpu_i2c_init();

	//Configure clock object
	Clock_Params clockParams;
	Clock_Params_init(&clockParams);
	clockParams.period = ACCEL_PERIOD_IN_MS * (1000 / Clock_tickPeriod);
	clockParams.startFlag = FALSE;	//Indicates to start immediately

	//Dynamically Construct Clock
	Clock_construct(&accelClock, accel_SwiFxn, 0, &clockParams);
}

/**
 * Primary Accelerometer task. Calls function to initialize hardware once and samples Accelerometer via SPI0.
 *
 * @param 	none
 * @return 	none
 */
static void accel_taskFxn(UArg a0, UArg a1) {
	uint8_t accel_range_mask=0;

	//Initialize required hardware & clocks for task.
	accel_init();

	while (1) {
		//Wait for Accelerometer Semaphore
		Semaphore_pend(Semaphore_handle(&accelSemaphore), BIOS_WAIT_FOREVER);
//		myAccel.ACCEL_X = read_MPU(X_AXIS, ACCEL);
//		myAccel.ACCEL_Y = read_MPU(Y_AXIS, ACCEL);
//		myAccel.ACCEL_Z = read_MPU(Z_AXIS, ACCEL);
//		myAccel.GYRO_X = read_MPU(X_AXIS, GYRO);
//		myAccel.GYRO_Y = read_MPU(Y_AXIS, GYRO);
//		myAccel.GYRO_Z = read_MPU(Z_AXIS, GYRO);
//#if defined(USE_UART)
//		Log_info3("Accel Thread: ACCEL (XYZ) \t%d\t%d\t%d", (IArg)myAccel.ACCEL_X, (IArg)myAccel.ACCEL_Y, (IArg)myAccel.ACCEL_Z);
//#else
////		System_printf("Accel Thread: ACCEL (XYZ) \t%d\t%d\t%d\t%d\n", myAccel.ACCEL_X, myAccel.ACCEL_Y, myAccel.ACCEL_Z, i2cRead(0x75));
////		System_printf("Whoami: %d\n", i2cRead(0x75));
////		System_flush();
//#endif // USE_UART
		reset_myAccel.ACCEL_X = read_MPU(X_AXIS, ACCEL);
		reset_myAccel.ACCEL_Y = read_MPU(Y_AXIS, ACCEL);
		reset_myAccel.ACCEL_Z = read_MPU(Z_AXIS, ACCEL);

		if(setIMUThres)	{
			//Initialize the Accel threshold values depending on first value read
			user_setMpuThreshold(reset_myAccel);
			setIMUThres = 0;
		}
		else {
			accel_range_mask = user_mpuMovementState(reset_myAccel);

			//if there is motion
			if (accel_range_mask)
				emg_set_stats.movedOrNah[repCount] = 1;
			else //else
				emg_set_stats.movedOrNah[repCount] = 0;

//			if(accel_range_mask != 0)
//			{
//				//start vibration motors
//				if(accel_range_mask & X_AXIS_STATE_MASK)
//				{
//#if defined(USE_UART)
//					Log_info0("ACCEL X out of bound ");
//#else
//					System_printf("ACCEL X out of bound\n");
//					System_flush();
//#endif
//				}
//
//				if(accel_range_mask & Y_AXIS_STATE_MASK)
//				{
//#if defined(USE_UART)
//					Log_info0("ACCEL Y out of bound ");
//#else
//					System_printf("ACCEL Y out of bound\n");
//					System_flush();
//#endif
//				}
//
//				if(accel_range_mask & Z_AXIS_STATE_MASK)
//				{
//#if defined(USE_UART)
//					Log_info0("ACCEL Z out of bound ");
//#else
//					System_printf("ACCEL Z out of bound\n");
//					System_flush();
//#endif
//
//				}
//			}
		}
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

