void connect_wifi() {

  Serial.print("\nChecking WiFi..");
  
  while (WiFi.status() != WL_CONNECTED) { //Check WiFi connection status
    
    Serial.print(".");
    WiFi.begin(ssid_MQTT, password_MQTT);
    digitalWrite (ledPin, true);
    delay(500); // wait 500ms
    
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid_hard, password_hard);
      digitalWrite (ledPin, false);
      delay(500); // wait 500ms
    }
    
  }
  digitalWrite (ledPin, ledStatus);
  Serial.println(" Connected !");
}
