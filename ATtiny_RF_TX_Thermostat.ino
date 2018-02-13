



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

/*  
// SSD1306_minimal - Version: Latest 
#include <SSD1306_minimal.h>
*/

/*
// Tiny4kOLED - Version: Latest 
#include <Tiny4kOLED.h>
#include <font6x8.h>
#include <font8x16.h>

// TinyWireM - Version: Latest          // Not Needed with u8g2 / U8x8 - To Confirm
                                        // Is needed for Tiny4k
#include <TinyWireM.h>            // I2C Master lib for ATTinys which use USI
#include <USI_TWI_Master.h>
*/

/*
// U8g2 - Version: Latest   // I2C Displays
//#include <U8g2lib.h>  Try using just the 8x8 - less memory usage
#include <U8x8lib.h>
//        8x8 seems to use up about 4k!  - Just #include statement increases by 1k ??
//  May need to try:
//          adafruit SSD1306 library???
//          https://github.com/lexus2k/ssd1306    https://bitbucket.org/scargill/ssd1306  *
//          Tiny4k
//          TinyOzOLED
//        Maybe :  https://tinusaur.org/projects/ssd1306xled/
//        Also Maybe: https://github.com/greiman/SSD1306Ascii  *
//        or: http://www.didel.com/OledLib.pdf    (https://github.com/nicoud/Oled)
*/


// DallasTemperature - Version: Latest 
#include <DallasTemperature.h>            // Has Git comments on how to cut down code size
#include <OneWire.h>


//  PIN Reference  http://highlowtech.org/?p=1695

const byte pot_PIN = A2;                   // Pin to Adjustment Potentiometer

//const byte SCL_PIN = 2;                   // I2C Pins
//const byte SDA_PIN = 0;


// **************************************************************
// ****************** Module Set-ups ****************************

// 0.91" OLED display module  128x32

/*
const byte contrast = 255;
  // Constructor //    // U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C(rotation, [reset [, clock, data]])
    // U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C OLED(U8G2_R0);  // roation only setting due to HW I2C and no reset pin  // only if using U8G2lib

U8X8_SSD1306_128X32_UNIVISION_HW_I2C OLED;  // Setup U8x8  - No Rotation setting - Flip MAY be avalable as a separate command
//  Just constructor adds 2.4k
*/


// RF Rx Module (433 MHz ASK Module)
const byte RadioOut_PIN = 1; 

const byte RFNodeID = 1;                    // Set this unit to Node # expeected by Rx unit  - Rx unit uses this to confirm valid message

byte *RFData = "heat bedroom";                           // Activate Heat message expected by Rx

const byte MaxRFDataSize = 15;

RFTransmitter RFTx(RadioOut_PIN,RFNodeID);               // Set up RF Tx library on RadioOut Pin, set NodeID



// Temperature Sensor (DS18B20)       ******************** May need to use only onewire and manually send data *************************
//                                                DallasTemperature may be too big and not ATtiny Compatible
//                                                http://sheepdogguides.com/arduino/ar3ne1tt.htm may be useful


const byte TempSense_PIN = 3;             // Pin to Temperature Sensor

      /* Constructor */
OneWire oneWire(TempSense_PIN);           // Setup a oneWire instance to communicate with any OneWire devices  
                                          // (not just Maxim/Dallas temperature ICs)
DallasTemperature tempSensor(&oneWire);   // Pass our oneWire reference to Dallas Temperature Library. 

const byte TempSensorResolution = 9;      // Integer value for sensor precision.  Can be 9, 10, 11 or 12 bits
/*
          Mode      Resol   Conversion time
          9 bits    0.5°C     93.75 ms
          10 bits   0.25°C    187.5 ms
          11 bits   0.125°C   375 ms
          12 bits   0.0625°C  750 ms
*/
const unsigned int TempSensorConvTime = 100;  // Min. Time in ms to wait between request for temp conv. and read of temperature - Based on info above.

float currentTempC = 0.0;                 // Current Room Temperature in C
const byte tempHysteresis = 2;                  // To prevent frequent On/Off cycles near target temp               
                                          // Difference in temp between stop heating and starting again


// Other Stuff

unsigned long currentMillis;      // Working/scratchpad variable for checking times



void setup() {
  // RF Rx Module (433 MHz ASK Module)
  pinMode(RadioOut_PIN, OUTPUT); 

  pinMode(pot_PIN, INPUT);

  tempSensor.begin();                                 // Start the Library
  tempSensor.setWaitForConversion(false);             // Put into Async. Mode
  tempSensor.requestTemperatures();             // Send request for current temp - To get initial value going
  
  
/*  
  TinyWireM.begin();      // Start I2C Library
  oled.begin();             // Start up tiny4kOLED
  oled.clear();
  oled.on();
  oled.switchRenderFrame();
*/

/*
  OLED.begin();                                   // Start Display Library
  OLED.setFont(u8x8_font_pressstart2p_u);         // Set Prefered Font
  OLED.setContrast(contrast);                     // Set display Contrast
*/
  delay(100);
}



void loop() {
  // byte setpointTempC = map(analogRead(pot_PIN), 0, 1023, 15, 35);          // Target Temp in Degrees C, Set by Pot    - Left out for initial testing
  byte setpointTempC = 21;      // Starting point for testing

  checkRoomTemp();                  // get current temp

  if(currentTempC < (setpointTempC + tempHysteresis)){
      RFTx.send((byte *)RFData, strlen(RFData) + 1);
  }  
  
    delay(20000);                                 // Resend message every 20s
                            // Make non-blocking to allow for pot value adjustment/screen updates
}



// ------------------------------------------------------------------
// checkRoomTemp     checkRoomTemp     checkRoomTemp     checkRoomTemp
// ------------------------------------------------------------------
void checkRoomTemp(){
  static unsigned long lastTempCheck = 0;                       // keep track of when last converstion started
  
  if(currentMillis - lastTempCheck > TempSensorConvTime){         // if the last read was more than conversion time: 
   
    currentTempC = tempSensor.getTempCByIndex(0);             // Read Current Temperature in Degrees C     
    tempSensor.requestTemperatures();                         // Send request for new temp conversion
    lastTempCheck = currentMillis;                            // reset timer reference for conversion
  }
}
