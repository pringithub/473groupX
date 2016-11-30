/*
 * Application Name:		FlexZone (Application)
 * File Name: 				MPU2950.h
 * Group: 					GroupX - FlexZone
 * Description:				Defines and prototypes for the low level IMU (MPU9250) driver.
 */
#ifndef MPU2950_H
#define MPU2950_H

//**********************************************************************************
// Header Files
//**********************************************************************************
//Standard Header Files
#include <stdint.h>
#include "FlexZoneGlobals.h"

//**********************************************************************************
// Required Definitions
//**********************************************************************************
#define ACCEL_I2C_SLAVE_ADDR		0x68

//Accelerometer Registers
#define ACCEL_XOUT_H 0x3b
#define ACCEL_XOUT_L 0x3c
#define ACCEL_YOUT_H 0x3d
#define ACCEL_YOUT_L 0x3e
#define ACCEL_ZOUT_H 0x3f
#define ACCEL_ZOUT_L 0x40

//Gyroscope Registers
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47

//R/W masks
#define READ_FLAG 0x80

//Parameters
#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
#define GYRO 1
#define ACCEL 0

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
 *	Reads MPU2650 and returns 16-bit reading of specified axis/function.
 *
 * @param 	axis		Which axis to read. Available values (X_AXIS, Y_AXIS, Z_AXIS)
 * @param	fsel		Function select. Available values (GYRO, ACCEL)
 * @return	uint16_t value of specified axis/function.
 */
uint16_t read_MPU(uint8_t axis, uint8_t fsel);

/**
 * Performs a register read and return 8-bit value of register.
 *
 * @param 	regAddr		RA to read
 * @return uint8_t register value
 */
uint8_t read_reg(uint8_t regAddr);

/**
 * Writes data to the specified register
 *
 * @param 	regAddr		RA to write
 * @param	data		1-byte data
 * @return 	none
 */
void write_reg(uint8_t regAddr, uint8_t data);


//**********************************************************************************
// Low Level Functions
//**********************************************************************************
/**
 * Initialize MPU2950 on I2C0 module
 *
 * @param 	none
 * @return	none
 */
void mpu_i2c_init();

/**
 * Reads value at specified address.
 *
 * @param 	regAddr		1-byte register address (RA)
 * @return	uint8_t data from accelerometer's SDA line.
 */
uint8_t i2cRead(uint8_t regAddr);

/**
 * Writes 1-byte value to specified address.
 *
 * @param 	regAddr		1-byte register address (RA)
 * @return	uint8_t data from accelerometer's SDA line.
 */
void i2cWrite(uint8_t regAddr, uint8_t data);

#endif /* MPU2950_H */