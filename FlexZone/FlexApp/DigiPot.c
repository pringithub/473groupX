/*
 * Application Name:	FlexZone (Application)
 * File Name: 			DigiPot.c
 * Group: 				GroupX - FlexZone
 * Description:			Implementation file for the low level digipot driver.
 */

//**********************************************************************************
// Header Files
//**********************************************************************************
//XDCtools Header Files
#include <xdc/runtime/Log.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

//TI-RTOS Header Files
#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>
#include <ti/drivers/dma/UDMACC26XX.h>

//Board Specific Header Files
#include "Board.h"

//Home brewed Header Files
#include <DigiPot.h>


//Standard Header Files
#include <string.h>
//#include <stdio.h>

//**********************************************************************************
// Required Definitions
//**********************************************************************************

//**********************************************************************************
// Global Data Structures
//**********************************************************************************
//SPI Transaction Buffers
uint8_t txBuf[1];     // Transmit buffer
uint8_t rxBuf[1];     // Receive buffer

//SPI Driver Handles
SPI_Handle spiHandle;

//Pin driver handles
static PIN_Handle spiCsPinHandle;

//Global memory storage for a PIN_Config table
static PIN_State spiCsPinState;

//Initial SPI CS pin configuration table
PIN_Config spiCsPinTable[] = {
		DIGIPOT_0_CS | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
		PIN_TERMINATE };

//**********************************************************************************
// Local Function Prototypes
//**********************************************************************************

//**********************************************************************************
// Function Definitions
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
void set_Wiper(uint8_t value, uint8_t digiPotNum)
{
	write_reg(0xC0, value, digiPotNum);
}

/**
 * Performs internal CS and writes data to the specified register. Returns value on MISO line after transaction.
 *
 * @param 	regAddr		(something)
 * @param	data		(something)
 * @param	digiPotNum		Index of digipot to write to.
 * @return 	(something)
 */
uint8_t write_reg(uint8_t regAddr, uint8_t data, uint8_t digiPotNum)
{
	digiPot_cs_low(digiPotNum);
	//Transaction consists of two writes: Register address then data
	digiPot_spiXmitByte(regAddr);
	uint8_t temp = digiPot_spiXmitByte(data);
	digiPot_cs_high(digiPotNum);
	return temp;
}

//**********************************************************************************
// Low Level Functions
//**********************************************************************************
/**
 * Initialize Digipots on SPI1 module
 *
 * @param 	none
 * @return	none
 */
void digiPot_spi_init()
{
	// Initialize board level SPI
	Board_initSPI();

	// Open SPI CS pin
	spiCsPinHandle = PIN_open(&spiCsPinState, spiCsPinTable);
	if (!spiCsPinHandle) {
		Log_error0("Error initializing digipot SPI1 CS pins");
	}

	// Initialize SPI parameters
	SPI_Params spiParams;
	SPI_Params_init(&spiParams);
	spiParams.bitRate     = 1000000;
	spiParams.frameFormat = SPI_POL0_PHA0;
	spiParams.mode        = SPI_MASTER;
	spiParams.transferMode = SPI_MODE_BLOCKING;
	//spiParams.transferMode = SPI_MODE_CALLBACK;
	spiParams.transferCallbackFxn = NULL;
	spiParams.transferTimeout = 2000;
	spiParams.dataSize = 8;

	// Open the SPI and perform the transfer
	spiHandle = SPI_open(Board_SPI1, &spiParams);
	if (!spiHandle) {
		Log_error0("SPI1 did not open");
	}
}

/**
 * Sets Digipot CS low.
 *
 * @param	digiPotNum		Index of digipot to write to.
 * @return	none
 */
void digiPot_cs_low(uint8_t digiPotNum)
{
	switch(digiPotNum) {
	case 0:
		PIN_setOutputValue(spiCsPinHandle, DIGIPOT_0_CS, 0);
		break;
	case 1:
		PIN_setOutputValue(spiCsPinHandle, DIGIPOT_1_CS, 0);
		break;
	default:
		PIN_setOutputValue(spiCsPinHandle, DIGIPOT_0_CS, 0);
	}
}

/**
 * Sets Digipot CS high.
 *
 * @param	digiPotNum		Index of digipot to write to.
 * @return	none
 */
void digiPot_cs_high(uint8_t digiPotNum)
{
	switch(digiPotNum) {
	case 0:
		PIN_setOutputValue(spiCsPinHandle, DIGIPOT_0_CS, 1);
		break;
	case 1:
		PIN_setOutputValue(spiCsPinHandle, DIGIPOT_1_CS, 1);
		break;
	default:
		PIN_setOutputValue(spiCsPinHandle, DIGIPOT_0_CS, 1);
	}
}

/**
 * Sends data to digipot. Returns what is on digipot's MISO bus afterwards.
 * Operates independently of CS. Pre/post conditions are digiPot_cs_<low/high>() call.
 *
 * @param 	data	(something)
 * @return	uint8_t data from accelerometer's MISO bus.
 */
uint8_t digiPot_spiXmitByte(uint8_t data)
{
	SPI_Transaction spiTransaction;

	// Configure the transaction object
	spiTransaction.arg = NULL;
	spiTransaction.count = sizeof(txBuf);
	spiTransaction.txBuf = txBuf;
	spiTransaction.rxBuf = rxBuf;

	// Place data to be sent in tx buffer
	txBuf[0] = data;

	//Perform transaction
	bool ret = SPI_transfer(spiHandle, &spiTransaction);
	if (!ret) {
	    System_printf("Unsuccessful SPI1 Transfer");
	    System_flush();
	}

	// Wait on successful transaction
    while(SPI_TRANSFER_COMPLETED != spiTransaction.status) {};

	// Return results in receive buffer
	return rxBuf[0];
}

