void getRawTemp(void) { // Get Raw Temperature Data
  oneWire.reset();
  oneWire.write(0xCC);          // Bypass ROM command - since only 1 sensor used
  oneWire.write(0xBE);          // Request Read Scratchpad
  rawTemp[0] = oneWire.read();
  rawTemp[1] = oneWire.read();
  oneWire.reset();              // Send Reset to stop sending or rest of scratchpad
}

void requestTemp(void) {
  oneWire.reset();
  oneWire.write(0xCC);  // Bypass ROM command - since only 1 sensor used
  oneWire.write(0x44);  // Request Temp Conversion
}

void ConvertRawTemp(void) {     // ConvertRawTemp
  /*  GLOBALS
     byte wholeDegreeC = 0;    // Operating range well below range of byte
     byte decimalDegreeC = 0;  // Only send 1 Sig. Bit
  */
  int _working = 0;             // Container for maths

  _working = ((int(rawTemp[1]) << 8) | rawTemp[0]);     // combine 2 byte value to single int
  if (rawTemp[1] >= 0x80)                               // if sign bit set, temp is negative
  {
    tempPos = false;                                    // set sign byte to false (temp is Neg) 
    _working = -_working;                               // Flip to positive value - removes 2's compliment
  }
  else
    tempPos = true;

    wholeDegreeC = _working >> 4;           // get temperature value of whole degrees
  
    _working = _working & 0x000F;           // Clear all bits above zero
    _working = _working * 625;              // convert sensor reading into 1/10000 of degrees ... 5000 = 0.5C
  
    decimalDegreeC = (_working / 1000);     // drop least sig. digits to make into 1/10 of a degree ... 5 = 0.5C
    if (_working % 1000 >= 500)             // if the dropped digit 5 or more
      decimalDegreeC++;                     // Round up

}
