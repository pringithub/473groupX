/*
* Application Name:		FlexZone (Application)
* File Name: 			vibe.h
* Group: 				GroupX - FlexZone
* Description:			Defines and prototypes for the vibration motor Thread.
 */
#ifndef VIBE_H
#define VIBE_H

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
 * Creates Vibration motor task running at and configures all required hardware components.
 *
 * @param 	none
 * @return 	none
 */
extern void vibe_createTask(void);

#endif /* VIBE_H */
