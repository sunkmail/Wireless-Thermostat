void setup() {

  pinMode(TempSense_PIN, INPUT);      // Digital Input from Temp Sensor
  pinMode(Pot_PIN, INPUT);            // Analog input from Pot

  digitalWrite(RadioOut_PIN, false);  // Default RF Tx state to off.
  pinMode(RadioOut_PIN, OUTPUT);      // Output to RF Transmit Data Pin


  // ************* Temp Sensor Start & Setup ********************
//  sensors.begin();
//  sensors.getAddress(Thermometer_Address, 0);
//  sensors.setResolution(Thermometer_Address, Resolution);    // set the resolution
//  sensors.setWaitForConversion(false);
//
//  sensors.requestTemperatures();

    // Set resolution ... should be correct by default, but just in case. :)
  oneWire.reset(); 
  oneWire.write(0xCC);  // Bypass ROM command - since only 1 sensor used
  oneWire.write(0x4E);  // Write to scratchpad command
  oneWire.write(0);     // Write Dummy value to alarm1 register
  oneWire.write(100);   // Write Dummy value to alarm2 register
  oneWire.write(0x7F);  // set resolution to 12
  delay(10);
  oneWire.reset(); 
  
// Do a read eeprom/scratchpad test on bootup to check resolution.  if already correct, do bother with above.
//  only impliment after above is working.


  oneWire.reset(); 
  oneWire.write(0xCC);  // Bypass ROM command - since only 1 sensor used
  oneWire.write(0x44);  // Request Temp Conversion

  //  delayInMillis = 750 / (1 << (12 - Resolution));   // Added as a const for 12 bit res.
  lastTempRequest = millis();


  // *********** 0.91" OLED display module  128x32 ***************
  OzOled.init();  //initialze OLED display

  OzOled.sendCommand(0xA8);   // Set MUX
  OzOled.sendCommand(0x1F);   // for 32 Lines

  OzOled.sendCommand(0xDA);   // Set ComPins  (Not sure How this helps - saw on YouTube - Makes text look better)
  OzOled.sendCommand(0x02);   // youtube.com/watch?v=zYYM_8Znk3E

  OzOled.sendCommand(0xC8);   // Vertically Flip Display  (Default is 0xC0)
  OzOled.sendCommand(0xA1);   //                          (Default is 0xA0)


  OzOled.setBrightness(Brightness);         // Set Brightness
  displayPreCharge(PreCharge1, PreCharge2); // To absolute min
  displayVcom(vCom);                        // Using all adjustments

  //  OzOled.clearDisplay();           //clear the screen and set start position to top left corner -- In default setup
  //  OzOled.setNormalDisplay();       //Set display to Normal mode         -- In default setup
  //  OzOled.setPageMode();            //Set addressing mode to Page Mode   -- In default setup


  // ************** RF Rx Module (433 MHz ASK Module) ******************
  //    Nothing to Set-up

  // Ready to go!

}
