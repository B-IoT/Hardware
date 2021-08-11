// Beacon states 
enum states {MOVE, BTN, MOVE_AND_BTN, NOTHING};

// Collect all device characteristics and store in BeaconData
typedef struct BeaconData {
  char address[18];  // mac address (67:f1:d2:04:cd:5d)
  int rssi;
  int txPower;
  uint8_t batteryLevel = 0; // Beacon Battery
  int8_t temperature = 0;  // Beacon Temperature
  uint8_t number = 0; // Indice of the beacon found
  uint8_t timeSinceLastClick = 255; // Number of seconds since the last click on button (Saturation value)
  uint8_t timeSinceLastMove = 255; // Number of seconds since the last movement (Saturation value)
  uint8_t state = NOTHING; // State of the beacon
} BeaconData;

bool beaconFound = false; // New Biot beacon to store
BeaconData buffer[50];    // Buffer to store found device data
BeaconData tabToSend[15]; // Table to store beacons to send

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
      extern BeaconData buffer[];

      if (nb_detected >= 50) {
        return;
      }

      // Bool initialization
      beaconFound = false;

      // Packet acquisition
      uint8_t* payLoad = advertisedDevice.getPayload(); // Get the entire packet
      char* payLoadHex = BLEUtils::buildHexData(nullptr, advertisedDevice.getPayload(), advertisedDevice.getPayloadLength()); // Convert the packet in hexadecimal string
      
      // Biot Beacon check
      char* biotFound = strstr(payLoadHex, biotName);

      // Filter: Biot packet 
      if (biotFound != NULL) {

        // Copy the actual mac adress to analyze it (for better reading)
        char macAdressReceived[18] ;
        strcpy(macAdressReceived, advertisedDevice.getAddress().toString().c_str());

        // First beacon found
        if (nb_detected == 0) {
          strcpy (buffer[nb_detected].address, macAdressReceived);
          buffer[nb_detected].number = nb_detected;
          parsePayload(payLoad, nb_detected); //Parse correctly the data
          nb_detected++;
        }
        
        // Other beacons to store
        else if (nb_detected != 0) {
          //Check if the beacon already exist         
          for (uint8_t idx = 0; idx < nb_detected; idx++) {
            if (strcmp(buffer[idx].address, macAdressReceived) == 0) { // if the beacon already exists           
              beaconFound = true;
              parsePayload(payLoad, idx); // Parse correctly the data
              break;
            }
          }

          if (beaconFound == false) {
            strcpy (buffer[nb_detected].address, macAdressReceived); // Store a new Biot beacon
            buffer[nb_detected].number = nb_detected;
            parsePayload(payLoad, nb_detected); // Parse correctly the data
            nb_detected++;
          }

          // Get RSSI
          getRSSIBeacon(advertisedDevice, nb_detected); 
          
        }
      }
    }
};

void ScanBeacons() {
  
//Time measurement for dev
//  unsigned long Temps_start_us, Temps_stop_us, Duree_us;
//  Temps_start_us = millis();

  beaconArray = 0;
 
  // Start scan beacon 
  BLEScan* pBLEScan = BLEDevice::getScan(); // Create new scan
  MyAdvertisedDeviceCallbacks cb; // Define callback
  pBLEScan->setAdvertisedDeviceCallbacks(&cb);
  
  // Collect the most packet per beacon 
  for (uint8_t i = 0; i < packetScanTime; i++){
    pBLEScan->setActiveScan(true); // Active scan uses more power, but get results faster
    BLEScanResults foundDevices = pBLEScan->start(beaconScanTime);
  }
  
  BLEDevice::getScan()->stop(); // Stop BLE

  // Status check and assignment 
  for (uint8_t i = 0; i < nb_detected; i++) {  
    if (buffer[i].timeSinceLastMove <= 15 && buffer[i].timeSinceLastClick <= 20) {
      buffer[i].state = MOVE_AND_BTN;
    }
    else if (buffer[i].timeSinceLastMove <= 15) {
      buffer[i].state = MOVE;
    }
    else if ( buffer[i].timeSinceLastClick <= 20) {
      buffer[i].state = BTN;
    }
    else{
      buffer[i].state = NOTHING;
    }
  }
  
  // Stock data of beacon to send and check empty mac adress
  for (uint8_t i = 0; i < nb_detected; i++) {
      tabToSend[beaconArray].number = buffer[i].number;
      strcpy(tabToSend[beaconArray].address, buffer[i].address);
      tabToSend[beaconArray].rssi = buffer[i].rssi;
      tabToSend[beaconArray].txPower = buffer[i].txPower;
      tabToSend[beaconArray].state = buffer[i].state;
      tabToSend[beaconArray].batteryLevel = buffer[i].batteryLevel;
      tabToSend[beaconArray].temperature = buffer[i].temperature;
      tabToSend[beaconArray].timeSinceLastClick = buffer[i].timeSinceLastClick;
      tabToSend[beaconArray].timeSinceLastMove = buffer[i].timeSinceLastMove;
      beaconArray ++;
    
  }
  
// Time measurement for dev 
//  Temps_stop_us = millis();
//  Duree_us = Temps_stop_us - Temps_start_us;
//  Serial.print("Duree scan beacon = ");
//  Serial.println(Duree_us);
  

  for (uint8_t w = 0; w < beaconArray; w++)
  {
    Serial.print("address;");
    Serial.println(tabToSend[w].address) ;
    Serial.print("rssi;");
    Serial.println(tabToSend[w].rssi);
  }

  delay(500);

}
