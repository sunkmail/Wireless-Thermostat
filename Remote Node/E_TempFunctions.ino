void getRawTemp(void) { // Get Raw Temperature Data
  oneWire.reset();
  oneWire.write(0xCC);  // Bypass ROM command - since only 1 sensor used
  oneWire.write(0xBE);  // Request Read Scratchpad
  rawTemp[0] = oneWire.read();
  rawTemp[1] = oneWire.read();
  oneWire.reset();        // Send Reset to stop sending or rest of scratchpad
}

void requestTemp(void) {
  oneWire.reset();
  oneWire.write(0xCC);  // Bypass ROM command - since only 1 sensor used
  oneWire.write(0x44);  // Request Temp Conversion
}

void ConvertRawTomDegreeC(void) {     // ConvertRawTomDegreeC

  int tempwhole;          // temperature in whole numbers
  int tempdecimal;        // temperature decimal portion
  
  mDegreeC = ((int(rawTemp[1]) << 8) | rawTemp[0]);     // combine 2 byte value to single int

  if (rawTemp[1] >= 0x80)       // if sign bit set, temp is negative
  {
    tempPos = false;
    mDegreeC = -mDegreeC;       // Flip to positive value - removes 2's compliment
  }
  else
    tempPos = true;

  tempwhole = mDegreeC >> 4;  // get temp value of whole degrees

  tempdecimal = mDegreeC & 0x000F;    // Clear all bits above zero
  tempdecimal = tempdecimal * 625;     // value in 1/10000 of degrees ... 5000 = 0.5C

  mDegreeC = ((tempwhole * 1000) + (tempdecimal / 10)); // drop least sig. digit to make into 1/1000 of a degree ... 500 = 0.5C

// Below - not really needed, going to only have 1 decimal place in the end anyway.
//  if (tempdecimal % 10 >= 5)                            // if the dropped digit 5 or more
//    mDegreeC++;                                         // Round up

}
