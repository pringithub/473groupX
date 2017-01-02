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
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <ti/sysbios/knl/Clock.h>

//SYS/BIOS Header Files
#include <ti/sysbios/knl/Semaphore.h>

//**********************************************************************************
// Required Definitions
//**********************************************************************************
//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#define USE_UART 							1

//EMG
#define EMG_NUMBER_OF_SAMPLES_SLICE			50
#define EMG_MAX_REPS						19 //lol

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
	uint16_t pulseWidth[EMG_MAX_REPS];
	uint16_t deadWidth[EMG_MAX_REPS];
	//time from start of rep to peak
	uint16_t concentricTime[EMG_MAX_REPS];
	//time from peak of rep to end
	uint16_t eccentricTime[EMG_MAX_REPS];
	uint16_t peakIntensity[EMG_MAX_REPS];
	uint8_t movedOrNah[EMG_MAX_REPS];			//1 - moved; 0 - nah
	uint8_t numReps;
	uint8_t setDone;
} EMG_stats;


typedef struct {
	uint8_t targetSetCount;
	uint8_t targetRepCount;
	uint16_t maxRestSeconds;
	uint8_t hapticFeedback;
	uint8_t imuFeedback;
} Workout_config;

//Bluetooth stuff
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
extern Workout_config myWorkoutConfig;

// Clocks
extern Clock_Struct emgClock;
extern Clock_Struct accelClock;

//Data
extern uint8_t repCount;
extern uint8_t setIMUThres;
extern uint8_t stopEmgRequest;
extern uint8_t emgRunning;
extern uint8_t setCount;
//**********************************************************************************
// General Functions
//**********************************************************************************
//Vibe Motor
extern void buzz(uint8_t numTimes);

//Bluetooth stuff
extern user_app_error_type_t user_sendEmgPacket(uint8_t* pData, uint8_t len, app_pkt_type_t packetType);
extern user_app_error_type_t user_sendAccelPacket(uint8_t* pData, uint8_t len, app_pkt_type_t packetType);


//FOR TESTING: DELETE LATERS
extern void printWorkoutConfig(void);

#endif /* FLEXZONE_GLOBALS_H */
