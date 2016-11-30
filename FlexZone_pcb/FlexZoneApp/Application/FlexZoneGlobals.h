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

//**********************************************************************************
// Required Definitions
//**********************************************************************************
//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file

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
	uint8_t setDone;
} EMG_stats;

typedef enum
{
	USER_APP_ERROR_OK = 0,			/* No error at user application  */
	USER_APP_ERROR_INVALID_LEN,		/* Input parameters have invalid length  */
	USER_APP_ERROR_INVALID_PARAM,	/* Input parameters are not correct  */
	USER_APP_ERROR_UNKNOWN,			/* Unknown error  */
} user_app_error_type_t;

typedef enum
{
	APP_PACKET_TYPE_DATA = 0,		/* Packet contains data  */
	APP_PACKET_TYPE_CONFIG = 1,		/* Packet contains configuration  */
} app_pkt_type_t;
//**********************************************************************************
// Globally Scoped Variables (for RTOS: Semaphores, Mailboxes, Queues, Data Structures)
//**********************************************************************************
//FlexZone - BLE Simple Peripheral Thread


//Accelerometer Thread
extern Semaphore_Struct accelSemaphore;

//EMG Thread
extern Semaphore_Struct emgSemaphore;
extern uint32_t rawAdc[EMG_NUMBER_OF_SAMPLES_SLICE];
extern EMG_stats emg_set_stats;
extern EMG_stats emgSets[10];

//**********************************************************************************
// General Functions
//**********************************************************************************
extern user_app_error_type_t user_sendEmgPacket(uint8_t* pData, uint8_t len, app_pkt_type_t packetType);
extern user_app_error_type_t user_sendAccelPacket(uint8_t* pData, uint8_t len, app_pkt_type_t packetType);

#endif /* FLEXZONE_GLOBALS_H */
