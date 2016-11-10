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
#include <ti/sysbios/knl/Semaphore.h>

//**********************************************************************************
// Required Definitions
//**********************************************************************************
//EMG
#define EMG_NUMBER_OF_SAMPLES_SLICE			50

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

typedef struct {
	uint32_t pulseWidth[20];
	uint32_t deadWidth[20];
	//time from start of rep to peak
	uint32_t concentricTime[20];
	//time from peak of rep to end
	uint32_t eccentricTime[20];
	uint8_t numReps;
} EMG_stats;
//**********************************************************************************
// Globally Scoped Variables (for RTOS: Semaphores, Mailboxes, Queues, Data Structures)
//**********************************************************************************
//FlexZone - BLE Simple Peripheral Thread


//Accelerometer Thread
extern Semaphore_Struct accelSemaphore;

//EMG Thread
extern Semaphore_Struct emgSemaphore;
extern uint32_t rawAdc[EMG_NUMBER_OF_SAMPLES_SLICE];
extern uint32_t adjustedAdc, uvAdc;


#endif /* FLEXZONE_GLOBALS_H */
