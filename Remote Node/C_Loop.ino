void loop() {
  currentMillis = millis();     // Update current time for this loop

  if ((currentMillis - lastTempRequest) >= delayInMillis) { // waited long enough??

    getRawTemp();                     // Get Raw Data
    requestTemp();                    // After fetching Data, request new sample
    lastTempRequest = millis();       // reset timing for conversion

    ConvertRawTomDegreeC();           // Convert Raw data To mDegreeC
  }

  updateDisplay();
  delay(200);
  
}
