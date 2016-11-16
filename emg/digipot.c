void setWiperValue(uint8_t value, char digipotNum){
    set_cs_low(); // comment out if not needed 
    spiXmitByte(0xC0); // we're talking to our reg
    delay(0);
    spiXmitByte(value);
    set_cs_high();
}
