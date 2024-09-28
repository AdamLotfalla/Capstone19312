#include <Arduino.h>

#define IDK 3

bool ON = true;

void setup(){
  pinMode(IDK, INPUT);
  digitalWrite(IDK, LOW);
}

void loop(){
  digitalWrite(IDK, ON);
  ON = !ON;
  delay(5000);
  // adjust delay if needed
}