const bool isDebug = true;              // Debug messages?
//const bool isDebug = false;

/*
   To Do ...   MARK OFF WHEN COMPLETED
    Done -  With Wired connection - Make RF receiver work for single transmiter
      - Display sent data to screen

    - Make local Temp and Remote both display on screen
    - Make RF Rx work for multiple remote nodes

    - If remote node not transmit new data after X amount of time, display error indicator

    - Have a set-point for temp to control relay
      - include hysterysis - Check if standard value 1 Degree? 2 Degree?  Other

      - Be able to select between nodes

      - If remote node in error condition, default back to local temp until new data received
*/


/*
   Semi-Smart Thermostat to Replace 1970's style mecury switch Thermostat

   Functions:
   - Read and Display Room Temperature
   - Control Relay to activate Heating system (24VAC Solinoid for Hot-Water Radiator)
   - Receive RF signals from remote Nodes
   - Display Temperatures from Remote Nodes (Including one for Outside Temp.)
      - Error indicator if No new info from Node
   - Control which internal Node controls Heating system
      - Indicate active Node on Display
      - Fallback to 'local' sensor if 'Control Node' not active


      Hardware Sections:
        DS18B20 Temp. Sensor
        0.96" 128x64 OLED, I2C
        433 MHz Radio Module
        Rotary Encoder, with push button switch
        12 VDC SPDT Relay
        "Tiny RTC" I2C Real time Clock module
*/

#include <U8g2lib.h>
#include <U8x8lib.h>

#include <OneWire.h>

#include <RFReceiver.h>

// **************************************************************
// ************** Rotary Encoder Assignments ********************


const byte EncodeA_PIN = 3;      // HW Int1 - CLK signal from Rotary Encoder (Pin A) - Used for generating interrupts
const byte EncodeB_PIN = 4;      // DT (data) signal from Rotary Encoder (Pin B) - Used for reading direction
const byte EncodeSW_PIN = 5;     // Push button switch on Rotary Encoder

const byte EncoderBounce = 5;       // Encoder Debounce time (max) in milliseconds
const byte SwBounce = 50;           // Encoder Debounce time (max) in milliseconds

volatile byte virtualPosition = 80;   // Updated by the ISR (Interrupt Service Routine)
byte virtualPositionSave = 80;        // Place to save position for moving through various settings



// **************************************************************
// **************** General PIN Assignments *********************

const byte HeatOn_PIN = 13;       // Pin Assignment for HeatOn - To activate Heater

const byte RadioIn_PIN = 2;       // Pin Assignment for Radio Input

const byte Pot_PIN = A0;           // Pin Assignment for Potentiometer - Not currently implemented

const byte TempSense_PIN = 9;


// **************************************************************
// *************** General Const assignments ********************

const byte Resolution = 12;    // Temp Sensor Resolution

const byte arrayTempPos = 0;        // Position in data array for various parts
const byte arrayWholeDegrees = 1;
const byte arrayDecimalDegrees = 2;
const byte arrayPackageID = 3;

// **************************************************************
// ********************* Module Set-ups *************************

// Declare Radio Module
const byte RFNodes = 4;   // Total number used - to set up data array

const byte localNode = 0;     // Declare Node Names for array referencing
const byte bedroomNode = 1;
const byte denNode = 2;
const byte outsideNode = 3;
/*
  RFNode List so far:
    0 = - Not used in RF for this application, Refers to Local Data
    1 = Bedroom Temperature
    2 = Den Temperature Sensor
    3 = Outside Temperature Sensor
*/
unsigned int RfPulseLength = 250;           // ** MUST match Tx ** Time of each pulse section in uSec Bigger = more stable, but slower

/* Constructor */
//RFReceiver(byte inputPin, unsigned int pulseLength = 100) // Default
RFReceiver RfRx(RadioIn_PIN, RfPulseLength);          // Setup with my custom pulse Length value


// 0.96" OLED display module  128x62  -I2C 7-bit address 0x3C ??

/* Constructor */
// U8G2_SSD1306_128X64_NONAME_1_HW_I2C(rotation, [reset [, clock, data]])
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C OLED(U8G2_R0);  // roation only setting due to HW I2C and no reset pin


// Until other module arrives:

// 0.91" OLED display module  128x32
/* Constructor */    // U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C(rotation, [reset [, clock, data]])
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C OLED(U8G2_R0);  // roation only setting due to HW I2C and not reset pin


// **************************************************************
// ****************** Temp Sensor Set-up ************************

// Data wire pin assignment
//const byte ONE_WIRE_BUS = TempSense_PIN;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(TempSense_PIN);

byte rawTemp[2];              // Raw Sensor data - bypassing Dallas Library

const int delayInMillis = 750 / (1 << (12 - Resolution));   // If Resolution variable, make non-const

//byte tempPos = true;      // Is the temp 0 or above
//byte wholeDegreeC = 0;    // Operating range well below range of byte
//byte decimalDegreeC = 0;  // Only send 1 Sig. Bit

// **************************************************************
// ********************* Global Variables ***********************

unsigned long currentMillis;      // Working/scratchpad variable for checking times

unsigned long lastTempRequest;    // Value for when the last temp request was made

byte tempData[RFNodes][4];    // (RFNodes = 4) Rows x 4 Column array for node data - 16 bytes memory
            // Row = Node #    Node 0 = Local Data
            // Columns 0 - 3 are: tempPos, wholeDegreeC, decimalDegreeC, packageId 
            
unsigned long tempDataRxTime[RFNodes];  // Array to hold time of most recent data Rx from each node
                                        // Position 0 not used - can spare memory to make coding easier though

// ----------------------------------------------------------------------------
// DEBUG      DEBUG      DEBUG      DEBUG      DEBUG      DEBUG      DEBUG
// One size fits all Serial Monitor debugging messages
// ----------------------------------------------------------------------------
//bool isDebug = true;              // Debug messages?          Moved to top of code - easier to find
//bool isDebug = false;

template<typename T> void debugPrint(T printMe, bool newLine = false) {
  if (isDebug) {
    if (newLine) {
      Serial.println(printMe);
    }
    else {
      Serial.print(printMe);
    }
    Serial.flush();
  }
}
