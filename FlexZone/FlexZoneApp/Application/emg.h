/*
* Application Name:		FlexZone (Application)
* File Name: 			emg.h
* Group: 				GroupX - FlexZone
* Description:			Defines and prototypes for the EMG Thread. Runs at Priority 1.
 */
#ifndef EMG_H
#define EMG_H

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
 * Creates EMG task running at Priority 1 and configures all required hardware components.
 *
 * @param 	none
 * @return 	none
 */
extern void emg_createTask(void);

//*****************************************************************************
// Mark the end of the C bindings section for C++ compilers.
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
