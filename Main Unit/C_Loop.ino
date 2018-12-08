
void loop() {
  currentMillis = millis();     // Update current time for this loop

  checkEncoderButton();       // Poll Encoder Pushbutton   - Flag as needed
  if (encoderButton_FLAG == true)
    enterMenu();


  // Get New Local Temp, if Available
  if ((currentMillis - lastTempRequest) >= delayInMillis) { // waited long enough??
    getRawTemp();                     // Get Raw Data
    requestTemp();                    // After fetching Data, request new sample
    ConvertRawTemp(localNode);        // Convert Raw data To useful Temperature Data
  }

  checkDataAge();                     // Check integrity of Data ... Has it been too long since last valid reading received?

  isControlValid();                   // If Preferred control node not valid, set Control to local node

  updateSetpoint();                   // Update setpoint with virtual position value

  //  if (tempData[controlNode][arrayTempPos] == true) { // If Control Node has valid data ...
  if ((tempData[controlNode][arrayTempPos] == true) && (tempData[controlNode][arrayWholeDegrees] > 0) && (tempData[controlNode][arrayWholeDegrees] != 85) ) { // If Control Node has valid data ...
    systemError = false;                // Remove Error Flag, If present
    doSetpoint();                       // Compare current temp and turn on/off heating as required
  }
  else {                                // If no Valid Data on Control Pin ...
    //(At this point, control is either a valid remote, or set to local)
    // If local doesn't have valid data, assume error
    systemError = true;                     // Set System Error Flag
    digitalWrite(HeatOn_PIN, LOW);          // Turn Heating off. (De-Energise Relay)
    if (isDebug)
      digitalWrite(13, digitalRead(HeatOn_PIN));   // If in debug mode, use built in LED on Pin 13 to indicate heater status.
  }

  updateDisplay();

  // Get New RF Info, if Available
  getRfData();

  //  debugPrint(F("Rotary Position: "));        // Display Rotary position value - Test confirmation
  //  debugPrint(virtualPosition, 1);

  // doSetpoint();

}
