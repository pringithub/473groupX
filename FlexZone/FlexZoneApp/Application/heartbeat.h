/*
* Application Name:		FlexZone (Application)
* File Name: 			heartbeat.h
* Group: 				GroupX - FlexZone
* Description:			Defines and prototypes for the Heartbeat Thread. Runs at Priority 2.
 */
#ifndef HEARTBEAT_H
#define HEARTBEAT_H

//*****************************************************************************
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//===============================================
//================== Defines ====================
//===============================================


//===============================================
//============ Function Prototypes ==============
//===============================================
/**
 * Creates Heartbeat task running at Priority 2 and configures all required hardware components.
 *
 * @param 	none
 * @return 	none
 */
extern void heartbeat_createTask(void);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
