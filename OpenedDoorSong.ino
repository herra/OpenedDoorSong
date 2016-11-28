#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <StreamString.h>
#include <ArduinoJson.h>

int doorSensorPin = 2;
int prevDoorState = -1;
ADC_MODE(ADC_VCC);
ESP8266WiFiMulti wiFiMulti;

bool reportWifiConnected = true, sendAnyway = true;
String sessionId, serverDateTime, chipId;
uint sleepTime;
HTTPClient http;

int buzzerPin = 0;
int songSwap = 1;

struct Song {
  int tempo;
  int length;
  char* notes;
  int* duration;    
};
Song jingleBells, silentNight;

char notesName[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'h', 'C', 'D', 'E', 'F' };
int tones[] = { 523, 587, 659, 698, 783, 880, 987, 1046, 1174, 1318, 1396 };

extern "C" {
  #include "gpio.h"
}

extern "C" {
  #include "user_interface.h"
}

void reportConnectionToWifi();
void login();
void sendSensorData(int currentDoorState);
void wakeUp();
void playSong(Song* song);


void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(doorSensorPin, INPUT_PULLUP);
  Serial.begin(74880);  
  WiFi.mode(WIFI_OFF); //Default mode is AP_STA which creates Access Point and connects to wifi.
  
  wiFiMulti.addAP("SSID", "SSID_PASS");
  wiFiMulti.addAP("SSID2", "SSID_PASS");
  chipId = String("Esp01-" + String(ESP.getChipId()));
  Serial.print("Device ChipId: ");  Serial.print(chipId); Serial.println("");
  Serial.print("PowerSupply: "); Serial.print(ESP.getVcc()); Serial.println("");

  String notes;
    
  notes = "eeeeeeegcdefffffeeeeeddedg";
  int jingleBellsDurations [] = {2, 2, 4, 2, 2, 4, 2, 2, 3, 1, 8, 2, 2, 3, 1, 2, 2, 2, 1, 1, 2, 2, 2, 2, 4, 4};

  jingleBells.length = notes.length() + 1;
  jingleBells.notes = (char*)malloc(jingleBells.length);
  jingleBells.duration = (int*)malloc(jingleBells.length * sizeof(int));
  jingleBells.tempo = 75;
  
  notes.toCharArray(jingleBells.notes, jingleBells.length); 
  for (int i = 0; i < jingleBells.length; i++) {
    jingleBells.duration[i] = jingleBellsDurations[i];
  }
  
  //
  notes = "gagegageDDhCCgaaChagageaaChagageDDFDhCECgegfdc";
  int silentNightDurations [] = {3,1,2,6,  3,1,2,6,  4,2,6,  4,2,6,  4,2,3,1,2,   3,1,2,6,  4,2,3,1,2,   3,1,2,6,   4,2,3,1,2,   6,6,  2,2,2, 3,1,2, 12 };

  silentNight.length = notes.length() + 1;
  silentNight.notes = (char*)malloc(silentNight.length);
  silentNight.duration = (int*)malloc(silentNight.length * sizeof(int));
  silentNight.tempo = 118;
  
  
  notes.toCharArray(silentNight.notes, silentNight.length);
  for (int i = 0; i < silentNight.length; i++) {
    silentNight.duration[i] = silentNightDurations[i];
  }

  gpio_init(); // Initilize GPIO pins
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);  
}

void loop() {  
  Serial.println('Starting');
  delay(10000);
  Serial.println('Starting');
  int currentDoorState = digitalRead(doorSensorPin);
    
//  if (sessionId == NULL || sessionId == "") {
//    wakeUp();
//    login();
//    delay(10000);
//  }
//  if (prevDoorState < 0 || currentDoorState != prevDoorState || sendAnyway) {
//    sendAnyway = false;
//    sendSensorData(currentDoorState);    
//    delay(500);
//  }
  
  //if (currentDoorState == 1) {
      //sleepTime = millis();    

      
      if (songSwap == 1) {
        playSong(&silentNight);       
        songSwap = 0;
      } else {
        playSong(&jingleBells);
        songSwap = 1;
      }      
  //} 

  prevDoorState = currentDoorState;
  
//  if (millis() - sleepTime >= 9000 && currentDoorState == 0) {
//    Serial.println("Going to sleep");
//    delay(100);
//    wifi_station_disconnect();
//    wifi_set_opmode(NULL_MODE);
//    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
//    wifi_fpm_open();    
//    gpio_pin_wakeup_enable(GPIO_ID_PIN(2), GPIO_PIN_INTR_HILEVEL);
//    wifi_fpm_do_sleep(0xFFFFFFF);
//    delay(200);    
//  }
  Serial.println("Running");
  delay(500);
//  
//  if (wiFiMulti.run() != WL_CONNECTED) {
//    sendAnyway = true;
//    //wakeUp();
//    Serial.println("WOKE UP");
//  } 
  Serial.println("WOKE UP");
}

void playSong(Song* song) {  
  Serial.println("Playing song");
  
  for (int j = 0; j < song->length - 1; j++) {
    Serial.println(j);
    int toneDuration = song->duration[j] * song->tempo;
    char note = song->notes[j];
    Serial.println("Note: " + String(note) + "  duration: " + String(toneDuration));
    for (int i = 0; i < sizeof(tones); i++) {    
      if (note == notesName[i]) {      
        tone(buzzerPin, tones[i], toneDuration);
      }
    }
    
    delay(toneDuration * 2);
  }
}

void wakeUp() {
  delay(200);
  WiFi.mode(WIFI_STA);
  delay(200);
  Serial.println("");
  Serial.print("Connecting Wifi");
  while(wiFiMulti.run() != WL_CONNECTED) {
    reportConnectionToWifi();
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
}

void reportConnectionToWifi() {
  if(wiFiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected: " + WiFi.SSID());
      Serial.print("IP address: ");     Serial.print(WiFi.localIP());
      Serial.println("");
      reportWifiConnected = false; 
  }
}

void sendSensorData(int currentDoorState) {
  StaticJsonBuffer<600> jsonBuffer;
  JsonObject& sensorData = jsonBuffer.createObject();
  sensorData["sessionId"] = sessionId;
  JsonArray& data = sensorData.createNestedArray("sensorsData");
  
  JsonObject& valueDataItem = jsonBuffer.createObject();
  valueDataItem["sensorIdentifier"] = (String(chipId + "-Door-Sensor"));
  valueDataItem["deviceIdentifier"] = chipId;
  valueDataItem["error"] = "";  
  valueDataItem["value"] = currentDoorState;
  data.add(valueDataItem);

  JsonObject& psDataItem = jsonBuffer.createObject();
  psDataItem["sensorIdentifier"] = (String(chipId + "-Power-Supply"));
  psDataItem["deviceIdentifier"] = chipId;
  psDataItem["error"] = "";  
  psDataItem["value"] = ESP.getVcc();
  data.add(psDataItem);  
  
  String requestData;
  sensorData.printTo(requestData);  
  Serial.println(requestData);
  
  http.begin("http://www.dinz.eu/homeautomation/ReceiveData");
  http.addHeader("Content-Type", "application/json");  
  int sendDataResponseCode = http.POST(requestData);
  if (sendDataResponseCode == 200) {
    String returnvalue = http.getString();    
    StaticJsonBuffer<100> jsonResponseBuffer;
    JsonObject& sendDataResponse = jsonResponseBuffer.parseObject(returnvalue);   
    if (sendDataResponse.success())
    {
      Serial.println("Received data: " + returnvalue);
    }
  } else {
    Serial.print("Could not send data: "); Serial.print(sendDataResponseCode); Serial.println("");
  }
  
  http.end();
}

void login() {      
  http.begin("http://www.dinz.eu/homeautomation/login");
  http.addHeader("Content-Type", "application/json");  
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["username"] = "demo";
  root["password"] = "demo123";
  
  String loginRequestData, returnvalue = "";
  root.printTo(loginRequestData);  
  
  if (http.POST(loginRequestData) == 200) {
    returnvalue = http.getString();    
    StaticJsonBuffer<2000> jsonResponseBuffer;
    JsonObject& loginResponse = jsonResponseBuffer.parseObject(returnvalue);   
    const char* sessionId_ = loginResponse["sessionId"];    
    const char* serverDateTime_ = loginResponse["loginDateTime"];
    sessionId = String(sessionId_);
    serverDateTime = String(serverDateTime_);  
    Serial.println("Current time: " + serverDateTime);
    Serial.println("SessionId: " + sessionId);
    Serial.println("Received: " + returnvalue);
  } else {
    Serial.println("Could not login");
  }
  
  http.end();
}


