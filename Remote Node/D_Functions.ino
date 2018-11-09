
// ------------------------------------------------------------------
// updateDisplay     updateDisplay     updateDisplay     updateDisplay
// ------------------------------------------------------------------

void updateDisplay() {
  OzOled.printNumber(tempC,1, 5, 1);
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
