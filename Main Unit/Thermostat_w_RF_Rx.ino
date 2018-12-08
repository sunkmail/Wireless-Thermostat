const bool isDebug = true;              // Debug messages?
// const bool isDebug = false;


// Testing Errors Found:
//      Setpoint temp raises - was set to 22, found it set at 24.
//        Unable to reproduce on purpose
//          suspect false trigger on encoder ... trying:  increase encoder debounce time
//                                              if not, try hardware debounce
//      Was having 'errors' and "heatOn' cyclings

//    Made hardwired soldered board - Seems to haave settled down encoder creep

//    Made setpoint to (setpoint -1) - needed to increase hysterysis
 
//    increased reject time to 13 sec to try to avoid read errors


// Before using any F():
//Sketch uses 14660 bytes (47%) of program storage space. Maximum is 30720 bytes.
//Global variables use 1409 bytes (68%) of dynamic memory, leaving 639 bytes for local variables. Maximum is 2048 bytes
//
//After Adding F()'s:
//Sketch uses 14800 bytes (48%) of program storage space. Maximum is 30720 bytes.
//Global variables use 1131 bytes (55%) of dynamic memory, leaving 917 bytes for local variables. Maximum is 2048 bytes.
//
// With isDebug = False:
//Sketch uses 13112 bytes (42%) of program storage space. Maximum is 30720 bytes.
//Global variables use 952 bytes (46%) of dynamic memory, leaving 1096 bytes for local variables. Maximum is 2048 bytes.



//Added where possible ... including debug print items in following way:
//debugPrint(F("Setup finished successfully."), 1);
//Not sure if actually works, not yet tested.
//
// if more needed, try "Flash" Library.

/*
   To Do ...   MARK OFF WHEN COMPLETED
    Done -  With Wired connection - Make RF receiver work for single transmiter
    Done - Display sent data to screen
    Done - Make local Temp and Remote both display on screen
    Done - set invalid data to -99.9
    Done - Display "---" data invalid
    Done - Have a set-point for temp to control relay
    Done - include hysterysis - Check if standard value 1 Degree? 2 Degree?  Other
    Done - If remote node not transmit new data after X amount of time, display error indicator (Set data to invalid)
    Done - If Selected remote node in error condition, default back to local temp
    Done - Make some sort of Menu System To Be able to select between nodes
    Done - Switch back to prefered (remote)node when new data received for at least X consecutive packets



    - Make/Test RF Rx works for multiple WIRED remote nodes - use resistor between each Tx and Rx'r????

    - Add EEPROM Storage for last set values
          - Setpoint
          - Preferred Control Node

    - Make/Test RF Rx works for 1 WIRELESS remote node
    - Make/Test RF Rx works for multiple WIRELESS remote nodes

    - Add RTC Module
        - Time Set menu
        - Display Real Time
        - Program Temp Changes (ie, cooler at night)
*/


/*
   Semi-Smart Thermostat to Replace 1970's style mecury switch Thermostat

   Functions:
   - Read and Display Room Temperature
   - Control Relay to activate Heating system (24VAC Solinoid for Hot-Water Radiator)
   - Receive RF signals from remote Nodes
   - Display Temperatures from Remote Nodes (Including one for Outside Temp.)
      - Error indicator if No new info from Node
   - Control which inside Node controls Heating system - Don't allow outside temp to be control
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

const byte EncoderBounce = 15;       // Encoder Debounce time (max) in milliseconds   Was 5. Setting sometimes changed
const byte SwBounce = 50;           // Encoder Debounce time (max) in milliseconds

volatile byte virtualPosition = 0;   // Updated by the ISR (Interrupt Service Routine)
byte virtualPositionSave;            // Place to save position for moving through various settings

bool encoderButton_FLAG = false;

// **************************************************************
// **************** General PIN Assignments *********************

//const byte HeatOn_PIN = 6;       // Pin Assignment for HeatOn - To activate Heater
const byte HeatOn_PIN = 15;       // Pin Assignment for HeatOn (15 = A1) - To activate Heater - Made easiest for SMD PCB Routing

//const byte RadioIn_PIN = 2;      // Pin Assignment for Radio Input- Make easiest for PCB Routing
const byte RadioIn_PIN = 17;      // Pin Assignment for Radio Input (17 = A3) - Made easiest for SMD PCB Routing

const byte Pot_PIN = A0;         // Pin Assignment for Potentiometer - Not currently implemented

const byte TempSense_PIN = 9;


// **************************************************************
// *************** General Const assignments ********************

const bool ButtonActive = LOW;    // Pushbuttons are using pull-ups.  Signal is Low when active

const byte Resolution = 12;    // Temp Sensor Resolution

const byte arrayTempPos = 0;        // Position/Index in data array for various component parts
const byte arrayWholeDegrees = 1;
const byte arrayDecimalDegrees = 2;
const byte arrayPackageID = 3;

const char rmName[][9] = {       // Setup an array of char arrays to use for display function loop
  "Front Rm",                   // F() Vs PROGMEM??
  "Bedroom",
  "Den",
  "Outside:"
};

const byte EncoderMaxPossible = 59;  // Absolute Max encoder can (should) go to - assuming timing will be implemented at some point

const byte tempSetpointDefault = 21;     // Default value for tempSetpoint - used for setup
const byte tempSetpointMin = 10;          // Minimum value for tempSetpoint
const byte tempSetpointMax = 27;         // Maximum value for tempSetpoint

const byte expiryTimeMin = 2;
//const unsigned long expiryTime = (expiryTimeMin * 60 * 1000);   // Expiry time converted from min to ms

const unsigned long expiryTime = 13000;        // FOR TESTING - MAKE SHORT - Remote units on 3 sec send - set to 13 sec


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
unsigned int RfPulseLength = 250;   // ** MUST match Tx ** Time of each pulse section in uSec Bigger = more stable, but slower

/* Constructor */
//RFReceiver(byte inputPin, unsigned int pulseLength = 100) // Default
RFReceiver RfRx(RadioIn_PIN, RfPulseLength);          // Setup with my custom pulse Length value


// 0.96" OLED display module  128x64  -I2C 7-bit address 0x3C ??
/* Constructor */
// U8G2_SSD1306_128X64_NONAME_1_HW_I2C(rotation, [reset [, clock, data]])
U8G2_SSD1306_128X64_NONAME_1_HW_I2C OLED(U8G2_R0);  // roation only setting due to HW I2C and no reset pin

//U8G2_SSD1306_128X64_NONAME_1_HW_I2C OLED(U8G2_R2);  // roation only setting due to HW I2C and no reset pin



// **************************************************************
// ****************** Temp Sensor Set-up ************************

// Data wire pin assignment
//const byte ONE_WIRE_BUS = TempSense_PIN;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(TempSense_PIN);

byte rawTemp[2];              // Raw Sensor data - bypassing Dallas Library

const int delayInMillis = 850;    // give extra time to complete conversion - still greater than 1Hz
// const int delayInMillis = 750 / (1 << (12 - Resolution));   // If Resolution variable, make non-const


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

byte tempSetpoint = tempSetpointDefault;  // Set default Setpoint into setpoint value

byte controlNode = localNode;             // Node to reference for Setpoint comparison - Local Node by Default
byte controlNodeRequested = controlNode;  // prefered node, if data valid make this controlNode - otherwise will default to Local

bool systemError = false;                 // TRUE only IF no valid temps are available

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
