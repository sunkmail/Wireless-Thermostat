void setup() {
  // Just for debug, view output on serial monitor -
  if (isDebug)
    Serial.begin(9600);       // Only open Serial comms if in Debug mode


  // **************************************************************
  // ****************** UNUSED Pin Setups *************************
  // Set all pins as Input with Pullup as default
  // Lines below will update to appropriate setting where needed
  // Pin 0 & 1 don't touch - used for serial
  // Pin A4 & A5 - Don't set if using I2C

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);

  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  //  pinMode(A4, INPUT_PULLUP);      // I2C Line
  //  pinMode(A5, INPUT_PULLUP);      // I2C Line

  if (isDebug)
    pinMode(13, OUTPUT);


  // **************************************************************
  // **************** Rotary Encoder Setup ************************

  pinMode(EncodeSW_PIN, INPUT_PULLUP);      // Switch is floating - use the in-built PULLUP so we don't need a resistor

  pinMode(EncodeA_PIN, INPUT_PULLUP);       // Rotary pulses are INPUTs - Possibly floating if not using module board.  (Extra Pull up shouldn't hurt, even if using module board.)
  pinMode(EncodeB_PIN, INPUT_PULLUP);

  // **************************************************************
  // ****************** Other Pin Setups **************************


  digitalWrite(HeatOn_PIN, LOW);        // Default Heating to off.  Pre-Assign to ensure setting when set to output
  pinMode(HeatOn_PIN, OUTPUT);          // Heat On = Relay activation




  // **************************************************************
  // ********************** Misc Setup ****************************

  // 0.96" OLED display module  128x64
  OLED.begin();                             // Start the OLED display Object/Library

  // RF Module
  RfRx.begin();                             // Start RF module to listen for data

  // **************************************************************
  // ************* Temp Sensor Start & Setup ********************

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


  requestTemp();  // Request Temp Conversion

  //  lastTempRequest = millis();     // Now Done in requestTemp function ...
  // Make function return millis value?
  // ie, lastTempRequest = requestTemp(); - requests temp and sets value?

  for (byte x = 0; x <= (RFNodes - 1); x++) { // Set up default temp values in data Arrays to -99.9
    makeInvalid(x);
  }

  virtualPosition = tempSetpointDefault;  // Set default Setpoint into virtualposition

  delay(100);         // Let everything settle for 100 ms on boot-up

  attachInterrupt(digitalPinToInterrupt(EncodeA_PIN), isr_EncoderKnob, LOW);     // Attach interrupt for encoder

  // Ready to go!
  debugPrint(F("Setup finished successfully."), 1);
}
