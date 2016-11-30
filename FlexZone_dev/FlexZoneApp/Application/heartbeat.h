/*
* Application Name:		FlexZone (Application)
* File Name: 			heartbeat.h
* Group: 				GroupX - FlexZone
* Description:			Defines and prototypes for the Heart beat Thread. Provides drivers for onboard LEDs.
 */
#ifndef HEARTBEAT_H
#define HEARTBEAT_H

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
 * Creates Heart beat task and configures all required hardware components.
 *
 * @param 	none
 * @return 	none
 */
extern void heartbeat_createTask(void);

#endif /* HEARTBEAT_H */
