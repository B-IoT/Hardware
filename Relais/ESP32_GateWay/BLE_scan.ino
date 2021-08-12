// We collect each device caracteristics and store them in BeaconData
typedef struct {
  char id[25]; //Device Name
  uint8_t address[MAC_ADDRESS_LENGTH]; // 67:f1:d2:04:cd:5d (mac address)
  int rssi;
  int txPower;
  uint8_t batteryLevel = 80; // Beacon Battery
  int8_t temperature = 22;  // Beacon Temperature
  uint8_t state = 0; // State of the beacon move
  
} BeaconData;

uint8_t bufferIndex = 0;  // Found devices counter
BeaconData buffer[50];    // Buffer to store found devices data
uint8_t whiteList[WHITELIST_LENGTH][MAC_ADDRESS_LENGTH]; // White list for the MAC


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  public:
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      extern uint8_t bufferIndex;
      extern BeaconData buffer[];

      if (bufferIndex >= 50) {
        return;
      }

      // First check that MAC Address is in the whitelist
      uint8_t receivedMac[MAC_ADDRESS_LENGTH];
      memcpy(receivedMac, advertisedDevice.getAddress().getNative(), MAC_ADDRESS_LENGTH);

      uint8_t presentInWhiteList = 0;
      nb_detected = 0;

      for (int j = 0; j < WHITELIST_LENGTH ; j++) {
          uint8_t eq = 1;
          for (int k = 0; k < MAC_ADDRESS_LENGTH; k++) {
            if(whiteList[j][k] != receivedMac[k]) {
              eq = 0;
              break;
            }
          }
          if(eq) {
            presentInWhiteList = 1;
            break;
          }
      }
      
      if(presentInWhiteList){
        //Name
        if (advertisedDevice.haveName()) {
          strcpy (buffer[bufferIndex].id, advertisedDevice.getName().c_str());
        }
        //RSSI
        if (advertisedDevice.haveRSSI()) {
          buffer[bufferIndex].rssi = advertisedDevice.getRSSI();
        } else {
          buffer[bufferIndex].rssi =  0;
        }
        //MAC Adresse
        memcpy(buffer[bufferIndex].address, receivedMac, MAC_ADDRESS_LENGTH);
  
        //TX Power
        if (advertisedDevice.haveTXPower()) {
          buffer[bufferIndex].txPower = advertisedDevice.getTXPower();
        }
  
        //Debug Print
        /*Serial.printf("name: %s \n", advertisedDevice.getName().c_str());
          Serial.printf("MAC: %s \n", advertisedDevice.getAddress().toString().c_str());
          Serial.printf("Manufactuerer Data: %d \n", advertisedDevice.getManufacturerData());
          Serial.printf("RSSI: %d \n", advertisedDevice.getRSSI());
          Serial.printf("TX Power: %d \n", advertisedDevice.getTXPower());*/
        bufferIndex++;
      }
      
    }
};

void ScanBeacons() {

  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  MyAdvertisedDeviceCallbacks cb; //define callback
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(beaconScanTime);
  BLEDevice::getScan()->stop(); // Stop BLE


  whiteList[0][0] = 0xd1;
  whiteList[0][1] = 0x0b;
  whiteList[0][2] = 0x14;
  whiteList[0][3] = 0xb3;
  whiteList[0][4] = 0x18;
  whiteList[0][5] = 0x6a;
  
  whiteList[1][0] = 0xfc;
  whiteList[1][1] = 0x02;
  whiteList[1][2] = 0xa0;
  whiteList[1][3] = 0xfa;
  whiteList[1][4] = 0x33;
  whiteList[1][5] = 0x19;
  
  whiteList[2][0] = 0xe3;
  whiteList[2][1] = 0x6f;
  whiteList[2][2] = 0x28;
  whiteList[2][3] = 0x36;
  whiteList[2][4] = 0x5a;
  whiteList[2][5] = 0xdb;
  
  whiteList[3][0] = 0xf1;
  whiteList[3][1] = 0x96;
  whiteList[3][2] = 0xcd;
  whiteList[3][3] = 0xee;
  whiteList[3][4] = 0x25;
  whiteList[3][5] = 0xbd;

  //checking whitelist
  nb_detected = bufferIndex;

  //Prints to show in Serial
  Serial.print("\n\n");
  //printLocalTime();
  Serial.print("B-IoT devices found: ");
  Serial.println(nb_detected);
  for (int i = 0; i < nb_detected; i++) {

    Serial.print("Name: ");
    Serial.println(buffer[i].id);
    Serial.print("Mac: ");
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x", buffer[i].address[0], buffer[i].address[1], buffer[i].address[2], buffer[i].address[3], buffer[i].address[4], buffer[i].address[5]);
    Serial.print("RSSI: ");
    Serial.println(buffer[i].rssi);
    Serial.print("TX Power: ");
    Serial.println(buffer[i].txPower);
     Serial.print("Batterie: ");
    Serial.println(buffer[i].batteryLevel);
    Serial.print("Temperature: ");
    Serial.println(buffer[i].temperature);
    Serial.print("state: ");
    Serial.println(buffer[i].state);
    Serial.println("---------------------");
  }
  bufferIndex = 0;
  delay(500);
  Serial.println("\nScan done!\n");
  delay(500);
}
