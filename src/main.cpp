#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
// #include <Ethernet.h>
// #include <EthernetClient.h>

#define WIFI_SSID "Redmi Note 10S"
#define WIFI_PASSWORD "11111111"
//OPPO Reno8 T 5G, 12348765

#define led 23
//https://docs.google.com/spreadsheets/d/1S6_IL7yn1GuPK6xeMa3itCuH75X0g4UemVOapFCJXmc/edit?gid=0#gid=0
//1S6_IL7yn1GuPK6xeMa3itCuH75X0g4UemVOapFCJXmc

String URL = "https://script.google.com/macros/s/AKfycbyXmQYj_BsG6RlaBTjZySvpfMkLCk3HEpxNikECkL3xlvlD_A282rAniDI4T9mdBlRw/exec?sts=write";



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

int counter = 0;

void loop(){
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(led, HIGH);
  }
  else{
    digitalWrite(led, LOW);
  }
  counter ++;
  delay(3000);
  if(counter == 15){
    counter = 0;
    URL += "&temp=" + String(69);
    URL += "&humd=" + String(150);
    URL += "&npeople=" + String(1000);


    //::::::::::::::::::The process of sending or writing data to Google Sheets.
    // Initialize HTTPClient as "http".
    HTTPClient http;

    // HTTP GET Request.
    http.begin(URL.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    // Gets the HTTP status code.
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    Serial.println("Sending data to Google spreadsheet...");
    Serial.print("URL: ");
    Serial.print(URL);
    Serial.println();

    // Getting response from google sheets.
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload);    
    }
    
    http.end();
    //::::::::::::::::::
  }
}