/*
* Application Name:		FlexZone (Application)
* File Name: 			FlexZoneGlobals.h
* Group: 				GroupX - FlexZone
* Description:			Global data structures for inter-thread access.
*/

#ifndef FLEXZONE_GLOBALS_H
#define FLEXZONE_GLOBALS_H

//**********************************************************************************
// Header Files
//**********************************************************************************
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

//SYS/BIOS Header Files
#include <ti/sysbios/knl/Semaphore.h>

//TI-RTOS Header Files
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>

//**********************************************************************************
// Required Definitions
//**********************************************************************************


//**********************************************************************************
// Data Structures
//**********************************************************************************
typedef struct {
	uint16_t ACCEL_X;
	uint16_t ACCEL_Y;
	uint16_t ACCEL_Z;
	uint16_t GYRO_X;
	uint16_t GYRO_Y;
	uint16_t GYRO_Z;
} Accel_State;


//**********************************************************************************
// Globally Scoped Variables (for RTOS: Semaphores, Mailboxes, Queues, Data Structures)
//**********************************************************************************


//**********************************************************************************
// General Functions
//**********************************************************************************

#endif /* FLEXZONE_GLOBALS_H */
