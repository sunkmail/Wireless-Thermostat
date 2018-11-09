/*
   To Do:

      1) Confirm / Calibrate Temp sensor
      2) Add RF Tx'r - RFTransmitter.h - Some added to check compile size.  TinyRH too big
      3) Add power saving
      4) Optimise Code
*/

#include <TinyWireM.h>
#include <TinyOzOLED.h>

#include <OneWire.h>
#include <DallasTemperature.h>

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

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress Thermometer_Address;


// **************************************************************
// ****************** RF Module Set-up **************************
/*
  RFNode List so far:
    0 = - Not used in RF for this application
    1 = Bedroom Temperature
    2 = Den Temperature Sensor
    3 = Outside Temperature Sensor
*/
const byte RFNodeID = 1;                    // Set this unit to Node # expected by Rx unit  - Rx unit uses this to confirm valid message

RFTransmitter RFTx(RadioOut_PIN,RFNodeID);               // Set up RF Tx library on RadioOut Pin, set NodeID




// **************** Global Variables ************
unsigned long currentMillis;      // Working/scratchpad variable for checking times

float tempC = 0;            // Temperature in Celcius

unsigned long lastTempRequest = 0;
const int delayInMillis = 750 / (1 << (12 - Resolution));
