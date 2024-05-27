
void setupVoiceControl(void)
{
  setupRelays();
  Serial.println("relay");
  oledPrint("Setup Relay");
  
  setupFlipSwitches();
  Serial.println("flip");
  oledAppend(1,"Setup Flip Switches");
  
  setupSinricPro();
  Serial.println("sinricpro");
  oledAppend(2, "Setup SinricPro");  
  delay(2000);
}

void setupRelays() { 
  for (auto &device : devices) {           // for each device (relay, flipSwitch combination)
    int relayPIN = device.second.relayPIN; // get the relay pin
    pinMode(relayPIN, INPUT);             // initially set the relays off
    digitalWrite(relayPIN, HIGH); 
  }
}

void setupFlipSwitches() {
  for (auto &device : devices)  {                     // for each device (relay / flipSwitch combination)
    flipSwitchConfig_t flipSwitchConfig;              // create a new flipSwitch configuration

    flipSwitchConfig.deviceId = device.first;         // set the deviceId
    flipSwitchConfig.lastFlipSwitchChange = 0;        // set debounce time
    flipSwitchConfig.lastFlipSwitchState = true;     // set lastFlipSwitchState to false (LOW)--

    int flipSwitchPIN = device.second.flipSwitchPIN;  // get the flipSwitchPIN

    flipSwitches[flipSwitchPIN] = flipSwitchConfig;   // save the flipSwitch config to flipSwitches map
    pinMode(flipSwitchPIN, INPUT_PULLUP);                   // set the flipSwitch pin to INPUT
  }
}

bool onPowerState(String deviceId, bool &state)
{
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "on" : "off");
  int relayPIN = devices[deviceId].relayPIN; // get the relay pin for corresponding device
  pinMode(relayPIN, state?OUTPUT:INPUT);
  digitalWrite(relayPIN, !state);             // set the new relay state
//    digitalWrite(relayPIN, state);             // now pin will go high when on;
  //update for pir
  if(deviceId == device_ID_2){
    PIR_changed_light1 = false;
  }
  if(deviceId == device_ID_3){
    PIR_changed_light2 = false;
  }
  if(deviceId == device_ID_4){
    PIR_changed_fan = false;
  }
  
  return true;
}

void handleFlipSwitches() {
  unsigned long actualMillis = millis();                                          // get actual millis
  for (auto &flipSwitch : flipSwitches) {                                         // for each flipSwitch in flipSwitches map
    unsigned long lastFlipSwitchChange = flipSwitch.second.lastFlipSwitchChange;  // get the timestamp when flipSwitch was pressed last time (used to debounce / limit events)

    if (actualMillis - lastFlipSwitchChange > DEBOUNCE_TIME) {                    // if time is > debounce time...

      int flipSwitchPIN = flipSwitch.first;                                       // get the flipSwitch pin from configuration
      bool lastFlipSwitchState = flipSwitch.second.lastFlipSwitchState;           // get the lastFlipSwitchState
      bool flipSwitchState = digitalRead(flipSwitchPIN);                          // read the current flipSwitch state
      if (flipSwitchState != lastFlipSwitchState) {                               // if the flipSwitchState has changed...
#ifdef TACTILE_BUTTON
        if (flipSwitchState) {                                                    // if the tactile button is pressed 
#endif      
          flipSwitch.second.lastFlipSwitchChange = actualMillis;                  // update lastFlipSwitchChange time
          String deviceId = flipSwitch.second.deviceId;                           // get the deviceId from config
          int relayPIN = devices[deviceId].relayPIN;                              // get the relayPIN from config
          pinMode(relayPIN, OUTPUT);
          bool newRelayState = !digitalRead(relayPIN);                            // set the new relay State
//          digitalWrite(relayPIN, newRelayState);  // set the trelay to the new state
          pinMode(relayPIN, newRelayState?INPUT:OUTPUT);
          digitalWrite(relayPIN, newRelayState);             // set the new relay state
          ////titumir
          Serial.println("Flipped!"+String(relayPIN)+"         State:"+String(newRelayState));
          SinricProSwitch &mySwitch = SinricPro[deviceId];                        // get Switch device from SinricPro
          mySwitch.sendPowerStateEvent(!newRelayState);                            // send the event

          //update PIR variables
            if(deviceId == device_ID_2){
              PIR_changed_light1 = false;
            }
            if(deviceId == device_ID_3){
              PIR_changed_light2 = false;
            }
            if(deviceId == device_ID_4){
              PIR_changed_fan = false;
            }
            
#ifdef TACTILE_BUTTON
        }
#endif      
        flipSwitch.second.lastFlipSwitchState = flipSwitchState;                  // update lastFlipSwitchState
      }
    }
  }
}

void setupWiFi(byte attemt)
{
  Serial.printf("\r\n[Wifi]: Connecting to SSID-%d", int(attemt));
  oledPrint("Connecting to WiFi");
  oledAppend(1,"SSID: ");
  oledAppend(2, String(WIFI_SSID[attemt]));
  WiFi.begin(WIFI_SSID[attemt], WIFI_PASS[attemt]);
  unsigned long t = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf(".");
    delay(250);
    t += 250;
    if(t > 5e3) setupWiFi((attemt+1) % NO_OF_SSID);
  }
  digitalWrite(wifiLed, 1);
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
  oledPrint("Connected to WiFi");
  oledAppend(1,"IP-Address: ");
  oledAppend(2, String(WiFi.localIP().toString().c_str()));
  delay(2000);
}

void setupSinricPro()
{
  for (auto &device : devices)
  {
    const char *deviceId = device.first.c_str();
    SinricProSwitch &mySwitch = SinricPro[deviceId];
    mySwitch.onPowerState(onPowerState);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true);
}
