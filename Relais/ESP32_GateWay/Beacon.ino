/* Goal: Retrieve data from payload*/
/* packetToParse: Packet received to be analyzed
 * bufferIndex: Index of the beacon to store the data
 */
void parsePayload(uint8_t* packetToParse, uint8_t bufferIndex) {

  //Packet Kontakt TLM ID: 6AFE030902
  if (packetToParse[5] == 0x6A && packetToParse[6] == 0xFE && packetToParse[7] == 0x03 && packetToParse[8] == 0x09 && packetToParse[9] == 0x02) {
    buffer[bufferIndex].batteryLevel = packetToParse[24];
    buffer[bufferIndex].temperature = packetToParse[28];
    buffer[bufferIndex].timeSinceLastMove = packetToParse[16];
  }
  
  // Packet Kontakt detection ID: 6AFE02
  else if(packetToParse[5] == 0x6A && packetToParse[6] == 0xFE && packetToParse[7] == 0x02) {
    buffer[bufferIndex].batteryLevel = packetToParse[11];           
    buffer[bufferIndex].txPower = packetToParse[12]; //To convert correctly, in dbm [TO DO ]
  }
  
  //Packet Button ID: 6AFE03030D  
  else if (packetToParse[5] == 0x6A && packetToParse[6] == 0xFE && packetToParse[7] == 0x03 && packetToParse[8] == 0x03 && packetToParse[9] == 0x0D) {
    buffer[bufferIndex].timeSinceLastClick = packetToParse[10];
  }
}

/* Goal: get RSSI beacon*/
/* advertisedDevice: Device received to analyse
 * idx: Index of the beacon to store the data
 */
void getRSSIBeacon(BLEAdvertisedDevice advertisedDevice, uint8_t idx) {
  
  if (advertisedDevice.haveRSSI()) {
    buffer[idx].rssi = advertisedDevice.getRSSI();
  }
  else {
    buffer[idx].rssi =  0;
  }
}
