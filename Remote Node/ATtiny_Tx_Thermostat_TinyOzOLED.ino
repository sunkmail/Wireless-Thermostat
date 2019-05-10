//  Must be set to 8MHz for RF to work?? - NEED to Confirm 
//  
/*
   To Do:

      - Remove unneeded font characters to save space?   . Make Local copy of Library?
      - Use Timer ISR for sending data?
      - Add power saving
           - Sleep Modes?
           - Disable ACC?
           - Input with internal pull-ups for unused pins
           - check nick gammon webpage for ideas
      - Optimise Code
      - Make larger font??      
      
      Todo's completed:
      
      - Confirm / Calibrate Temp sensor - Tested waterproof sensor - within 1/8 degree in ice water.
      - Need pull-up on Reset? - No - via forums ... seems to work with internal Pull up 
      - Add RF Tx'r - RFTransmitter.h

*/

#include <TinyWireM.h>
#include <TinyOzOLED.h>

#include <OneWire.h>

#include <RFTransmitter.h>    // https://andreasrohner.at/posts/Electronics/New-Arduino-library-for-433-Mhz-AM-Radio-Modules/


const byte TempSense_PIN = 3;
const byte Pot_PIN = A2;

const byte RadioOut_PIN = 1;
//const byte RFIn_PIN = 4;        // Not needed for project but is needed for Library

// Data wire pin assignment
const byte ONE_WIRE_BUS = TempSense_PIN;

const int Resolution = 12;

// **************************************************************
// ****************** OLED Module Set-up ************************

// 0.91" OLED display module  128x32  -I2C 7-bit address 0x3C

const byte displayI2CAddress = 0x3C;
const byte SET_DisplayPreCharge = 0xD9;     //  0-7 , Higher = Brighter ??
const byte SET_DisplayVcomDeselect = 0xdb;  // 0,2,3 are valid  - Higher = Brighter

const byte Brightness = 0;    // 0 - 255, Higher = Brighter, 127 = Default

//byte PreCharge1Default = 1;
//byte PreCharge1Min = 0;
//byte PreCharge1Max = 15;
const byte PreCharge1 = 0;

//byte PreCharge2Default = 15;
//byte PreCharge2Min = 0;
//byte PreCharge2Max = 15;
const byte PreCharge2 = 0;

//byte vComDefault = 4;       // Default == 4 - Range = 0-7
//byte vComMin = 0;
//byte vComMax = 7;
const byte vCom = 0;

// **************************************************************
// ****************** Temp Sensor Set-up ************************

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

byte rawTemp[2];              // Raw Sensor data - bypassing Dallas Library for effeciency

unsigned long lastTempRequest = 0;
const int delayInMillis = 750 / (1 << (12 - Resolution));

byte tempPos = true;    // Is the temp 0 or above

byte wholeDegreeC = 0;    // Operating range well below range of byte
byte decimalDegreeC = 0;  // Only send 1 Sig. Bit


// **************************************************************
// ****************** RF Module Set-up **************************
/*
  RFNode List so far:
    0 = - Not used in RF for this application - (Library default)
    1 = Bedroom Temperature
    2 = Den Temperature Sensor
    3 = Outside Temperature Sensor
*/
const byte RfNodeID = 3;                    // Set this unit to Node # to identify signal source

const unsigned int RfPulseLength = 250;           // Time of each pulse section in uSec  - Bigger = more stable, but slower 
const unsigned int RfBackoffDelay = 100;          // time in milliseconds between the repeated transmissions
const byte RfResendCount = 2;                     // number of times the transmission of a packet is repeated.  More = Better data integrity, but slower

//RFTransmitter(byte outputPin, byte nodeId = 0, unsigned int pulseLength = 100, unsigned int backoffDelay = 100, byte resendCount = 1) 
RFTransmitter RfTx(RadioOut_PIN,RfNodeID,RfPulseLength,RfBackoffDelay,RfResendCount);  // Set up RF Tx library on RadioOut Pin, set NodeID




// **************** Global Variables ************
unsigned long currentMillis;      // Working/scratchpad variable for checking times

const byte RfBytesToSend = 3;
byte RfData[RfBytesToSend];   // Data Packet of 3 bytes:  [tempPos, WholeDegreeC, DecimalDegreeC]
