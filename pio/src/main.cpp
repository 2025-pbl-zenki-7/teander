#include <Arduino.h>

const int relayPin = 10;
const int motor1Pin = 11;
const int water_fall_time = 2000;

String cmd = "";
unsigned long startTime = 0;

enum State {
  INIT,
  WAIT_CMD
};
State currentState = INIT;

void setup() {
  pinMode(relayPin,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  switch(currentState){
    case INIT:
      Serial.println("initialized!!");
      digitalWrite(relayPin, LOW);
      cmd = "";
      currentState = WAIT_CMD;
      break;
    case WAIT_CMD:
      while(Serial.available()){
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
          cmd.trim();
          if(cmd == "relay_on"){
            Serial.println("Command received: relay on");
            digitalWrite(relayPin, HIGH);
          }else if(cmd == "relay_off"){
            Serial.println("Command received: relay off");
            digitalWrite(relayPin, LOW);
          }else if(cmd == "motor1_on") {
            Serial.println("Command received: motor on");
            digitalWrite(motor1Pin,HIGH);
          }else{
            Serial.println("Unknown command received");
          }
          cmd = "";
        } else {
          cmd += c;
        }
      }
      break;
    default:
      Serial.println("Unknown state, resetting...");
      currentState = INIT;
      break;
  }
  
  delay(100);
}
