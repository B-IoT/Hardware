void connect_MQTT() {

  //Creating the Json document to send
  DynamicJsonDocument doc(500);
  doc["company"] = "biot";

  //Sending the Json
  char buffer[400];
  serializeJson(doc, buffer);

  Serial.print("Checking MQTT with company id: ");
  Serial.print(buffer);

  client.setBufferSize(MQTT_BUFFER_SIZE_RECEIVE); // in char 6*2 * 1024 + 2*1024 (whitelist + 2*1024 for the rest to be sure)

  while (!client.connected()) {
    //if wifi deconnects after the first check
    if(WiFi.status() != WL_CONNECTED) {
          Serial.println("Wifi exit ...");
          break;
        }

    // Attempt to connect
    Serial.print("Connecting to ");
    Serial.println(mqttServer);
    ledBlueOn();

    delay(500);
    if (client.connect(relayID, mqttUser, mqttPassword, "will", 1, false, buffer)) {
      // Subscribe to channel
      Serial.print("update.parameters state : ");
      Serial.println(client.subscribe("update.parameters"));
     Serial.println(" Connected! - Subscribed to each channels");
      ledBlueOn();
    } else {
      //if cannot connect
      Serial.print(" errorCode= ");
      Serial.println(client.state());
      // Wait 5 seconds before retrying
      ledTurquoiseOn();
      delay(500);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {

  DynamicJsonDocument doc(500);

  //Prints for dev
  Serial.println("\n---------------\n");
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print(". Message: ");


  //Converting the receiveid message to char in Json format
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    mqttMessageTemp[i] = (char)message[i];
  }
  Serial.println("");
  
  //Deserialize the received Json
  DeserializationError error = deserializeJson(doc, mqttMessageTemp);

  //Printing error
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //Updating the parameters
  mqttFloor = doc["floor"];
  mqttLatitude = doc["latitude"];
  mqttLongitude = doc["longitude"];
  mqttSSID = strdup(doc["wifi"]["ssid"]);
  mqttPasswordWIFI = strdup(doc["wifi"]["password"]); 
  char* whiteListString = strdup(doc["whiteList"]);
  

  // DEBUG 
  Serial.print("received whiteList String : ");
  Serial.println(whiteListString);

  updateWhiteList(whiteListString);
  
}

void send_MQTT() {
  
  //Parse the JSON condition 
  /*NOTE: JSON is sending one by one because of a problem occuring from the reception on the backend */
  /*Code in comments allows to send 3 by 3 */
  /*if (nb_detected > maxBeaconToSend) {
    uint8_t startIdx = 0; // Index of the first beacon to send
    uint8_t nbDocToCreate;*/
    uint8_t startIdx = 0;
    uint8_t nbDocToCreate = nb_detected;
    // Compute number of doc to create to send all beacons
    /*if ((nb_detected % maxBeaconToSend) == 0) {
      nbDocToCreate = nb_detected / maxBeaconToSend; //Number of document required to send all beacons
    }
    else {
      nbDocToCreate = nb_detected / maxBeaconToSend + 1; //Number of document required to send all beacons
    }*/
    
    for (uint8_t j = 0; j < nbDocToCreate; j++) {
      uint8_t endIdx = startIdx + maxBeaconToSend; //Index of the last beacon to send 
      
      //Creating the Json document to send
      DynamicJsonDocument doc(600);  //TO INCREASE
      doc["relayID"] = relayID; //Title of the Json head is the relayID

      //Sanity condition to avoid blank data
      if (endIdx > nb_detected) {
        endIdx = nb_detected; 
      }
      
      //Serial print for dev
      /*Serial.printf("startIdx = %d \n",startIdx);
      Serial.printf("endix = %d \n",endIdx);*/
      
      //Fill and send the JSON
      send_JSON(doc, startIdx, endIdx);
      startIdx += maxBeaconToSend; 
    }
  /*} else {
 
    //Creating the Json document to send
    DynamicJsonDocument doc(600);//TO INCREASE
    doc["relayID"] = relayID; //title of the Json head is the relayID
    //Fill and send the JSON
    send_JSON(doc, 0, nb_detected);
  }*/
}

//Goal: Fill the doc and send the JSON doc to the mqtt
//Parameters: doc: the Json document to fill
//            idxStart: index to start the filling
//            idxEnd: index to stop the filling
void send_JSON(DynamicJsonDocument doc, uint8_t idxStart, uint8_t idxEnd) {
    
    
  JsonArray beacons = doc.createNestedArray("beacons"); // Beacon array to send
  
  // Write Beacon data to send
  for (uint8_t i = idxStart; i < idxEnd; i++) {
    DynamicJsonDocument beaconDoc(100); // Json size for on beacon = 78 (measured)

    char macAddressString[18];
    snprintf(macAddressString, MAC_ADDRESS_STRING_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x", buffer[i].address[0], buffer[i].address[1], buffer[i].address[2], buffer[i].address[3], buffer[i].address[4], buffer[i].address[5]);
    beaconDoc["mac"] = macAddressString;
    beaconDoc["rssi"] = buffer[i].rssi;
    beaconDoc["battery"] = buffer[i].batteryLevel;
    beaconDoc["temperature"] = buffer[i].temperature;
    beaconDoc["status"] = buffer[i].state;
    beacons.add(beaconDoc);
  }
    doc["latitude"] = serialized(String(mqttLatitude, 6)); //latitude of the Json
    doc["longitude"] = serialized(String(mqttLongitude, 6)); //longitude of the Json
    doc["floor"] = mqttFloor;
  
    //Sending the Json
    char buffer[600];
    //Serial.println("Bonjour serial");
    serializeJson(doc, buffer);
    //JSON size for dev
    /*size_t jsonSize =  measureJson(doc);//measure number of caracters inside the json serialized
    Serial.print("JSON SIZE = ");
    Serial.println(jsonSize);*/
    
    client.publish("incoming.update", buffer);
    Serial.println(buffer);
    Serial.println("Sent Json on incoming.update");
    Serial.println("\n---------------------");
}

/**
  * Update the whiteList (which is a global var) given the pointer to the whiteListString
  * The string must have the following format: "aabbccddeeff112233445566..."
  * If the String is malformed, the code will not fail but the behavior is not guaranteed 
  * i.e., the Whitelist could contain not recognized mac addresses.
  * The function also fill the non-used white list slots with 0's
  *
  **/
int updateWhiteList(char* whiteListString) {
    uint8_t validMac = 1;
    for(int i = 0; i < WHITELIST_LENGTH; i++) {
        uint8_t outMac[MAC_ADDRESS_LENGTH];
        if(whiteListString[i*MAC_ADDRESS_LENGTH*2] == '\0'){
            // End of the string, go out of the loop
            break;
        }
        for(int j = 0; j < MAC_ADDRESS_LENGTH; j++){
            char c1 = whiteListString[i*MAC_ADDRESS_LENGTH*2 + j*2];
            char c2 = whiteListString[i*MAC_ADDRESS_LENGTH*2 + j*2 + 1];
            if(c1 == '\0' || c2 == '\0'){
                Serial.printf("Update whiteList ERROR: whiteListString truncated\n");
                validMac = 0;
                break;
            }
          outMac[j] = hexCharToUint8t(c1) * 16 + hexCharToUint8t(c2);
        }
        
        if(!validMac) {
            break;
        }
        for(int k = 0; k < MAC_ADDRESS_LENGTH; k++){
            whiteList[i][k] = outMac[k];
        }
        whiteListCount = whiteListCount + 1;
        
       
    }

    // Put 0's in all next white listed MAC addresses
    for(int i = whiteListCount; i < WHITELIST_LENGTH; i++) {
        for(int k = 0; k < MAC_ADDRESS_LENGTH; k++){
            whiteList[i][k] = 0;
        }
    }
    // for(int i = 0; i < WHITELIST_LENGTH; i++) {
    //     printf("%02x:%02x:%02x:%02x:%02x:%02x\n", whiteList[i][0], whiteList[i][1], whiteList[i][2], whiteList[i][3], whiteList[i][4], whiteList[i][5]);
    // }
    return 0;
}

uint8_t hexCharToUint8t(char c) {
    if(c == '0'){
        return 0;
    }
    else if(c == '1'){
        return 1;
    }
    else if(c == '2'){
        return 2;
    }
    else if(c == '3'){
        return 3;
    }
    else if(c == '4'){
        return 4;
    }
    else if(c == '5'){
        return 5;
    }
    else if(c == '6'){
        return 6;
    }
    else if(c == '7'){
        return 7;
    }
    else if(c == '8'){
        return 8;
    }
    else if(c == '9'){
        return 9;
    }
    else if(c == 'a'){
        return 10;
    }
    else if(c == 'b'){
        return 11;
    }
    else if(c == 'c'){
        return 12;
    }
    else if(c == 'd'){
        return 13;
    }
    else if(c == 'e'){
        return 14;
    }
    else if(c == 'f'){
        return 15;
    } else {
        printf("Update whiteList ERROR: invalid character in hex\n");
        return 0;
    }

}
