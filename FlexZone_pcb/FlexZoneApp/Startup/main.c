/*
 * Application Name:	FlexZone (Application)
 * File Name: 			main.c
 * Group: 				GroupX - FlexZone
 * Description:			Main function for FlexZone application. Thread initialization & SYS/BIOS kickoff.
 * Required Stacks:		BLE-Stack 	v2.01.00
 * 						TI-RTOS 	v2.13.00
 * 						CC26XXware	v2.21.01
 * 						SYSBIOS		v6.42.00
 */

//**********************************************************************************
// Header Files
//**********************************************************************************
//XDCtools Header Files
#include <xdc/runtime/Error.h>

//SYS/BIOS Header Files
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/BIOS.h>
#include <uart_logs.h>
#include <ti/drivers/UART.h>

//BLE Header Files
#include "ICall.h"
#include "bcomdef.h"
#include "peripheral.h"
#ifndef USE_DEFAULT_USER_CFG
#include "bleUserConfig.h"
// BLE user defined configuration
bleUserCfg_t user0Cfg = BLE_USER_CFG;
#endif // USE_DEFAULT_USER_CFG

//Home brewed Drivers
#include "FlexZoneGlobals.h"
#include "FlexZone.h"
#include "emg.h"
#include "accelerometer.h"
#include "vibe.h"

//**********************************************************************************
// Required Definitions
//**********************************************************************************

//**********************************************************************************
// Global Data Structures
//**********************************************************************************


//**********************************************************************************
// Function Definitions
//**********************************************************************************

/**
 * Exception Handler
 *
 * @param 	none
 * @return 	none
 */
void exceptionHandler() {
	volatile uint8_t i = 1;
	while (i) {
	}
}

/**
 * Error handled to be hooked into TI-RTOS
 *
 * @param 	none
 * @return 	none
 */
Void smallErrorHook(Error_Block *eb) {
	for (;;)
		;
}

/**
 * HAL assert handler required by OSAL memory module.
 *
 * @param 	none
 * @return 	none
 */
void halAssertHandler(void) {
	for (;;)
		;
}


//**********************************************************************************
// Main
//**********************************************************************************
int main() {
	PIN_init(BoardGpioInitTable);

#ifndef POWER_SAVING
	/* Set constraints for Standby, powerdown and idle mode */
	Power_setConstraint(Power_SB_DISALLOW);
	Power_setConstraint(Power_IDLE_PD_DISALLOW);
#endif // POWER_SAVING

#if defined(USE_UART)
	//Initialize the RTOS Log formatting and output to UART in Idle thread.
	//Note: Define xdc_runtime_Log_DISABLE_ALL to remove all impact of Log.
	//Note: NULL as Params gives 115200,8,N,1 and Blocking mode
	UartLog_init(UART_open(Board_UART, NULL));
#endif // USE_UART

	//Initialize ICall module
	ICall_init();

	//Start tasks of external images - Priority 5
	ICall_createRemoteTasks();

	//Kick off profile - Priority 3
	GAPRole_createTask();

	//**********************************************************************************
	// Thread Initialization
	//**********************************************************************************
	//BlE task - Priority 4
	FlexZone_createTask();
	//EMG task - Priority 1
	emg_createTask();
	// MPU2650 Accelerometer (SPI) - Priority 2
	accel_createTask();

	//BLE Services - Priority 3
//	accelConfig_createSwi();
//	accelConfig_createTask();
	emgConfig_createSwi();
	emgConfig_createTask();

	//Vibration Motor Task - Priority 1
	vibe_createTask();
	//**********************************************************************************
	// Enable Interrupts & start SYS/BIOS
	//**********************************************************************************
	BIOS_start();

	return 0;
}

void printWorkoutConfig(void) {
#if defined(USE_UART)
		Log_info5("Sets: %u Reps: %u Rest: %u Haptic: %u IMU: %u",
				myWorkoutConfig.targetSetCount,myWorkoutConfig.targetRepCount,myWorkoutConfig.maxRestSeconds,
				myWorkoutConfig.hapticFeedback,myWorkoutConfig.imuFeedback);
#else
		System_printf("Sets: %u Reps: %u Rest: %u Haptic: %u IMU: %u\n",
				myWorkoutConfig.targetSetCount,myWorkoutConfig.targetRepCount,myWorkoutConfig.maxRestSeconds,
				myWorkoutConfig.hapticFeedback,myWorkoutConfig.imuFeedback);
		System_flush();
#endif //USE_UART
}
