//Function to fetch time - Used for proto dev but useless IRL

void GetTime() {
  //Set NTP paramaters
  const char* ntpServer = "pool.ntp.org";
  const long  gmtOffset_sec = 3600;
  const int   daylightOffset_sec = 3600;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}

void printLocalTime()
{
  //Prints the time in the terminal, main use is for IRL data collecting and prototyping
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  else {
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S\n");
  }
}
