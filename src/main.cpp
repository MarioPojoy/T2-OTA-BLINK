#include <Arduino.h>
#include <WiFi.h>
#if defined(ARDUINO_ESP32_DEV)    
  #include <ESPmDNS.h>
  #include <WiFiUdp.h>
  const char* hostname = "esp32";
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  const char* hostname = "picow";
#else
  #error BOARD NOT DEFINED
#endif
#include <ArduinoOTA.h>
#include "credentials.h"

#define ledPin LED_BUILTIN
#define interval 250

int ledState = LOW;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    #if defined(ARDUINO_ESP32_DEV)
      ESP.restart();
    #elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
      rp2040.restart();
    #else
      #error  BOARD NOT DEFINED
    #endif
  }

  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  ArduinoOTA.setHostname(hostname);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {
        type = "filesystem";
      }
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(ledPin, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }
}