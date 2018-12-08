// ------------------------------------------------------------------
// checkEncoderButton     checkEncoderButton     checkEncoderButton     checkEncoderButton
// ------------------------------------------------------------------
void checkEncoderButton()  {
  static unsigned long _lastEncoderButtonPress = 0;       // Static =  Variable to retain value (like global) but only used in this routine.
  static bool _stillPressed = false;

  if (digitalRead(EncodeSW_PIN) == ButtonActive) {       // If encoder button pushed ...
    if (_stillPressed == false) {
      if (currentMillis - _lastEncoderButtonPress > SwBounce) {     // If checked faster than SwBounce ms, assume it's a bounce and ignore
        encoderButton_FLAG = true;                                //  If not a bounce, flag as a valid push
        _stillPressed = true;                                    // Flag button 'still' being pressed for next loop
        _lastEncoderButtonPress = currentMillis;                   // & Update last valid push time
        debugPrint(F("Encoder Button Pressed"), 1);
      }
    }
  }
  else {
    _stillPressed = false;              // If button not being pressed, reset flag
  }
}


// ------------------------------------------------------------------
// updateSetpoint   updateSetpoint    updateSetpoint    updateSetpoint
// ------------------------------------------------------------------
void updateSetpoint(void) {

  tempSetpoint = virtualPosition;         // Update setpoint to current virtualposition value

  if (tempSetpoint < tempSetpointMin) {   // If Setpoint has fallen below minimum allowable setpoint ...
    tempSetpoint = tempSetpointMin;        // Set Setpoint to the lowest allowable value
  }
  else if (tempSetpoint > tempSetpointMax) {    // If Setpoint has risen above maximum allowable setpoint ...
    tempSetpoint = tempSetpointMax;        // Set Setpoint to the highest allowable value
  }

  //  debugPrint(F("Temp. Setpoint: "));        // Display current tempSetpoint - Test confirmation
  //  debugPrint(tempSetpoint, 1);

}



// ------------------------------------------------------------------
// doSetpoint   doSetpoint    doSetpoint    doSetpoint    doSetpoint
// ------------------------------------------------------------------
void doSetpoint(void) {

  //  if( temp < setpoint ) -> turn on heat
  if (tempData[controlNode][arrayWholeDegrees] < (tempSetpoint-1)) {      // If controlling Node temp is less than setpoint - Assumption made: inside temp will never go below zero
    digitalWrite(HeatOn_PIN, HIGH);                                   // Turn Heating ON. (Activate Relay)
  }
  else if (tempData[controlNode][arrayWholeDegrees] > tempSetpoint) { //Using '>' Vs. '>=' gives 1.1 Vs. 0.1 Degrees of Hysterysis
    digitalWrite(HeatOn_PIN, LOW);                                     // Turn Heating off. (De-Energise Relay)
  }

  if (isDebug)
    digitalWrite(13, digitalRead(HeatOn_PIN));   // If in debug mode, use built in LED on Pin 13 to indicate heater status.
}

// ------------------------------------------------------------------
// checkDataAge   checkDataAge    checkDataAge    checkDataAge
// ------------------------------------------------------------------
void checkDataAge(void) {
  static byte _lastPacket[RFNodes] = {0, 0, 0, 0};    // Create Array to store last good packet ID

  // Do for loop to check if lst data time is less than current time - expiry time for all variables
  // if expiried, set to invalid
  // If fresh Data, but first after being invalid, don't use, may be corrupt, wait for next packet

  for (byte i = 0; i <= (RFNodes - 1); i++) {               // Check each node's data
    if ((currentMillis - tempDataRxTime[i]) > expiryTime) {    // If too old ...
      //      debugPrint("Data invalid - Timeout, Node: ");
      //      debugPrint((char*)rmName[i], 1);                          // Print room name for Node, char array
      makeInvalid(i);                                        // Set data to invalid value, -99.9
      _lastPacket[i] = 0;                                     // And Flag that data has become too old for next check
    }
    else {                                                  // If data isn't too old
      if (_lastPacket[i] == 0) {                              // but Data WAS invalid on last loop (this is the first new packet)
        makeInvalid(i);                                         // make data invalid for this loop
        _lastPacket[i] = tempData[i][arrayPackageID];           // but record a good packet was received
      }
      else {                                                // else: Data isn't too old, or the first packet
        _lastPacket[i] = tempData[i][arrayPackageID];       // keep data, just update last packet value
      }
    }
  }
}

// ------------------------------------------------------------------
// isControlValid   isControlValid    isControlValid    isControlValid
// ------------------------------------------------------------------
void isControlValid(void) {
  if (controlNodeRequested != localNode) {                  // Check if wanting to use a remote node to control heating ....
    if (tempData[controlNodeRequested][arrayTempPos] == false) {      // if Invalid Temp Data on Control Node  (Assumption - Inside temps will never be < zero)
      controlNode = localNode;                              // ... Make the local node the Control Node
    }
    else {
      controlNode = controlNodeRequested;                   // Otherwise, Data Valid, therefore use requested Node
    }
  }
  else {
    controlNode = localNode;                              // Make the local node the Control Node, as requested
  }
}

// ------------------------------------------------------------------
// enterMenu    enterMenu   enterMenu   enterMenu   enterMenu   enterMenu
// ------------------------------------------------------------------
void enterMenu(void) {
  debugPrint(F("Entering Menu System"), 1);

  // Menu idea ... When button pressed, multiple choice menu
  // 1) Exit                    Keep it first in case of accidental button press
  // 2) Control Node            Choose default controlling node
  // 3) Set time                IF RTC Implemented

  // Menu only for control node at this point - keep simple until more functions needed

  encoderButton_FLAG = false;                   // Restore Button flag

  noInterrupts();                               // Turn off interrupts while changing variable value
  virtualPositionSave = virtualPosition;        // Save the current virtual position from RUN mode
  virtualPosition = controlNodeRequested;       // Set VirtualPosition to current preferred setting
  interrupts();

  while (encoderButton_FLAG == false) {         // Until Button Pressed ...
    currentMillis = millis();                     // Update current time
    programSettings();                            // Enter the program settings menus
    checkEncoderButton();                         // Poll Encoder Pushbutton   - Flag as needed
  }
  noInterrupts();
  virtualPosition = virtualPositionSave;        // Restore the virtual position for RUN mode
  encoderButton_FLAG = false;                   // Restore Button Flag
  interrupts();

  debugPrint(F("Exiting Menu System"), 1);
}


// ------------------------------------------------------------------
// programSettings    programSettings   programSettings   programSettings   programSettings
// ------------------------------------------------------------------
void programSettings(void) {
  // Menu only for control node at this point - keep simple until more functions needed
  if (virtualPosition == EncoderMaxPossible)    // If encoder rolled over from zero
    virtualPosition = (RFNodes - 2);              //Set to max valid value
  else if (virtualPosition > (RFNodes - 2)) {   // IF Control node is above a valid value, set to lowest - Outside is NOT a valid option
    virtualPosition = localNode;         // Therefore (node[array] - 2) allows only inside options
  }
  //  debugPrint(F("Entered programSetting Routine"),1);
  controlNodeRequested = virtualPosition;   // Set requested Control Node to current virtual position setting

  menuDisplay();                            // Update display with current Settings data
}


// ------------------------------------------------------------------
// makeInvalid    makeInvalid   makeInvalid   makeInvalid   makeInvalid
// ------------------------------------------------------------------
void makeInvalid(byte _node) {
  tempData[_node][arrayTempPos] = false;                     // Set data to invalid value, -99.9
  tempData[_node][arrayWholeDegrees] = 99;
  tempData[_node][arrayDecimalDegrees] = 9;
}
