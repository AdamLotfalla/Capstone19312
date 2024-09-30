#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <SD.h>
#include <Update.h>

#define WIFI_SSID "OPPO Reno8 T 5G"
#define WIFI_PASSWORD "12348765"

#define led 23

void setup(){

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  Serial.begin(74880);
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

}

void loop(){
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(led, HIGH);
  }
  else{
    digitalWrite(led, LOW);
  }
  delay(3000);
}