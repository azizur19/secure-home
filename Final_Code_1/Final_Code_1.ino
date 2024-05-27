//////// Code for voice control uning Google assistant and Sinric Pro
// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
//#include <ESP8266WiFi.h>
#include<WiFi.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

#include <map>

char *WIFI_SSID[]     =    {"Celestial Wave", "Titumir"};    
char *WIFI_PASS[]     =    {"wave1234!",      "pl2weeks"};
byte NO_OF_SSID       =     2;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Voice control Sinric Pro 
#define APP_KEY           "22c9b171-0012-4605-aed8-b6d756a10632"      // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET        "c382b94a-0230-42d3-8070-a6a1a757bf05-6f729751-cbe9-49dc-8496-4afd74f18958"   // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"

//Enter the device IDs here
#define device_ID_1   "xxxxxxxxxxxxxxxxxxxxxxxx"
#define device_ID_2   "xxxxxxxxxxxxxxxxxxxxxxxx"
#define device_ID_3   "xxxxxxxxxxxxxxxxxxxxxxxx"
#define device_ID_4   "xxxxxxxxxxxxxxxxxxxxxxxx"

// define the GPIO connected with Relays and switches
#define RelayPin1 34  //unused 
#define RelayPin2 18   //light 1  
#define RelayPin3 5  //light 2
#define RelayPin4 19  //fan 

#define SwitchPin1 35  //unused
#define SwitchPin2 15  //light 1 
#define SwitchPin3 4   //light 2
#define SwitchPin4 23  //fan


typedef struct {      // struct for the std::map below
  int relayPIN;
  int flipSwitchPIN;
} deviceConfig_t;


// this is the main configuration
// please put in your deviceId, the PIN for Relay and PIN for flipSwitch
// this can be up to N devices...depending on how much pin's available on your device ;)
// right now we have 4 devicesIds going to 4 relays and 4 flip switches to switch the relay manually
std::map<String, deviceConfig_t> devices = {
    //{deviceId, {relayPIN,  flipSwitchPIN}}
    {device_ID_1, {  RelayPin1, SwitchPin1 }},
    {device_ID_2, {  RelayPin2, SwitchPin2 }},
    {device_ID_3, {  RelayPin3, SwitchPin3 }},
    {device_ID_4, {  RelayPin4, SwitchPin4 }}     
};

typedef struct {      // struct for the std::map below
  String deviceId;
  bool lastFlipSwitchState;
  unsigned long lastFlipSwitchChange;
} flipSwitchConfig_t;

std::map<int, flipSwitchConfig_t> flipSwitches;    // this map is used to map flipSwitch PINs to deviceId and handling debounce and last flipSwitch state checks
                                                  // it will be setup in "setupFlipSwitches" function, using informations from devices map

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// INPUT and OUTPUT PINS
#define wifiLed   2    //build in led

#define controlSwitch1  13  
#define controlSwitch2  12  
#define controlSwitch3  14  
#define upKey         SwitchPin3  //controlSwitch1
#define menuKey       SwitchPin2  //controlSwitch2
#define downKey       SwitchPin4  //controlSwitch3

#define solinoidalLock  27
// setting PWM properties
const int freq = 1000;
const int ledChannel = 0;
const int resolution = 8;

#define pirSensor       26
#define gasSensor       25
#define buzzer          33

#define PIR_surr_1      34
#define EMERGENCY_pin      32

#define VOLTAGE_SEN_PIN 36 //edit_please
#define CURRENT_SEN_PIN 39


#define LCD_PIN         I2C_PIN

// comment the following line if you use a toggle switches instead of tactile buttons
#define TACTILE_BUTTON 1
#define BAUD_RATE      115200
#define DEBOUNCE_TIME  250




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Code for fingure print sensor
#include <Adafruit_Fingerprint.h>
#define RXD2 16
#define TXD2 17

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
uint8_t id;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////WhatsApp message alert!
#include <HTTPClient.h>   //Including HTTPClient.h library to use all api
//String apiKey = "9855452";              //Add your Token number that bot has sent you on WhatsApp messenger
//String phone_number = "+8801571206300"; //Add your WhatsApp app registered phone number (same number that bot send you in url)
String my_apiKey = "3807964";              //Add your Token number that bot has sent you on WhatsApp messenger
String my_phone_number = "+88017xxxxxxxx"; //Add your WhatsApp app registered phone number (same number that bot send you in url)
String apiKey = "9855452";              //Add your Token number that bot has sent you on WhatsApp messenger
String phone_number = "+88017xxxxxxxx"; //Add your WhatsApp app registered phone number (same number that bot send you in url)

String url;                            //url String will be used to store the final generated URL

String gsm_phone_num = "+88017xxxxxxxx";



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////LCD I2C display
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // I2C address 0x27, 20 column and 4 rows

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////OLED display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G oled = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TEXT_SIZE 1    // max 8
#define LINE_GAP  10  //no pixel vertically


unsigned long tLastButtonHandle, lastTimeHandle = millis(); 
unsigned long lastDoorOpened = millis();
unsigned long lastPIROnTime = millis();
unsigned long lastBuzzerOn = millis();
long          last_gas_sms_sent = -1e3;
long          last_pir_surr_sms_sent = -1e3;
unsigned long lastPowerDisplayed = millis();

boolean PIR_changed_light1 = false;
boolean PIR_changed_light2 = false;
boolean PIR_changed_fan    = false;

boolean lastPIRState = LOW;
int     no_gas_sms_sent = 0;
int     no_pir_surr_sms_sent = 0;

float voltage_rms = 223.745;
float current_rms = 0.0703;



void setup()
{
  Serial.begin(115200);
  setupGSM(); //set serial baud rate to 9600
  Serial.println("serial ok");
  setup_oled();
  power_setup();

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(solinoidalLock, ledChannel);
  
  pinMode(pirSensor, INPUT);
  pinMode(gasSensor, INPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(PIR_surr_1, INPUT);
  pinMode(EMERGENCY_pin, INPUT);

  pinMode(downKey, INPUT_PULLUP);
  pinMode(menuKey, INPUT_PULLUP);
  pinMode(upKey, INPUT_PULLUP);

  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, 0);
  setupWiFi(1 % NO_OF_SSID);
  Serial.println("wifi");

  setupVoiceControl();
  setupFingure();
  oledUpdate();
  
}

long t_ = micros();
boolean emergency_sent = false;
long last_emergency = millis();

void loop()
{
  SinricPro.handle();
  handleFlipSwitches();

  gasPIRDoor();

  if((long)(millis()-lastTimeHandle)  >  200){
    manageFingerPrint();
    //managePower();
  }
}

void manageFingerPrint(void)
{
  lastTimeHandle = millis();
  int x = checkValidFinger();
  if(x>0 && x<128){
    lastTimeHandle += 5e3;
    doorOpen();  //finger matched
    
    Serial.println("Door Opened for ID: "+String(x));
    oledPrint(0, "Door Opened ID: "+String(x));
    message_to_whatsapp("ID: "+String(x)+" Entered");
    oledAppend(1, "Add Finger (UP Key)");
    oledAppend(2, "Delete Finger (Down");
    oledAppend(3, "                Key)");
    tLastButtonHandle = millis(); //for waiting for key press
    handleButton();
    delay(1000);
    oledUpdate();
  }
  else if(x == 0){
    lastTimeHandle += 5e3;
    Serial.println("Someone trying to Enter!");//finger not matched. send alert text
    oledPrint(    "Finger not matched!");
    oledAppend(1, "You Are Not Allowed!");
    oledAppend(2, "Please Try Again If");
    oledAppend(3, "You Are Authorized.");
    finger.getTemplateCount();
    if(finger.templateCount < 1){
      insertNewModel(1);
    }
    else message_to_whatsapp("Someone trying to Enter!");//finger not matched. send alert text
    delay(1000);
    oledUpdate();
  }
//  Serial.println("Checked at: "+String(lastTimeHandle));
}

void handleButton(void)
{
  while(millis()-tLastButtonHandle < 5e3){
    if(digitalRead(upKey) == 0){
      tLastButtonHandle = millis();
      oledPrint("Press OK to Enroll");
      while(millis()-tLastButtonHandle < 5e3){
        if(digitalRead(menuKey) == 0){
          insertNewModel(1);
        }
      }
    }
    if(digitalRead(downKey) == 0){
      tLastButtonHandle = millis();
      oledPrint("Press OK to Delete");
      while(millis()-tLastButtonHandle < 5e3){
        if(digitalRead(menuKey) == 0){
          deleteModel();
        }
      }
    }
  }
}

void doorOpen(void)
{
 //open door 
// pinMode(solinoidalLock, OUTPUT);

 lastDoorOpened = millis();
 ledcWrite(ledChannel, 255);
 delay(1000);
 ledcWrite(ledChannel, 80);
}

void gasPIRDoor(void)
{
  //close door if time out
  if(millis()-lastDoorOpened > 10e3){
    ledcWrite(ledChannel, 0);
//    Serial.println("Door Closed");
  }
  //gas
  if(!digitalRead(gasSensor)){
    if(millis()-lastBuzzerOn > 1000){
      digitalWrite(buzzer, !digitalRead(buzzer));
      lastBuzzerOn = millis();
    }
    if((millis()-last_gas_sms_sent > 10e3) && (no_gas_sms_sent < 3)){
      //message to fire station
      //message to user
      message_to_whatsapp("Gas Detected! Message Sent to Fire Station");
      send_sms(gsm_phone_num, "Gas Detected! Please Help! Address: New Market 1205.");
      delay(2000);
      make_call_to(gsm_phone_num);
      //edit_please
      no_gas_sms_sent += 1;
      last_gas_sms_sent = millis();
    }
  }
  else digitalWrite(buzzer, LOW);
  //PIR
//  managePIR();
//  manageSurroundingPIR();
  emergency();
}

void emergency(void){
  if(!digitalRead(EMERGENCY_pin)) {digitalWrite(buzzer,0); emergency_sent = false; return;}
  
  if(millis()-lastBuzzerOn > 1000){
    digitalWrite(buzzer, !digitalRead(buzzer));
    lastBuzzerOn = millis();
  }
  
  if(millis()-last_emergency < 30e3) return;

  if(emergency_sent) return;
  
  message_to_whatsapp("Emergency ");
  make_call_to(gsm_phone_num);
  send_sms(gsm_phone_num, "Emergency");
  
  emergency_sent = true;
  last_emergency = millis();
}

void oledUpdate(void)
{
    oledPrint("IOT Based Smart Home");
    oledAppend(1, " Enter Finger Print");
    oledAppend(2, "Don't Try!");
    oledAppend(3,"   If Unauthorized!");
    oledAppend(4,String("V: ")+String(voltage_rms)+String("  C: ")+String(current_rms));
    oledAppend(5, String("    Power: ")+String(voltage_rms*current_rms*0.87));
}

void playBuzzer(long t)
{
  digitalWrite(buzzer, HIGH);
  delay(t);
  digitalWrite(buzzer, LOW);
}

void managePIR(void)
{
//  Serial.println("PIR State: "+String(digitalRead(pirSensor)));
  if(digitalRead(pirSensor) && lastPIRState==LOW){ 
    Serial.println("Turning ON");
    
    byte tem_relay_PIN = devices[device_ID_2].relayPIN;
    pinMode(tem_relay_PIN, OUTPUT); //Made as output. Does not need to make it INPUT as it is OUTPUT if not HIGH
    if(digitalRead(tem_relay_PIN) == HIGH){ // HIGH=off
      changeState(device_ID_2, LOW);  // light 1 // LOW=ON //to drive pnp bjt(of relay module). inverted
      PIR_changed_light1 = true;
      Serial.println("Room Light ON");
    }

    tem_relay_PIN = devices[device_ID_3].relayPIN;
    pinMode(tem_relay_PIN, OUTPUT);
    if(digitalRead(tem_relay_PIN) == HIGH){ // HIGH=off
      changeState( device_ID_3, LOW);  // light 2 // LOW=ON //to drive pnp bjt(of relay module). inverted
      PIR_changed_light2 = true;
      Serial.println("Kitchen ON");
    }

    tem_relay_PIN = devices[device_ID_4].relayPIN;
    pinMode(tem_relay_PIN, OUTPUT);
    if(digitalRead(tem_relay_PIN) == HIGH){ // HIGH=off
      changeState( device_ID_4, LOW);  // fan // LOW=ON //to drive pnp bjt(of relay module). inverted
      PIR_changed_fan = true;
      Serial.println("Fan ON");
    }
    
    lastPIRState = HIGH;
    lastPIROnTime = millis();
  }
  else if(!digitalRead(pirSensor) && lastPIRState==HIGH && (millis()-lastPIROnTime > 20e3)){
    Serial.println("Turning OFF");

    if(PIR_changed_light1){
      changeState( device_ID_2, HIGH);  // light 1// HIGH=OFF //to drive pnp bjt(of relay module). inverted
      PIR_changed_light1 = false;
      Serial.println("Room OFF");
    }
    if(PIR_changed_light2){
      changeState( device_ID_3, HIGH);  // light 2// HIGH=OFF //to drive pnp bjt(of relay module). inverted
      PIR_changed_light2 = false;
      Serial.println("Kitchen OFF");
    }
    if(PIR_changed_fan){
      changeState( device_ID_4, HIGH);  // fan// HIGH=OFF //to drive pnp bjt(of relay module). inverted
      PIR_changed_fan = false;
      Serial.println("Fan OFF");
    }

    lastPIRState = LOW;
  }
}

void manageSurroundingPIR(void)
{
//  //if man detected surronding the house
//  //message to user
//  //alert with alarm
//  if(analogRead(PIR_surr_1) > 1000){
//    if((millis()-last_pir_surr_sms_sent > 1000) && (no_pir_surr_sms_sent < 5)){
//      last_pir_surr_sms_sent = millis();
//      //message to user
//      message_to_whatsapp("Person Detected Surrounding the House!");
//      Serial.println("Person Detected Surrounding the House!");
//      //edit_please
//      no_pir_surr_sms_sent += 1;
//    }
//  }
////  else digitalWrite(buzzer, LOW);
//  //edit_please
}

void changeState(String deviceId, boolean state) //can change the state of a device and update to cloud
{  
  int relayPIN = devices[deviceId].relayPIN;                              // get the relayPIN from config
  pinMode(relayPIN, state?INPUT:OUTPUT);  // LOW == off. but to turn off make the pin HIGH(pnp bjt) or declare as INPUT
  digitalWrite(relayPIN, state);             // set the new relay state
  ////titumir
  SinricProSwitch &mySwitch = SinricPro[deviceId];                        // get Switch device from SinricPro
  mySwitch.sendPowerStateEvent(!state);                            // send the event
}

void managePower(void)
{
  voltage_rms = rms_voltage();
  current_rms = rms_current();
}
