void connect_wifi() {


  Serial.println("Enable Wifi STATION mode");
  WiFi.enableSTA(true);
  WiFi.disconnect();
  delay(500);
  Serial.print("\nChecking WiFi..");
  
  while (WiFi.status() != WL_CONNECTED) { //Keeps trying to connect until it's ok

    int n = WiFi.scanComplete();
      if (n == -2) {
        Serial.println("->enter to scanning function");
        WiFi.scanNetworks (true, false);   //async, show_hidden
        Serial.println("->out of scanning function");
        
      }
      else if (n == -1) {
        delay(100);
      }
      /*else if (n > 0) {
        for (int i = 0; i <= n; i++) {
          Serial.print(i);
          Serial.print(")");
          Serial.println(WiFi.SSID(i));
        }
        WiFi.scanDelete();
        Serial.println("->scanning deleted");
      }*/

    
  
    Serial.print(".");
    WiFi.begin(hardSSID, hardPassword);
     //tries to connect with the MQTT received credentials
    ledTurquoiseOn(); //Led turquoise
    delay(500); // wait 500ms

    //if still not connected
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(mqttSSID, mqttPasswordWIFI); //tries to connect with the default credentials
      ledGreenOn(); // 
      delay(500); // wait 500ms
    }
    
  }
  Serial.println(" Connected !");
  ledGreenOn();
  
}
