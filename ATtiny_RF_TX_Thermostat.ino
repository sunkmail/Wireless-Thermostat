

/*
Small module to send a signal to activate a radiator solinoid when below adjustable threshold.  (A Thermostat)

The module is to be controlled by a switch activated power outlet. 
     When power is available on the outlet, the switch must be 'ON', therefore send signal to controller unit
The Radiator unit, a distance from this Thermostat unit, can then switch in a local power source/outlet to active solinoid.

*/


// RFTransmitter - Version: Latest 
#include <RFTransmitter.h>
  // https://andreasrohner.at/posts/Electronics/New-Arduino-library-for-433-Mhz-AM-Radio-Modules/

// TinySnore - Version: Latest 
//#include <tinysnore.h>
  // Send ATtiny to sleep in one line
  
// TinyWireM - Version: Latest 
#include <TinyWireM.h>
#include <USI_TWI_Master.h>

// U8g2 - Version: Latest   // I2C Displays
#include <U8g2lib.h>
#include <U8x8lib.h>

// DallasTemperature - Version: Latest 
#include <DallasTemperature.h>
#include <OneWire.h>


//  PIN Reference  http://highlowtech.org/?p=1695


// RF Rx Module (433 MHz ASK Module)
const byte RadioOut_PIN = PB1; 
const byte RFNodeID = 1;                    // Set this unit to Node # expeected by Rx unit  - Rx unit uses this to confirm valid message
byte *RFData = "heat bedroom";                           // Activate Heat message expected by Rx
const byte MaxRFDataSize = 15;
RFTransmitter RFTx(RadioOut_PIN,RFNodeID);               // Set up RF Tx library on RadioOut Pin, set NodeID


// Other Stuff
const byte tempSense_PIN = PB3;             // Pin to Temperature Sensor
const byte pot_PIN = PB4;                   // Pin to Adjustment Potentiometer

const byte SCL_PIN = PB2;                   // I2C Pins
const byte SDA_PIN = PB0;

const byte Hysterisis = 3;                  // Difference in temp between stop heating and starting again


void setup() {
  // RF Rx Module (433 MHz ASK Module)
  pinMode(RadioOut_PIN, OUTPUT); 

  pinMode(pot_PIN, INPUT);
}



void loop() {
  int setpointTempInC = map(analogRead(pot_PIN), 0, 1023, 15, 35);          // Target Temp in Degrees C
  
    RFTx.send((byte *)RFData, strlen(RFData) + 1);
    delay(20000);                                 // Resend message every 20s
}
