#include <Arduino.h>

const int relayPin = 10;
const int water_fall_time = 2000;

String cmd = "";
unsigned long startTime = 0;

enum State {
  INIT,
  WAIT_CMD,
  WATERING
};
State currentState = INIT;

void setup() {
  pinMode(relayPin,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  switch(currentState){
    case INIT:
      Serial.println("State: INIT");
      digitalWrite(relayPin, LOW);
      cmd = "";
      currentState = WAIT_CMD;
      break;
    case WAIT_CMD:
      while(Serial.available()){
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
          cmd.trim();
          if(cmd == "on"){
            Serial.println("Command received: on");
            digitalWrite(relayPin, HIGH);
            startTime = millis();
            currentState = WATERING;
          }
          cmd = "";
        } else {
          cmd += c;
        }
      }
      break;
    case WATERING:
      if(millis() - startTime >= water_fall_time){
        digitalWrite(relayPin, LOW);
        Serial.println("Watering complete");
      }
      break;
    default:
      Serial.println("Unknown state, resetting...");
      currentState = INIT;
      break;
  }
  
  delay(100);
}
