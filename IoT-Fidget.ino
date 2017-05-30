#define APSSID "Your AP name" // your network SSID (name) 
#define APPASS "Your AP password" // your network password
#define CHANNELID 0 // your thingspeak channel ID
#define WRITEAPIKEY "YOURAPIKEY" // your thingspeak write API key

#define DATAMININTERVAL 15 // 15 seconds

#include "ThingSpeak.h"

// ***********************************************************************************************************
// This example selects the correct library to use based on the board selected under the Tools menu in the IDE.
// Yun, Ethernet shield, WiFi101 shield, esp8266, and MXR1000 are all supported.
// With Yun, the default is that you're using the Ethernet connection.
// If you're using a wi-fi 101 or ethernet shield (http://www.arduino.cc/en/Main/ArduinoWiFiShield), uncomment the corresponding line below
// ***********************************************************************************************************

//#define USE_WIFI101_SHIELD
//#define USE_ETHERNET_SHIELD


#if !defined(USE_WIFI101_SHIELD) && !defined(USE_ETHERNET_SHIELD) && !defined(ARDUINO_SAMD_MKR1000) && !defined(ARDUINO_AVR_YUN) && !defined(ARDUINO_ARCH_ESP8266)
  #error "Uncomment the #define for either USE_WIFI101_SHIELD or USE_ETHERNET_SHIELD"
#endif

#if defined(ARDUINO_AVR_YUN)
    #include "YunClient.h"
    YunClient client;
#else
  #if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_ARCH_ESP8266)
    // Use WiFi
    #ifdef ARDUINO_ARCH_ESP8266
      #include <ESP8266WiFi.h>
    #else
      #include <SPI.h>
      #include <WiFi101.h>
    #endif
    char ssid[] = APSSID;    //  your network SSID (name) 
    char pass[] = APPASS;   // your network password
    int status = WL_IDLE_STATUS;
    WiFiClient  client;
  #elif defined(USE_ETHERNET_SHIELD)
    // Use wired ethernet shield
    #include <SPI.h>
    #include <Ethernet.h>
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    EthernetClient client;
  #endif
#endif

unsigned long counter_begin_time;
int switch1_counter = 0;
int switch2a_counter = 0;
int switch2b_counter = 0;
int btngrp3_counter = 0;
int btngrp4_counter = 0;
int btngrp5_counter = 0;
int switch1_status = 1;
int switch2a_status = 1;
int switch2b_status = 1;
int btngrp3_status = 1;
int btngrp4_status = 1;
int btngrp5_status = 1;

void setup() {
  Serial.begin(115200);

  Serial.println("\n\nInitiating...");
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);

  #ifdef ARDUINO_AVR_YUN
    Bridge.begin();
  #else   
    #if defined(ARDUINO_ARCH_ESP8266) || defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000)
      WiFi.begin(ssid, pass);
    #else
      Ethernet.begin(mac);
    #endif
  #endif

  ThingSpeak.begin(client);
  cleanup_counter();
}

void loop() {
  read_switch_and_button();
  int interval = (millis() - counter_begin_time) / 1000;
  if (
    ((interval >= DATAMININTERVAL) || (interval < 0))
    && WiFi.enableAP(true)
  ) {
    ThingSpeak.setField(1, switch1_counter);
    ThingSpeak.setField(2, switch2a_counter);
    ThingSpeak.setField(3, switch2b_counter);
    ThingSpeak.setField(4, btngrp3_counter);
    ThingSpeak.setField(5, btngrp4_counter);
    ThingSpeak.setField(6, btngrp5_counter);
    ThingSpeak.setField(7, interval);
    if (ThingSpeak.writeFields(CHANNELID, WRITEAPIKEY) == OK_SUCCESS) {
      Serial.println("Write success.");
      cleanup_counter();
      WiFi.enableAP(false);
    } else {
      Serial.println("Write failed.");
    }
  }
}

void cleanup_counter() {
  counter_begin_time = millis();
  switch1_counter = 0;
  switch2a_counter = 0;
  switch2b_counter = 0;
  btngrp3_counter = 0;
  btngrp4_counter = 0;
  btngrp5_counter = 0;
}

void read_switch_and_button() {
  int tmp = digitalRead(D7);
  if (tmp != switch1_status) {
    switch1_counter++;
    switch1_status = tmp;
  }
  tmp = digitalRead(D6);
  if (tmp != switch2a_status) {
    switch2a_counter++;
    switch2a_status = tmp;
  }
  tmp = digitalRead(D5);
  if (tmp != switch2b_status) {
    switch2b_counter++;
    switch2b_status = tmp;
  }
  tmp = digitalRead(D3);
  if (tmp != btngrp3_status) {
    btngrp3_counter++;
    btngrp3_status = tmp;
  }
  tmp = digitalRead(D2);
  if (tmp != btngrp4_status) {
    btngrp4_counter++;
    btngrp4_status = tmp;
  }
  tmp = digitalRead(D1);
  if (tmp != btngrp5_status) {
    btngrp5_counter++;
    btngrp5_status = tmp;
  }
}

