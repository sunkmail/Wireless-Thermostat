
// ------------------------------------------------------------------
// updateDisplay     updateDisplay     updateDisplay     updateDisplay
// ------------------------------------------------------------------

void updateDisplay() {

  //  OzOled.printNumber(long(mDegreeC),5, 2);
  byte x = 4;             // X position for display start
  const byte y = 2;       // Y position for display

  byte dataTemp = 0;    // for tracking how many characters have printed, to update X location


  if (tempPos == true)
    OzOled.printChar(' ', x, y);    // If number is positive, just leave a Blank
  else
    OzOled.printChar('-', x, y);    // If number is negative, place a minus sign
  x++;                              // Increment for next character(s)

  OzOled.printNumber(long(mDegreeC / 1000), x, y);  // Print the whole degree portion
  x++;                              // Increment for next character(s)
  if (mDegreeC / 1000 >= 10)        // if 2 digit whole degrees temp, add extra space for next print
    x++;                            // Increment for next character(s)

  OzOled.printChar('.', x, y);      // Print decimal character
  x++;                              // Increment for next character(s)


  dataTemp = (mDegreeC % 1000);       // Parse out decimal portion
  if (dataTemp % 100 >= 50)           // if the 2nd digit is 5 or higher,
    dataTemp = (dataTemp / 100) + 1;  // round up the first decimal digit
  else
    dataTemp = (dataTemp / 100);      // otherwise just get first decimal

  OzOled.printNumber(long(dataTemp), x, y);  // Print the whole degree portion
  x++;                              // Increment for next character(s)

  OzOled.printChar(' ', x, y);      // Print 'space' character - overwrite any old data
  x++;                              // Increment for next character(s)

  OzOled.printChar('C', x, y);      // Print DegreeC indicator character
  x++;                              // Increment for next character(s)

  for (byte i = 1; i >= 3; i++) {   // Print multiple 'space' characters to overwrite any old data
    OzOled.printChar(' ', x, y);    
    x++;                            
  }

  //char tmp[6];      // Array to hold Characters, assuming -XX.Y to be largest length + 1 for Null
  //char tmp[15] = {'-','1','2','.','3','4'};
  //tempC = -12.34;    // set float value, for testing various combos - # of digits, negatives, etc
  //dtostrf(tempC,5,1,tmp);   // Convert tempC to Array of Chars - 5 char total, 1 after decimal

  //  char tmp[8] = {'1','2','3','4','5','6'};

  /*  Can't get to compile - should be right -> undefined reference ??

  */
  //  OzOled.printBigNumber(tmp, 0, 0, 5);  // Print Large Font up to 5 Chars
  //  OzOled.printBigNumber(tmp);
}


// ------------------------------------------------------------------
// displayVcom    displayVcom   displayVcom   displayVcom
// ------------------------------------------------------------------

void displayVcom(byte value) {      //  0,2, or 3 - Higher Values are Brighter

  OzOled.sendCommand(SET_DisplayVcomDeselect);
  OzOled.sendCommand(value << 4);             // sends brightness value byte
}

// ------------------------------------------------------------------
// displayPreCharge    displayPreCharge   displayPreCharge    displayPreCharge
// ------------------------------------------------------------------

void displayPreCharge(byte value1, byte value2) {      //  0 - 15 in each
  // value1 - higher value = Darker -> very little diff. above 3
  // value2 - higher value = brighter
  OzOled.sendCommand(SET_DisplayPreCharge);
  OzOled.sendCommand((value2 << 4) | value1);
}
