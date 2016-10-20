#include "accel_hard.h"

#define ACCEL_XOUT_H 0x3b
#define ACCEL_XOUT_L 0x3c
#define ACCEL_YOUT_H 0x3d
#define ACCEL_YOUT_L 0x3e
#define ACCEL_ZOUT_H 0x3f
#define ACCEL_ZOUT_L 0x40
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define READ_FLAG 0x80


#define X 0
#define Y 1
#define Z 2
#define GYRO 1
#define ACCEL 0
static uint8_t axis[][] = {{ACCEL_XOUT_H, ACCEL_YOUT_H, ACCEL_ZOUT_H},{GYRO_XOUT_H, GYRO_YOUT_H, GYRO_ZOUT_H}}

//axis can be X,Y,Z
//fsel can by GYRO or ACCEL
uint16_t readMPU(uint8_t axis, uint8_t fsel);

uint8_t readReg(uint8_t regAddr);

void writeReg(uint8_t regAddr, uint8_t data);
