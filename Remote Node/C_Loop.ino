void loop() {
  currentMillis = millis();     // Update current time for this loop

  if ((currentMillis - lastTempRequest) >= delayInMillis) { // waited long enough??
    tempC = sensors.getTempCByIndex(0);
    sensors.requestTemperatures();      // After fetching Temp, request new sample
    lastTempRequest = millis();
  }
  
  updateDisplay();
  delay(200);
  
}
