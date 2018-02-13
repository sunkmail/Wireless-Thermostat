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
#include <tinysnore.h>
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
const byte RadioOut_PIN = 1; 
const byte RFNodeID = 1;                    // Set this unit to Node # expeected by Rx unit  - Rx unit uses this to confirm valid message
byte *RFData = "heat bedroom";                           // Activate Heat message expected by Rx
const byte MaxRFDataSize = 15;
RFTransmitter RFTx(RadioOut_PIN,RFNodeID);               // Set up RF Tx library on RadioOut Pin, set NodeID



// Temperature Sensor (DS18B20)   
const byte TempSense_PIN = 3;             // Pin to Temperature Sensor

/* Constructor */
OneWire oneWire(TempSense_PIN);           // Setup a oneWire instance to communicate with any OneWire devices  
                                          // (not just Maxim/Dallas temperature ICs)
DallasTemperature tempSensor(&oneWire);   // Pass our oneWire reference to Dallas Temperature Library. 

//const byte TempSensorCount = 1;           // How many sensors are we expecting      // not needed if only using 1
byte tempSensorAddr[8];                   // Variable to store device's uniquie ID
const byte TempSensorResolution = 9;      // Integer value for sensor precision.  Can be 9, 10, 11 or 12 bits
/*
          Mode      Resol   Conversion time
          9 bits    0.5°C     93.75 ms
          10 bits   0.25°C    187.5 ms
          11 bits   0.125°C   375 ms
          12 bits   0.0625°C  750 ms
*/
const unsigned int TempSensorConvTime = 100;  // Time in ms to wait between request for temp conv. and read of temperature - Based on info above.
float currentTempC = 0.0;                 // Current Room Temperature in C
// byte targetTempC = 21;                    // Target Room Temperature (for thermostat control)
byte tempHysteresis = 2;                  // To prevent frequent On/Off cycles near target temp
//const byte Hysterisis = 3;                  // Difference in temp between stop heating and starting again


// Other Stuff

const byte pot_PIN = A2;                   // Pin to Adjustment Potentiometer

const byte SCL_PIN = 2;                   // I2C Pins
const byte SDA_PIN = 0;

unsigned long currentMillis;      // Working/scratchpad variable for checking times



void setup() {
  // RF Rx Module (433 MHz ASK Module)
  pinMode(RadioOut_PIN, OUTPUT); 

  pinMode(pot_PIN, INPUT);

  tempSensor.begin();                                 // Start the Library
  tempSensor.getAddress(tempSensorAddr, 0);           // Get addrress for device at index 0  (only sensor, no loop needed)
                                                        // Addresses used as faster than doing by index.

    tempSensor.setResolution(tempSensorAddr, TempSensorResolution);     // Set resolution of temp sensor
    tempSensor.setWaitForConversion(false);             // Put into Async. Mode
    // tempSensor.setCheckForConversion(true);             // Program will look for flag that conversion complete    //NOT ACTUALLY SURE what this setting does.  Will just track millis
    tempSensor.requestTemperaturesByAddress(tempSensorAddr);    // Send request for current temp - To get initial value going
  delay(100);
}



void loop() {
  int setpointTempInC = map(analogRead(pot_PIN), 0, 1023, 15, 35);          // Target Temp in Degrees C

  checkRoomTemp();                  // get current temp

  
    RFTx.send((byte *)RFData, strlen(RFData) + 1);
    
    delay(20000);                                 // Resend message every 20s
}



// ------------------------------------------------------------------
// checkRoomTemp     checkRoomTemp     checkRoomTemp     checkRoomTemp
// ------------------------------------------------------------------
void checkRoomTemp(){
  static unsigned long lastTempCheck = 0;                       // keep track of when last converstion started
  
  if(currentMillis - lastTempCheck > TempSensorConvTime){         // if the last read was more than conversion time: 
    currentTempC = tempSensor.getTempC(tempSensorAddr);           // Read Current Temperature in Degrees C 
    tempSensor.requestTemperaturesByAddress(tempSensorAddr);    // Send request for new temp conversion
    lastTempCheck = currentMillis;                                // reset timer reference for conversion
//    updateDisplay_FLAG = true;                                       // Set flag to trigger display update
  }
}
