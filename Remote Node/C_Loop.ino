void loop() {
  currentMillis = millis();     // Update current time for this loop

  static unsigned long lastTx = 0;
  const int txTimeDelay = 3000;     // Time delay between RF packets

  if ((currentMillis - lastTempRequest) >= delayInMillis) { // waited long enough??

    getRawTemp();                     // Get Raw Data
    requestTemp();                    // After fetching Data, request new sample
    lastTempRequest = millis();       // reset timing for conversion

    ConvertRawTemp();                 // Convert Raw data To useful Temperature Data
    
    updateDisplay();                  // Update the display after each new temp reading
  }


  if (currentMillis > (lastTx + txTimeDelay)) {   // Check if time to send next data packet

//    char *msg = "Hello World!";               // From example code
//    RfTx.send((byte *)msg, strlen(msg) + 1);  // From example code

    makeRfDataPacket();                         // Assemble packet to send
    RfTx.send(RfData, RfBytesToSend);           // Send Data Packet

    lastTx = currentMillis;                     // Set Time of last Tx
  }

  
  //  delay(5000);                                // From example code
  //  RfTx.resend((byte *)msg, strlen(msg) + 1);  // From example code

  //  delay(200);

}
