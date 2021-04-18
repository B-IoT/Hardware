// We collect each device caracteristics and store them in BeaconData

typedef struct {
  //char id[25] ; //Device Name
  char address[18];   // 67:f1:d2:04:cd:5d
  int rssi;
  int txPower;
  uint8_t batteryLevel;
  int8_t temperature;
  uint8_t number;
  String packetFlag =" ";
  uint8_t state;
} BeaconData;

enum states{ MOVE, BTN, MOVE_AND_BTN };

uint8_t bufferIndex = 0;  // Found devices counter
BeaconData buffer[100];    // Buffer to store found device data

//Kontakt identifier for parsing payload Data [hexadecimal number]
char * biotName = "42494f54"; //BIOT in heaxadecimal 
// To improve dev
//char * payloadKontaktID = "6afe02"; 
//char * payloadKontaktTLMID = "6afe030902";
//char * payloadKontaktButtonID = "6afe03030d";
//char * payloadKontaktLocationID = "6afe05";


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  public:
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      extern uint8_t bufferIndex;
      extern BeaconData buffer[];

      if (bufferIndex >= 100) {
        return;
      }
      //Packet acquisition
      uint8_t *payLoad = advertisedDevice.getPayload(); //Get the entire packet 
      char *payLoadHex = BLEUtils::buildHexData(nullptr, advertisedDevice.getPayload(), advertisedDevice.getPayloadLength()); //Convert the packet in hexadecimal string

      //Biot Beacon check
      char* biotFound = strstr(payLoadHex,biotName); 
      
      if(biotFound !=NULL)
      {
 
//          Serial.println("je suis beacon biot");
//          Serial.println(biotFound);

          // Beacon number for dev checking
          buffer[bufferIndex].number = bufferIndex;
          
          //MAC Adress
          strcpy (buffer[bufferIndex].address, advertisedDevice.getAddress().toString().c_str());
//         Serial.print("mac adress = ");
//         Serial.println(buffer[bufferIndex].address);

          //RSSI
          if (advertisedDevice.haveRSSI()) {
            buffer[bufferIndex].rssi = advertisedDevice.getRSSI();
          } else {
            buffer[bufferIndex].rssi =  0;
          }
    
          // Parse payload to find information about beacon
          if(payLoad[5] ==0x6A && payLoad[6]==0xFE && payLoad[7] == 0x02)
          {
            //Serial.println("Kontakt packet");
            buffer[bufferIndex].packetFlag ="KONTAKT";
            buffer[bufferIndex].batteryLevel = payLoad[11];
            
            buffer[bufferIndex].txPower = payLoad[12]; //To convert correctly, in dbm
            
          }
          else if (payLoad[5] ==0x6A && payLoad[6]==0xFE && payLoad[7] ==0x03 && payLoad[8] == 0x09 && payLoad[9]==0x02)
          {
            //Serial.println("TLM KOntakt packet grouped field");
            buffer[bufferIndex].packetFlag ="TLM";
            buffer[bufferIndex].batteryLevel = payLoad[24];
            buffer[bufferIndex].temperature = payLoad[28];
    
          }
          else if (payLoad[5] ==0x6A && payLoad[6]==0xFE && payLoad[7] ==0x03 && payLoad[8] == 0x03 && payLoad[9]==0x0D)
          {
            //Serial.println("Button TLM packet");
            buffer[bufferIndex].packetFlag="BUTTON";
            int timeSinceLastBtnClick = payLoad[10];
            //Sanity check for dev
//            if(timeSinceLastBtnClick == 0xff)
//            {
//              Serial.println("Saturation, click to refresh");
//            }
          }
       bufferIndex++; 
      }
         // Packet to know if the beacon move (no BIOT name inside)
//      else if (payLoad[5] ==0x6A && payLoad[6]==0xFE && payLoad[7] ==0x05)
//      {
//          //Beacon number 
//          buffer[bufferIndex].number = bufferIndex;
          //MAC Adresse
//          strcpy (buffer[bufferIndex].address, advertisedDevice.getAddress().toString().c_str());
//          
//          //RSSI
//          if (advertisedDevice.haveRSSI()) {
//            buffer[bufferIndex].rssi = advertisedDevice.getRSSI();
//          } else {
//            buffer[bufferIndex].rssi =  0;
//          }
//          buffer[bufferIndex].packetFlag="MOVE";
//        //Serial.println("Movement packet");
//        uint8_t movementFlag = payLoad[8];
//        bufferIndex++
//      }

//------------------------- Old version uncomment to use it---------------------------//
//if (bufferIndex >= 50) {
//        return;
//      }
//
//      //Name
//      if (advertisedDevice.haveName()) {
//        strcpy (buffer[bufferIndex].id, advertisedDevice.getName().c_str());
//      }
//      //RSSI
//      if (advertisedDevice.haveRSSI()) {
//        buffer[bufferIndex].rssi = advertisedDevice.getRSSI();
//      } else {
//        buffer[bufferIndex].rssi =  0;
//      }
//      //MAC Adresse
//      strcpy (buffer[bufferIndex].address, advertisedDevice.getAddress().toString().c_str());
//      
//      //TX Power
//      if (advertisedDevice.haveTXPower()) {
//        buffer[bufferIndex].txPower = advertisedDevice.getTXPower();
//      }
//
//      /*Serial.printf("name: %s \n", advertisedDevice.getName().c_str());
//        Serial.printf("MAC: %s \n", advertisedDevice.getAddress().toString().c_str());
//        Serial.printf("Manufactuerer Data: %d \n", advertisedDevice.getManufacturerData());
//        Serial.printf("RSSI: %d \n", advertisedDevice.getRSSI());
//        Serial.printf("TX Power: %d \n", advertisedDevice.getTXPower());*/
//      bufferIndex++;
//    }

      
    }  
};

void ScanBeacons() {
  bufferIndex = 0; // Initialization number of biot beacon
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  MyAdvertisedDeviceCallbacks cb; //define callback
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(beaconScanTime);
  BLEDevice::getScan()->stop(); // Stop BLE



//------------------- OLD version -----------------------------------//

  //Deprecated but kept for dev
//  char whiteList[2][18] = // 10 is the length of the longest string + 1 ( for the '\0' at the end ) DEPRECATED
//  {
//    "cf:18:14:5d:8a:04",
//    "e2:51:e0:31:ee:0e",
//    "cf:ae:ce:64:a0:f6",
//    "d1:0b:14:b3:18:6a",
//    "fc:02:a0:fa:33:19",
//    "e3:6f:28:36:5a:db",
//    "f1:96:cd:ee:25:bd",
//  };

  //checking whitelist
//  nb_detected = 0;
//  for (uint8_t i = 0; i <= bufferIndex; i++) {
//    for (uint8_t j = 0; j <= sizeof(whiteList) / 18 ; j++) {
//      if (strcmp(buffer[i].address, whiteList[j]) == 0) {
//        strcpy(buffer[nb_detected].id, buffer[i].id);
//        strcpy(buffer[nb_detected].address, buffer[i].address);
//        buffer[nb_detected].rssi = buffer[i].rssi;
//        buffer[nb_detected].txPower = buffer[i].txPower;
//        nb_detected++;
//      }
//    }
//  }
//----------------------------------------//


nb_detected = bufferIndex; //For the new version otherwise comment
  //Prints to show in Serial
  Serial.print("\n\n");
  printLocalTime();
  Serial.print("B-IoT devices found: ");
  Serial.println(nb_detected);
  for (uint8_t i = 0; i < nb_detected; i++) {
    Serial.print("Number: ");
    Serial.println(buffer[i].number);
    Serial.print("Mac: ");
    Serial.println(buffer[i].address);
    Serial.print("RSSI: ");
    Serial.println(buffer[i].rssi);
    Serial.print("Packet flag: ");
    Serial.println(buffer[i].packetFlag);
    Serial.print("TX Power: ");
    Serial.println(buffer[i].txPower);
    Serial.print("Battery Level: ");
    Serial.print(buffer[i].batteryLevel);
    Serial.println("%");
    Serial.print("Temperature: ");
    Serial.print(buffer[i].temperature);
    Serial.println("°C");
    Serial.println("---------------------");
  }
  //bufferIndex = 0; //Old version
  delay(500);
  Serial.println("\nScan done!\n");
  delay(500);
}
