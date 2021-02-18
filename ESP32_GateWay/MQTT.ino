//Json dans lequel on va stocker toutes les données reçues
DynamicJsonDocument doc(2048);

void connect_MQTT() {

  Serial.print("Checking MQTT ...");
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect(nodeId)) {
      // Subscribe
      client.subscribe("update.params");
      client.subscribe("last.configuration");
      client.subscribe("incoming.update");
    } else {
      Serial.print(" errorCode= ");
      Serial.print(client.state());
      // Wait 5 seconds before retrying
      delay(2500);
    }
  }
  Serial.println(" Connected!");
}


void callback(char* topic, byte* message, unsigned int length) {

  Serial.println("\n---------------\n");
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print(". Message: ");
  
  char messageTemp[200];
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp[i] = (char)message[i];
  }
  
  DeserializationError error = deserializeJson(doc, messageTemp);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  ssid_MQTT = doc["wifi"]["ssid"];
  password_MQTT = doc["wifi"]["password"];
  const char* mac = doc["beacon"]["mac"]; //deprecated
  long txPower = doc["beacon"]["txPower"]; //deprecated
  bool ledStatus = doc["ledStatus"];

  /*Serial.println(ssid_MQTT);
  Serial.println(password_MQTT);
  Serial.println(mac);
  Serial.println(txPower);
  Serial.println(ledStatus);
  Serial.println("\n--------\n");*/
}

void send_MQTT() {

  StaticJsonDocument<200> doc;
  doc["mac"] = nodeId;

  JsonArray mac = doc.createNestedArray("mac");
  for (uint8_t i = 0; i < nb_detected; i++) {
    mac.add(buffer[i].address);
  }

  JsonArray rssi = doc.createNestedArray("rssi");
  for (uint8_t i = 0; i < nb_detected; i++) {
    rssi.add(buffer[i].rssi);
  }
  Serial.print("Created json: ");
  serializeJson(doc, Serial);
  Serial.println("\n---------------------");

}
