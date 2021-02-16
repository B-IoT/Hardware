//Json dans lequel on va stocker toutes les données reçues
DynamicJsonDocument doc(2048);

void connect_MQTT() {

  Serial.print("Checking MQTT ...");
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect(nodeId)) {
      // Subscribe to channel
      client.subscribe("update.params");
      client.subscribe("last.configuration");
      client.subscribe("incoming.update");
    } else {
      //if cannot connect
      Serial.print(" errorCode= ");
      Serial.print(client.state());
      // Wait 5 seconds before retrying
      delay(2500);
    }
  }
  Serial.println(" Connected!");
}


void callback(char* topic, byte* message, unsigned int length) {

  //Prints for dev
  Serial.println("\n---------------\n");
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print(". Message: ");

  //Char in which the received message is stored
  char messageTemp[200];

  //Converting the receiveid message to char in Jsan format 
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp[i] = (char)message[i];
  }

  //Deserialize the received Json
  DeserializationError error = deserializeJson(doc, messageTemp);
  
  //Printing error
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  //Updating the parameters
  ssid_MQTT = doc["wifi"]["ssid"];
  password_MQTT = doc["wifi"]["password"];
  const char* mac = doc["beacon"]["mac"]; //deprecated
  long txPower = doc["beacon"]["txPower"]; //deprecated
  bool ledStatus = doc["ledStatus"];
  digitalWrite (ledPin, ledStatus); //puts the led in the chosen status by MQTT

  //Serial for debug
  /*Serial.println(ssid_MQTT);
  Serial.println(password_MQTT);
  Serial.println(mac);
  Serial.println(txPower);
  Serial.println(ledStatus);
  Serial.println("\n--------\n");*/
}

void send_MQTT() {

  //Creating the Json document to send
  StaticJsonDocument<200> doc;                                                                                           //TO INCREASE
  doc["mac"] = nodeId; //title of the Json head is the NodeId

  //First array is the mac of all devices Json/nodeId/mac
  JsonArray mac = doc.createNestedArray("mac");
  for (uint8_t i = 0; i < nb_detected; i++) {
    mac.add(buffer[i].address); 
  }

  //Second array is the RSSI of all devices Json/nodeId/rssi
  JsonArray rssi = doc.createNestedArray("rssi"); 
  for (uint8_t i = 0; i < nb_detected; i++) {
    rssi.add(buffer[i].rssi);
  }

  //Print in Serial for debug
  Serial.print("Created json: ");
  serializeJson(doc, Serial);
  Serial.println("\n---------------------");

}
