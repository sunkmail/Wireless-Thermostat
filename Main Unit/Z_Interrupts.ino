// ------------------------------------------------------------------
// INTERRUPT     INTERRUPT     INTERRUPT     INTERRUPT     INTERRUPT
// ------------------------------------------------------------------
void isr_EncoderKnob()  {

  static unsigned long lastInterruptTime = 0;       // Static =  Variable to retain value (like global) but only used in this routine.  Doesn't need to be volitle as only called from within the isr.
  unsigned long interruptTime = millis();

  if (interruptTime - lastInterruptTime > EncoderBounce) {     // If interrupts come faster than 'EncoderBounce' ms, assume it's a bounce and ignore

    if (digitalRead(EncodeB_PIN) == LOW)    // If encoder moving Counter-Clockwise
    {
      if (virtualPosition > 0)
        virtualPosition-- ;
      else
        virtualPosition = EncoderMaxPossible;     // If already at lowest value, roll over to Max Value

    }
    else {
      if (virtualPosition < EncoderMaxPossible)
        virtualPosition++ ;             // Don't increment above 'EncoderMaxPossible' Const
      else
        virtualPosition = 0;            // If already at Max Value, roll over to 0
    }

  }
  //  debugPrint("ISR",1);
  lastInterruptTime = interruptTime;      // Keep track of when we were here last
}
