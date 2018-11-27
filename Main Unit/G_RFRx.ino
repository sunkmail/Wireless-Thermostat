
void getRfData(void) {
  byte msg[MAX_PACKAGE_SIZE];   // Make smaller to save Memory, if needed.  Default 84.  I use 3?
  byte packageId = 0;           // Package identifier ... 0 - 255
  byte len = 0;                 // Part of example - Not needed for my usage
  byte nodeId = 0;              // Default to unused value.
  /*
    RFNode List so far:
    0 = Local Device -- Not used in RF for this application
    1 = Bedroom Temperature
    2 = Den Temperature Sensor
    3 = Outside Temperature Sensor
  */



  if (RfRx.ready()) {                 // If data recieved, and not a duplicate
    len = RfRx.recvPackage((byte *)msg, &nodeId, &packageId);
    // From Tx:
    //    makeRfDataPacket();                         // Assemble packet to send
    //    RfTx.send(RfData, RfBytesToSend);           // Send Data Packet   (const byte RfBytesToSend = 3;)

    /*
       byte tempData[RFNodes][4];    // (RFNodes = 4) Rows x 4 Column array for node data - 16 bytes memory
          Row = Node #    Node 0 = Local Data
          Columns 0 - 3 are: tempPos, wholeDegreeC, decimalDegreeC, packageId
    */



    for (byte i = 0; i <= 2; i++) {       // For 0 -> (RfBytesToSend-1), transfer data to appropriate location
      tempData[nodeId][i] = msg[i];
    }
    tempData[nodeId][3] = packageId;      // Store latest PackageId
    tempDataRxTime[nodeId] =  currentMillis;   // Sore time of Rx


    debugPrint("", 1);
    debugPrint("Package: ");
    debugPrint(packageId, 1);
    debugPrint("RF Node: ");
    debugPrint(nodeId, 1);
    debugPrint("Message: ");
    if (msg[0] == 0)
      debugPrint("-");
    else
      debugPrint("+");

    debugPrint(msg[1]);
    debugPrint(".");
    debugPrint(msg[2]);
    debugPrint(" C", 1);

    debugPrint("", 1);
    debugPrint("Current Status:", 1);
    debugPrint("   Setpoint: ");
    debugPrint(tempSetpoint, 1);
    debugPrint("   Controlled by :");
    debugPrint((char*)rmName[controlNode]);           // Print room name for Node, char array,1);
    debugPrint(" currently at: ");
    debugPrint(tempData[controlNode][arrayWholeDegrees], 1);
    debugPrint("   Heater is: ");
    if (digitalRead(HeatOn_PIN) == 0)
      debugPrint("OFF", 1);
    else
      debugPrint("ON", 1);

    //    debugPrint("",1);
    //    debugPrint("tempData[nodeId] ");
    //    debugPrint(tempData[nodeId][0]);
    //    debugPrint(tempData[nodeId][1]);
    //    debugPrint(tempData[nodeId][2],1);
  }
}
