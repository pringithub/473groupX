/*
 * Application Name:	FlexZone (Application)
 * File Name: 			MPU9250.c
 * Group: 				GroupX - FlexZone
 * Description:			Implementation file for the low level IMU (MPU9250) driver.
 */

//**********************************************************************************
// Header Files
//**********************************************************************************
//XDCtools Header Files
#include <xdc/std.h>				//can probably remove these
#include <xdc/runtime/System.h>		//can probably remove these
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

//TI-RTOS Header Files
#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>
#include <ti/drivers/dma/UDMACC26XX.h>

//Board Specific Header Files
#include "Board.h"

//Home brewed Header Files
#include "MPU9250.h"

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
SPI_CS_PIN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
PIN_TERMINATE };

//Register Address Matrix
uint8_t axes[2][3] = {
		{ACCEL_XOUT_H, ACCEL_YOUT_H, ACCEL_ZOUT_H},
		{GYRO_XOUT_H, GYRO_YOUT_H, GYRO_ZOUT_H}
};
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
 *	Reads MPU2650 and returns 16-bit reading of specified axis/function.
 *
 * @param 	axis		Which axis to read. Available values (X_AXIS, Y_AXIS, Z_AXIS)
 * @param	fsel		Function select. Available values (GYRO, ACCEL)
 * @return	uint16_t value of specified axis/function.
 */
uint16_t read_MPU(uint8_t axis, uint8_t fsel)
{
	uint16_t result;

	uint16_t temp = read_reg(axes[fsel][axis]);
	uint16_t tempNext = read_reg(axes[fsel][axis] + 1);
	result = (temp << 8);
	result |= tempNext;
	return result;
}

/**
 * Performs a register read and return 8-bit value of register.
 *
 * @param 	regAddr		(something)
 * @return uint8_t value returned on MISO line after transaction.
 */
uint8_t read_reg(uint8_t regAddr)
{
	return write_reg(READ_FLAG | regAddr, 0x00);
}

/**
 * Performs internal CS and writes data to the specified register. Returns value on MISO line after transaction.
 *
 * @param 	regAddr		(something)
 * @param	data		(something)
 * @return 	(something)
 */
uint8_t write_reg(uint8_t regAddr, uint8_t data)
{
	_accel_cs_low();
	//Transaction consists of two writes: Register address then data
	spiXmitByte(regAddr);
	uint8_t temp = spiXmitByte(data);
	_accel_cs_high();
	return temp;
}

//**********************************************************************************
// Low Level Functions
//**********************************************************************************
/**
 * Initialize MPU2590 SPI module
 *
 * @param 	none
 * @return	none
 */
void mpu_spi_init()
{
	// Initialize board level SPI
	Board_initSPI();

	// Open SPI CS pin
	spiCsPinHandle = PIN_open(&spiCsPinState, spiCsPinTable);
	if (!spiCsPinHandle) {
		Log_error0("Error initializing SPI CS pin");
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
	spiHandle = SPI_open(Board_SPI0, &spiParams);
	if (!spiHandle) {
		Log_error0("SPI did not open");
	}
}

/**
 * Sets accelerometer's CS low.
 *
 * @param 	none
 * @return	none
 */
void _accel_cs_low()
{
	PIN_setOutputValue(spiCsPinHandle, SPI_CS_PIN, 0);
}

/**
 * Sets accelerometer's CS high.
 *
 * @param 	none
 * @return	none
 */
void _accel_cs_high()
{
	PIN_setOutputValue(spiCsPinHandle, SPI_CS_PIN, 1);
}

/**
 * Sends data to accelerometer. Returns what is on accelerometer's MISO bus afterwards.
 * Operates independently of CS. Pre/post conditions are _accel_cs_<low/high>() call.
 *
 * @param 	data	(something)
 * @return	uint8_t data from accelerometer's MISO bus.
 */
uint8_t spiXmitByte(uint8_t data)
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
		Log_error0("Unsuccessful SPI transfer");
	}

	// Wait on successful transaction
    while(SPI_TRANSFER_COMPLETED != spiTransaction.status) {};

	// Return results in receive buffer
	return rxBuf[0];
}

