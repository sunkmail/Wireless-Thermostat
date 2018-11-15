// ------------------------------------------------------------------
// INTERRUPT     INTERRUPT     INTERRUPT     INTERRUPT     INTERRUPT
// ------------------------------------------------------------------
void isr_EncoderKnob()  {
  static unsigned long lastInterruptTime = 0;       // Static =  Variable to retain value (like global) but only used in this routine.  Doesn't need to be volitle as only called from within the isr.
  unsigned long interruptTime = millis();
  
  if (interruptTime - lastInterruptTime > EncoderBounce) {     // If interrupts come faster than 'EncoderBounce' ms, assume it's a bounce and ignore
        
    if (digitalRead(EncodeB_PIN) == LOW)    // If encoder moving Counter-Clockwise
    {
      if(virtualPosition > 0)
        virtualPosition-- ;             
      
       
    }
    else {
      if(virtualPosition < 100)
        virtualPosition++ ;             // Don't increment above 100%
      
      
    }
  
//    updateDisplay_FLAG = true;      // Set Flag to indicate a Setting has changed - since in ISR, Something MUST have changed
// Display updates every loop in RUN mode
  }
//  debugPrint("ISR",1);
  lastInterruptTime = interruptTime;      // Keep track of when we were here last
}
