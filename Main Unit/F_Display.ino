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

  const byte dataX = 40;
  const byte dataY[] = {7, 15, 23, 32};  // Y value for screen lines
  const byte markerX = 78;      // X position for Contolling Node indicator - uses dataY

  const byte setX = 100;        // X posiion for setPoint Value
  const byte setY = 13;         // Y position for setPoint Value

  const byte controlX = 85;     // X posiion for Controlling Node Temp
  const byte controlY = 32;     // X posiion for Controlling Node Temp


  //  const char rmName[][9] = {       // Setup an array of char arrays to use for display function loop
  //    "Living:",                 // F() Vs PROGMEM??
  //    "Master:",
  //    "Den:",
  //    "Outside:"            // Moved to Global const area - so can be used in debugPrint statements with every Packet rx
  //  };

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

    // Display indicator on contolling node
    OLED.setCursor(markerX, dataY[controlNode]);           // Set cursor to prepare for write
    OLED.print("~");

    // Display setpoint value
    OLED.setCursor(setX, setY);           // Set cursor to prepare for write
    //    OLED.setFont(u8g2_font_logisoso16_tn);         // Set a font for 16 Pixel high
    OLED.setFont(u8g2_font_courB12_tn);         // Set a font for 12 Pixel high
    OLED.print(tempSetpoint);             // Display setpoint value

if(!systemError){                         // If system opperating correctly
    // Display controlling Node's current temp
    OLED.setCursor(controlX, controlY);           // Set cursor to prepare for write
    OLED.setFont(u8g2_font_fub14_tn);         // Set a font for 14 Pixel high
    OLED.print(tempData[controlNode][arrayWholeDegrees]);         // Print the whole degree portion
    OLED.print(".");                                         // Print decimal character
    OLED.print(tempData[controlNode][arrayDecimalDegrees]);       // Print the decimal degrees portion
}
else{  // If System Error
  OLED.setCursor(controlX, controlY);           // Set cursor to prepare for write
    OLED.setFont(u8g2_font_fub14_tn);         // Set a font for 14 Pixel high
    OLED.print("ERROR");
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

// ------------------------------------------------------------------
// menuDisplay    menuDisplay   menuDisplay   menuDisplay   menuDisplay
// ------------------------------------------------------------------

// Displayed info for when in Settings mode
void menuDisplay() {
  const byte _menuTitleX = 10;
  const byte _menuTitleY = 12;
  const byte _menuValueX = 15;
  const byte _menuValueY = 23;
  
  //  const char rmName[][9] = {       // Setup an array of char arrays to use for display function loop
  //    "Living:",                 // F() Vs PROGMEM??
  //    "Master:",
  //    "Den:",
  //    "Outside:"            // Moved to Global const area - so can be used in debugPrint statements with every Packet rx
  //  };

  OLED.firstPage();
  do {
    // all graphics commands have to appear within the loop body.

    OLED.setFont(u8g2_font_5x7_mr);             // Set a font for 6 Pixel high
    OLED.setCursor(0, 31);                      // Set cursor to prepare for write
    OLED.print("Adjust or Press to accept");
    
    OLED.setCursor(_menuTitleX, _menuTitleY);   // Set cursor to prepare for write
    OLED.setFont(u8g2_font_courB12_tn);         // Set a font for 12 Pixel high
    OLED.print("Preferred Control Node:");      // Display menu title

    // Display controlling Node's current room
    OLED.setCursor(_menuValueX, _menuValueY);   // Set cursor to prepare for write
    OLED.setFont(u8g2_font_fub14_tn);           // Set a font for 14 Pixel high
    OLED.print((char*)rmName[controlNodeRequested]);     // Print room name for Control Node Requested, char array



    
  } while ( OLED.nextPage() );
}
