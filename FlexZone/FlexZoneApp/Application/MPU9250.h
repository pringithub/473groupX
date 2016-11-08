/*
 * Application Name:		FlexZone (Application)
 * File Name: 				MPU2950.h
 * Group: 					GroupX - FlexZone
 * Description:				Defines and prototypes for the low level IMU (MPU9250) driver.
 */

//**********************************************************************************
// Header Files
//**********************************************************************************
//Standard Header Files
#include <stdint.h>

//**********************************************************************************
// Required Definitions
//**********************************************************************************
#define SPI_CS_PIN			Board_LCD_CS

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
 * @param 	regAddr		(something)
 * @return uint8_t value returned on MISO line after transaction.
 */
uint8_t read_reg(uint8_t regAddr);

/**
 * Performs internal CS and writes data to the specified register. Returns value on MISO line after transaction.
 *
 * @param 	regAddr		(something)
 * @param	data		(something)
 * @return 	(something)
 */
uint8_t write_reg(uint8_t regAddr, uint8_t data);


//**********************************************************************************
// Low Level Functions
//**********************************************************************************
/**
 * Initialize MPU2950 on SPI0 module
 *
 * @param 	none
 * @return	none
 */
void mpu_spi_init();

/**
 * Sets accelerometer's CS low.
 *
 * @param 	none
 * @return	none
 */
void _accel_cs_low();

/**
 * Sets accelerometer's CS high.
 *
 * @param 	none
 * @return	none
 */
void _accel_cs_high();

/**
 * Sends data to accelerometer. Returns what is on accelerometer's MISO bus afterwards.
 *
 * @param 	data	(something)
 * @return	uint8_t data from accelerometer's MISO bus.
 */
uint8_t spiXmitByte(uint8_t data);
