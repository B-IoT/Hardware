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

      uint8_t* payLoad = advertisedDevice.getPayload(); // Get the entire packet
      memcpy(receivedMac, advertisedDevice.getAddress().getNative(), MAC_ADDRESS_LENGTH);

      uint8_t presentInWhiteList = 0;
      nb_detected = 0;

      for (int j = 0; j < whiteListCount ; j++) {
        uint8_t eq = 1;
        for (int k = 0; k < MAC_ADDRESS_LENGTH; k++) {
          if (whiteList[j][k] != receivedMac[k]) {
            eq = 0;
            break;
          }
        }
        if (eq) {
          presentInWhiteList = 1;
          break;
        }
      }

      if (presentInWhiteList) {
        // Check that we did not receive it already
        uint8_t received = 0;
        for(int i = 0; i < bufferIndex; i++) {
          uint8_t eq = 1;
          for (int k = 0; k < MAC_ADDRESS_LENGTH; k++) {
            if (buffer[i].address[k] != receivedMac[k]) {
              eq = 0;
              break;
            }
          }
          if (eq) {
            received = 1;
            break;
          }
        }

        if(!received){
          parsePayload(payLoad, bufferIndex);
  
          //RSSI
          if (advertisedDevice.haveRSSI()) {
            buffer[bufferIndex].rssi = advertisedDevice.getRSSI();
          } else {
            buffer[bufferIndex].rssi =  0;
          }
  
          //MAC Adresse
          memcpy(buffer[bufferIndex].address, receivedMac, MAC_ADDRESS_LENGTH);
  
          /*if (advertisedDevice.haveName()) {
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

    }
};

void ScanBeacons() {

  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  MyAdvertisedDeviceCallbacks cb; //define callback
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(beaconScanTime);
  BLEDevice::getScan()->stop(); // Stop BLE

  //checking whitelist
  nb_detected = bufferIndex;

  //Prints to show in Serial
  Serial.print("\n\n");
  //printLocalTime();
  Serial.print("B-IoT devices found: ");
  Serial.println(nb_detected);
  for (int i = 0; i < nb_detected; i++) {

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

  Serial.println("\nScan done!\n");

  bufferIndex = 0;
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(500);
}
