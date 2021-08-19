void ledBlueOn() {
  ledcWrite(ledChannelBlue, intensiteOn); //Led OFF
  ledcWrite(ledChannelGreen, intensiteOff); //Led ON
  ledcWrite(ledChannelRed, intensiteOff); //Led ON
}

void ledGreenOn() {
  ledcWrite(ledChannelBlue, intensiteOff); //Led OFF
  ledcWrite(ledChannelGreen, intensiteOn); //Led ON
  ledcWrite(ledChannelRed, intensiteOff); //Led ON
}

void ledTurquoiseOn() {
  ledcWrite(ledChannelBlue, intensiteOn); //Led OFF
  ledcWrite(ledChannelGreen, intensiteOn); //Led ON
  ledcWrite(ledChannelRed, intensiteOff); //Led ON
}

void ledRedOn() {
  ledcWrite(ledChannelBlue, intensiteOff); //Led OFF
  ledcWrite(ledChannelGreen, intensiteOff); //Led ON
  ledcWrite(ledChannelRed, intensiteOn); //Led ON
}
void ledOff() {
  ledcWrite(ledChannelBlue, intensiteOff); //Led OFF
  ledcWrite(ledChannelGreen, intensiteOff); //Led ON
  ledcWrite(ledChannelRed, intensiteOff); //Led ON
}
