#include "accel_high.h"



uint16_t readMPU(uint8_t axis, uint8_t fsel){
uint16_t result;
result = (readReg(axis[fsel][axis]) << 8);
result |= readReg(axis[fsel][axis]+1);
return result;
}

uint8_t writeReg(uint8_t regAddr, uint8_t data){
_accel_cs_low();
spiXmitByte(regAddr);
uint8_t temp = spiXmitByte(data);
_accel_cs_high();
//maybe want a delay here
return temp;
}

uint8_t readReg(uint8_t regAddr){
return writeReg(READ_FLAG| regAddr, 0x00);
}



