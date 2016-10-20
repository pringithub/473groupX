// include any needed headers here



//any processor API/MMIO addressing that has to happen
//in order to use our processor's SPI 
void spiInit();
//sets accel's CS low
void _accel_cs_low();
//sets accel's CS high 
void _accel_cs_high(); 
//transmit data
//returns whatever the MISO line gave the processor
uint8_t spiXmitByte(uint8_t data);


