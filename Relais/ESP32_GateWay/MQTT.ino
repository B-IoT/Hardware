void init_and_connect_MQTT() {

  //Creating the Json document to send
  DynamicJsonDocument willDoc(250);
  willDoc["company"] = company;

  //Sending the Json
  char lastWillBuffer[400];
  int lastWillLength = serializeJson(willDoc, lastWillBuffer);

  Serial.print("Checking MQTT with company id: ");
  Serial.println(lastWillBuffer);

  mqtt_init_client(relayID, mqttServerUri, mqttUser, mqttPassword, lastWillBuffer, lastWillLength);
}

void callbackConnected() {
  Serial.println("MQTT connected to the server, subscribing to the topic");
  mqtt_lib_subscribe("update.parameters");
}

void callbackIncomingMsg(char* topic, int topicLength, char* message, int messageLength) {

  DynamicJsonDocument receiveDocJson(MQTT_JSON_SIZE_RECEIVE);

  //Print incoming message
  for (int i = 0; i < messageLength; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println("");

  //Deserialize the received Json
  DeserializationError error = deserializeJson(receiveDocJson, message);

  //Printing error
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //Updating the parameters
  mqttFloor = receiveDocJson["floor"];
  mqttLatitude = receiveDocJson["latitude"];
  mqttLongitude = receiveDocJson["longitude"];
  mqttSSID = strdup(receiveDocJson["wifi"]["ssid"]);
  mqttPasswordWIFI = strdup(receiveDocJson["wifi"]["password"]);
  char* whiteListString = strdup(receiveDocJson["whiteList"]);


  // DEBUG
  Serial.print("received whiteList String : ");
  Serial.println(whiteListString);

  updateWhiteList(whiteListString);

}

void send_MQTT() {

  //Parse the JSON condition
  int startIdx = 0;
  uint8_t nbDocToCreate = nb_detected;
  // Compute number of doc to create to send all beacons
  for (int j = 0; j < nbDocToCreate; j++) {
    int endIdx = startIdx + maxBeaconToSendInOnePacket; //Index of the last beacon to send

    //Creating the Json document to send
    DynamicJsonDocument doc(1024);  //TO INCREASE
    doc["relayID"] = relayID; //Title of the Json head is the relayID

    //Sanity condition to avoid blank data
    if (endIdx > nb_detected) {
      endIdx = nb_detected;
    }

    //Serial print for dev

    //Fill and send the JSON
    send_JSON(doc, startIdx, endIdx);
    startIdx += maxBeaconToSendInOnePacket;
  }
}

//Goal: Fill the doc and send the JSON doc to the mqtt
//Parameters: doc: the Json document to fill
//            idxStart: index to start the filling
//            idxEnd: index to stop the filling
void send_JSON(DynamicJsonDocument doc, uint8_t idxStart, uint8_t idxEnd) {


  JsonArray beacons = doc.createNestedArray("beacons"); // Beacon array to send

  // Write Beacon data to send
  for (int i = idxStart; i < idxEnd; i++) {
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
  char sendJsonBuffer[600];
  //Serial.println("Bonjour serial");
  serializeJson(doc, sendJsonBuffer);
  //JSON size for dev
  /*size_t jsonSize =  measureJson(doc);//measure number of caracters inside the json serialized
    Serial.print("JSON SIZE = ");
    Serial.println(jsonSize);*/

  //client.publish("incoming.update", sendJsonBuffer);
  mqtt_lib_publish("incoming.update", sendJsonBuffer);
  Serial.println(sendJsonBuffer);
  Serial.println("Sent Json on incoming.update");
  Serial.println("\n---------------------");
}

/**
    Update the whiteList (which is a global var) given the pointer to the whiteListString
    The string must have the following format: "aabbccddeeff112233445566..."
    If the String is malformed, the code will not fail but the behavior is not guaranteed
    i.e., the Whitelist could contain not recognized mac addresses.
    The function also fill the non-used white list slots with 0's

  **/
int updateWhiteList(char* whiteListString) {
  whiteListCount = 0;
  uint8_t validMac = 1;
  uint8_t outMac[MAC_ADDRESS_LENGTH];
  for (int i = 0; i < WHITELIST_LENGTH; i++) {

    if (whiteListString[i * MAC_ADDRESS_LENGTH * 2] == '\0') {
      // End of the string, go out of the loop
      break;
    }
    for (int j = 0; j < MAC_ADDRESS_LENGTH; j++) {
      char c1 = whiteListString[i * MAC_ADDRESS_LENGTH * 2 + j * 2];
      char c2 = whiteListString[i * MAC_ADDRESS_LENGTH * 2 + j * 2 + 1];
      if (c1 == '\0' || c2 == '\0') {
        Serial.printf("Update whiteList ERROR: whiteListString truncated\n");
        validMac = 0;
        break;
      }
      outMac[j] = hexCharToUint8t(c1) * 16 + hexCharToUint8t(c2);
    }

    if (!validMac) {
      break;
    }
    for (int k = 0; k < MAC_ADDRESS_LENGTH; k++) {
      whiteList[i][k] = outMac[k];
    }
    whiteListCount = whiteListCount + 1;
  }

  // Put 0's in all next white listed MAC addresses
  for (int i = whiteListCount; i < WHITELIST_LENGTH; i++) {
    for (int k = 0; k < MAC_ADDRESS_LENGTH; k++) {
      whiteList[i][k] = 0;
    }
  }

  Serial.print("Updated whiteList: whiteListCount = ");
  Serial.printf("%d\n", whiteListCount);
  /*Serial.println("White list is:");
    for(int i = 0; i < WHITELIST_LENGTH; i++) {
       printf("%02x:%02x:%02x:%02x:%02x:%02x\n", whiteList[i][0], whiteList[i][1], whiteList[i][2], whiteList[i][3], whiteList[i][4], whiteList[i][5]);
    }*/
  return 0;
}

uint8_t hexCharToUint8t(char c) {
  if (c == '0') {
    return 0;
  }
  else if (c == '1') {
    return 1;
  }
  else if (c == '2') {
    return 2;
  }
  else if (c == '3') {
    return 3;
  }
  else if (c == '4') {
    return 4;
  }
  else if (c == '5') {
    return 5;
  }
  else if (c == '6') {
    return 6;
  }
  else if (c == '7') {
    return 7;
  }
  else if (c == '8') {
    return 8;
  }
  else if (c == '9') {
    return 9;
  }
  else if (c == 'a') {
    return 10;
  }
  else if (c == 'b') {
    return 11;
  }
  else if (c == 'c') {
    return 12;
  }
  else if (c == 'd') {
    return 13;
  }
  else if (c == 'e') {
    return 14;
  }
  else if (c == 'f') {
    return 15;
  } else {
    printf("Update whiteList ERROR: invalid character in hex\n");
    return 0;
  }

}
