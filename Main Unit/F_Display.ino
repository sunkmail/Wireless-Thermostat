/*
  Display(s) being used:
        0.91" OLED Display module  128x32 - Just for testing until other module arrives
        0.96" OLED display module  128x64


  http://4umi.com/web/javascript/xbm.php   - For making custom characters
*/

// ------------------------------------------------------------------
// updateDisplay     updateDisplay     updateDisplay     updateDisplay
// ------------------------------------------------------------------

// Displayed info for when active (NOT in Settings mode)
void updateDisplay() {

  const byte dataY[] = {7, 15, 23, 32};  // Y value for screen lines
  const byte dataX = 40;


  const char rmName[][9] = {       // Setup an array of char arrays to use for display function loop
    "Living:",                 // F() Vs PROGMEM??
    "Master:",
    "Den:",
    "Outside:"
  };

  OLED.firstPage();
  do {
    // all graphics commands have to appear within the loop body.
    OLED.setFont(u8g2_font_5x7_mr);         // Set a font for 6 Pixel high

    for (byte i = 0; i <= (RFNodes - 1); i++) {
      OLED.setCursor(0, dataY[i]);           // Set cursor to prepare for write
      OLED.print((char*)rmName[i]);           // Print room name for Node, char array
      OLED.setCursor(dataX, dataY[i]);       // Set cursor to prepare for write
            // If 'invalid' data, display blank/error indicator
      if ((tempData[i][arrayTempPos] == false) & (tempData[i][arrayWholeDegrees] == 99) & (tempData[i][arrayDecimalDegrees] == 9))
        OLED.print(" ---");
      else              // If valid data
        printTemp(i);                           // Display Temp at cursor location
    }


  } while ( OLED.nextPage() );
}




// ------------------------------------------------------------------
// printTemp  printTemp  printTemp  printTemp   printTemp   printTemp
// ------------------------------------------------------------------

void printTemp(byte NodeID) {   // Write Temp out with: +/-, whole, decimalpoint, decimal, C

  if (tempData[NodeID][arrayWholeDegrees] < 10)
    OLED.print(" ");                // If number doesn't have 2-digit 'WholeDegrees', add a Blankspace

  if (tempData[NodeID][arrayTempPos] == true)
    OLED.print(" ");                // If number is positive, just leave a Blank
  else
    OLED.print("-");                // If number is negative, place a minus sign

  OLED.print(tempData[NodeID][arrayWholeDegrees]);         // Print the whole degree portion
  OLED.print(".");                                         // Print decimal character
  OLED.print(tempData[NodeID][arrayDecimalDegrees]);       // Print the decimal degrees portion
  OLED.print(" C");                                        // Print DegreeC indicator character

}
