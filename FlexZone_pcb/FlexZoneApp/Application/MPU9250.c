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

//TI-RTOS Header Files
//#include <ti/drivers/PIN.h>
#include <ti/drivers/I2C.h>

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
//I2C Transaction Buffer
uint8_t accelTxBuf[2];
uint8_t accelRxBuf[1];

//I2C Driver Handle
I2C_Handle accel_i2c_handle;

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
 * @param 	regAddr		RA to read
 * @return uint8_t register value
 */
uint8_t read_reg(uint8_t regAddr)
{
	return i2cRead(regAddr);
}

/**
 * Writes data to the specified register
 *
 * @param 	regAddr		RA to write
 * @param	data		1-byte data
 * @return 	none
 */
void write_reg(uint8_t regAddr, uint8_t data)
{
	i2cWrite(regAddr, data);
}

//**********************************************************************************
// Low Level Functions
//**********************************************************************************
/**
 * Initialize MPU2950 on I2C0 module
 *
 * @param 	none
 * @return	none
 */
void mpu_i2c_init()
{
	//Initialize I2C aPI
	I2C_init();

	//Initialize I2C parameters
	I2C_Params i2cParams;
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_400kHz;
	i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2cParams.transferCallbackFxn = NULL;

    //Open I2C handle
	accel_i2c_handle = I2C_open(Board_I2C, &i2cParams);
    if (!accel_i2c_handle) {
		System_printf("Accelerometer I2C did not open\n");
		System_flush();
    }
}

/**
 * Reads value at specified address.
 *
 * @param 	regAddr		1-byte register address (RA)
 * @return	uint8_t data from accelerometer's SDA line.
 */
uint8_t i2cRead(uint8_t regAddr)
{
	I2C_Transaction i2cTransaction;

	// Place data to be sent in tx buffer
	accelTxBuf[0] = regAddr;

	//Configure the transaction object
	i2cTransaction.slaveAddress = ACCEL_I2C_SLAVE_ADDR | READ_FLAG;
	i2cTransaction.writeBuf = accelTxBuf;
	i2cTransaction.writeCount = 1;
	i2cTransaction.readBuf = accelRxBuf;
	i2cTransaction.readCount = 1;

	//Perform transaction
	bool ret = I2C_transfer(accel_i2c_handle, &i2cTransaction);
	if (!ret) {
		System_printf("Unsuccessful accelerometer I2C transfer\n");
		System_flush();
	}

	// Return results in receive buffer
	return accelRxBuf[0];
}


/**
 * Writes 1-byte value to specified address.
 *
 * @param 	regAddr		1-byte register address (RA)
 * @return	uint8_t data from accelerometer's SDA line.
 */
void i2cWrite(uint8_t regAddr, uint8_t data)
{
	// TODO
}
