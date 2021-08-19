#include "MQTT_lib.cpp"

void init_MQTT() {
  Serial.println("Enter initt_MQTT()");
  
  //Creating the Json document to send
  DynamicJsonDocument willDoc(250);
  willDoc["company"] = "biot";

  char buffer[400];
  size_t lastWillBufferSize = serializeJson(willDoc, buffer);
  Serial.print("lastWill message: ");
  Serial.println(buffer);
  Serial.print("lastWill message length: ");
  Serial.println((int)lastWillBufferSize);

  mqtt_init_client(relayID, mqttServerUri, mqttUser, mqttPassword, buffer, (int)lastWillBufferSize, &callback);
}

void callback(const char* message, const int msgLength, const char* topic, const int topicLength) {
  Serial.println("Bonjour");
}

void send_MQTT() {
  
}
