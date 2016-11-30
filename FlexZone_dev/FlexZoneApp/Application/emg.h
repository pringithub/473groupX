/*
* Application Name:		FlexZone (Application)
* File Name: 			emg.h
* Group: 				GroupX - FlexZone
* Description:			Defines and prototypes for the EMG Thread.
 */
#ifndef EMG_H
#define EMG_H

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
 * Creates EMG task running at Priority 1 and configures all required hardware components.
 *
 * @param 	none
 * @return 	none
 */
extern void emg_createTask(void);

#endif /* EMG_H */
