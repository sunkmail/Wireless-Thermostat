
void loop() {
  currentMillis = millis();     // Update current time for this loop

  // Get New Local Temp, if Available
  if ((currentMillis - lastTempRequest) >= delayInMillis) { // waited long enough??

    getRawTemp();                     // Get Raw Data
    requestTemp();                    // After fetching Data, request new sample

    ConvertRawTemp(localNode);        // Convert Raw data To useful Temperature Data
  }

  updateDisplay();

  // Get New RF Info, if Available
  getRfData();

 
}
