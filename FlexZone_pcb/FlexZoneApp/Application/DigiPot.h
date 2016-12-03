/*
 * Application Name:		FlexZone (Application)
 * File Name: 				DigiPot.h
 * Group: 					GroupX - FlexZone
 * Description:				Defines and prototypes for the low level digipot driver.
 */
#ifndef DIGIPOT_H
#define DIGIPOT_H

//**********************************************************************************
// Header Files
//**********************************************************************************
//Standard Header Files
#include <stdint.h>
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

//**********************************************************************************
// High Level Functions
//**********************************************************************************
/**
 *	Sets wiper value on digipot.
 *
 * @param 	value			8-bit value of digipot.
 * @param	digipotNum		Index of digipot to write to.
 * @return
 */
uint8_t set_Wiper(uint8_t value, uint8_t digiPotNum);


uint8_t read_ISL(uint8_t addr, uint8_t digiPotNum);

/**
 * Performs internal CS and writes data to the specified register. Returns value on MISO line after transaction.
 *
 * @param 	regAddr			Address to write too.
 * @param	data			1-byte data value to write
 * @param	digiPotNum		Index of digipot to write to.
 * @return 	uint8_t value from digipot's MISO line.
 */
uint8_t digiPot_write_reg(uint8_t regAddr, uint8_t data, uint8_t digiPotNum);


//**********************************************************************************
// Low Level Functions
//**********************************************************************************
/**
 * Initialize Digipots on SPI1 module
 *
 * @param 	none
 * @return	none
 */
void digiPot_spi_init(void);

/**
 * Sets Digipot CS low.
 *
 * @param	digiPotNum		Index of digipot to write to.
 * @return	none
 */
void digiPot_cs_low(uint8_t digiPotNum);

/**
 * Sets Digipot CS high.
 *
 * @param	digiPotNum		Index of digipot to write to.
 * @return	none
 */
void digiPot_cs_high(uint8_t digiPotNum);

/**
 * Sends data to digipot. Returns what is on digipot's MISO bus afterwards.
 * Operates independently of CS. Pre/post conditions are digiPot_cs_<low/high>() call.
 *
 * @param 	data	(something)
 * @return	uint8_t data from accelerometer's MISO bus.
 */
uint8_t digiPot_spiXmitByte(uint8_t data);

#endif /* DIGIPOT_H */
