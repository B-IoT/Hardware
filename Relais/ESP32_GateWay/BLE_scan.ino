// We collect each device caracteristics and store them in BeaconData

typedef struct {
  char id[25] ; //Device Name
  char address[18];   // 67:f1:d2:04:cd:5d
  uint8_t batteryLevel;
  int8_t temperature;
  int rssi;
  int txPower;
} BeaconData;

uint8_t bufferIndex = 0;  // Found devices counter
BeaconData buffer[50];    // Buffer to store found device data

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  public:
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      extern uint8_t bufferIndex;
      extern BeaconData buffer[];

      if (bufferIndex >= 50) {
        return;
      }
        uint8_t *payLoad = advertisedDevice.getPayload(); //Get the entire packet (hexdecimal)
         
      //Name
//      if (advertisedDevice.haveName()) {
//        strcpy (buffer[bufferIndex].id, advertisedDevice.getName().c_str());
//      }
//      //MAC Adresse
//      strcpy (buffer[bufferIndex].address, advertisedDevice.getAddress().toString().c_str());
//      
//      //RSSI
//      if (advertisedDevice.haveRSSI()) {
//        buffer[bufferIndex].rssi = advertisedDevice.getRSSI();
//      } else {
//        buffer[bufferIndex].rssi =  0;
//      }
//      
//
//      //TX Power
//      if (advertisedDevice.haveTXPower()) {
//        buffer[bufferIndex].txPower = advertisedDevice.getTXPower();
//      }
//      //For dev beacons
//      if(advertisedDevice.haveServiceUUID())
//      {
//        buffer[bufferIndex].beaconServiceUUID = advertisedDevice.getServiceUUID();
//        buffer[bufferIndex].nbServiceDataUUID = advertisedDevice.getServiceUUIDCount();
//        //buffer[bufferIndex].serviceDataUUID = advertisedDevice.getServiceDataUUID();
//        //buffer[bufferIndex].serviceDataUUIDIndexe = advertisedDevice.getServiceUUID(0x180F);
//        
//      }
//      if(advertisedDevice.haveManufacturerData() == true)
//      {
//        buffer[bufferIndex].strManufacturerData = advertisedDevice.getManufacturerData();
////    buffer[bufferIndex].nbServiceData = advertisedDevice.getServiceDataCount();
////      buffer[bufferIndex].serviceData = advertisedDevice.getServiceData();
//      //buffer[bufferIndex].serviceDataIndexe = advertisedDevice.getServiceData(0);
//
//      }
//      
//      //Debug Print
//      /*Serial.printf("name: %s \n", advertisedDevice.getName().c_str());
//        Serial.printf("MAC: %s \n", advertisedDevice.getAddress().toString().c_str());
//        Serial.printf("Manufactuerer Data: %d \n", advertisedDevice.getManufacturerData());
//        Serial.printf("RSSI: %d \n", advertisedDevice.getRSSI());
//        Serial.printf("TX Power: %d \n", advertisedDevice.getTXPower());*/
      bufferIndex++;
    }
};

void ScanBeacons() {

  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  MyAdvertisedDeviceCallbacks cb; //define callback
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(beaconScanTime);
  BLEDevice::getScan()->stop(); // Stop BLE


  //Deprecated but kept for dev
  char whiteList[2][18] = // 10 is the length of the longest string + 1 ( for the '\0' at the end ) DEPRECATED
  {
    "cf:18:14:5d:8a:04",
    "e2:51:e0:31:ee:0e",
//    "cf:ae:ce:64:a0:f6",
//    "d1:0b:14:b3:18:6a",
//    "fc:02:a0:fa:33:19",
//    "e3:6f:28:36:5a:db",
//    "f1:96:cd:ee:25:bd",
  };

  //checking whitelist
  nb_detected = 0;
  for (uint8_t i = 0; i <= bufferIndex; i++) {
    for (uint8_t j = 0; j <= sizeof(whiteList) / 18 ; j++) {
      if (strcmp(buffer[i].address, whiteList[j]) == 0) {
        strcpy(buffer[nb_detected].id, buffer[i].id);
        strcpy(buffer[nb_detected].address, buffer[i].address);
        buffer[nb_detected].rssi = buffer[i].rssi;
        buffer[nb_detected].txPower = buffer[i].txPower;
        buffer[nb_detected].beaconServiceUUID = buffer[i].beaconServiceUUID;
        buffer[nb_detected].strManufacturerData = buffer[i].strManufacturerData;
        buffer[nb_detected].nbServiceDataUUID = buffer[i].nbServiceDataUUID;
        buffer[nb_detected].serviceDataUUID = buffer[i].serviceDataUUID;
        buffer[nb_detected].serviceDataUUIDIndexe = buffer[i].serviceDataUUIDIndexe;
        nb_detected++;
      }
    }
  }

  //Prints to show in Serial
  Serial.print("\n\n");
  printLocalTime();
  Serial.print("B-IoT devices found: ");
  Serial.println(nb_detected);
  for (uint8_t i = 0; i < nb_detected; i++) {

    Serial.print("Name: ");
    Serial.println(buffer[i].id);
    Serial.print("Mac: ");
    Serial.println(buffer[i].address);
    Serial.print("RSSI: ");
    Serial.println(buffer[i].rssi);
    Serial.print("TX Power: ");
    Serial.println(buffer[i].txPower);
    //For dev beacons
    Serial.print("Service UUID: ");
    Serial.println(buffer[i].beaconServiceUUID.toString().c_str());
    
    std::string manufacturedData = buffer[i].strManufacturerData;
    uint8_t cManufacturerData[100];
    manufacturedData.copy((char *)cManufacturerData, manufacturedData.length(), 0);
    Serial.printf("strManufacturerData: %d ", manufacturedData.length());
    for (int j = 0; j < manufacturedData.length(); j++)
    {
      Serial.printf("%c",cManufacturerData[j]);
    }
    Serial.printf("\n");
    Serial.print("NB Service Data UUID: ");
    Serial.println(buffer[i].nbServiceDataUUID);
    Serial.print("Service DATA UUID: ");
    Serial.println(buffer[i].serviceDataUUID.toString().c_str());
    Serial.print("Service Data Indexe: ");
    Serial.println(buffer[i].serviceDataUUIDIndexe.toString().c_str());

    //Don't work 
//    Serial.print("NB Service Data: ");
//    Serial.println(buffer[i].nbServiceData);
//    Serial.print("Service DATA: ");
//    Serial.println(buffer[i].serviceData.c_str());
//    Serial.print("Service Data 0: ");
//    Serial.println(buffer[i].serviceDataIndexe.c_str());
    Serial.println("---------------------");
  }
  bufferIndex = 0;
  delay(500);
  Serial.println("\nScan done!\n");
  delay(500);
}
