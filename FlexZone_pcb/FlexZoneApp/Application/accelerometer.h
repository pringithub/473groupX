/*
* Application Name:		FlexZone (Application)
* File Name: 			accelerometer.h
* Group: 				GroupX - FlexZone
* Description:			Defines and prototypes for the Accelerometer Thread.
 */
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

//**********************************************************************************
// Header Files
//**********************************************************************************
#include "FlexZoneGlobals.h"

//**********************************************************************************
// Required Definitions
//**********************************************************************************

//**********************************************************************************
// Global Data Structures
//**********************************************************************************

//**********************************************************************************
// Function Prototypes
//**********************************************************************************
/**
 * Creates Accelerometer task running at and configures all required hardware components.
 *
 * @param 	none
 * @return 	none
 */
extern void accel_createTask(void);

#endif /* ACCELEROMETER_H */
