#include <Arduino.h>

enum State {
  INIT,
  WAIT_CMD
};
State currentState = INIT;

enum MotorIndex{
  motor1,
  motor2,
  motor3,
  motor4,
  motor5,
  num_motors
};

const int relayPin = 10;

const int motorPins[num_motors][2] = {
  {11,-1},  //motor1
  {12,-1},  //motor2
  {13,-1},  //motor3
  {6,7},    //motor4A/B
  {8,9}     //motor5A/B
};

String cmd = "";

void setup() {
  pinMode(relayPin,OUTPUT);
  for(int i = 0; i<num_motors; i++){
    pinMode(motorPins[i][0],OUTPUT);
    if(motorPins[i][1] != -1){
      pinMode(motorPins[i][1],OUTPUT);
    }
  }
  Serial.begin(9600);
}

void logCommand(const String& cmd){
  Serial.println("Command received:"+cmd);
}

void controlRelay(bool relay_flag){
  digitalWrite(relayPin, relay_flag ? HIGH : LOW);
}

void controlMotor(int motorIndex, int sideIndex, bool motor_flag){
  if(motorIndex < 0 || motorIndex > num_motors) return;
  if(sideIndex < 0 || sideIndex > 1) return;
  if(motorPins[motorIndex][sideIndex] == -1) return;
  digitalWrite(motorPins[motorIndex][sideIndex], motor_flag ? HIGH:LOW);
}

/*
対応コマンド
relay:on
relay:off
motor1:on
motor1:off
motor2:on
motor2:off
motor3:on
motor3:off
motor4A:on
motor4A:off
motor4B:on
motor4B:off
motor5A:on
motor5A:off
motor5B:on
motor5B:off
reset
*/
void processCommand(String command){
  command.trim();
  logCommand(command);

  if (command == "relay:on") {
    controlRelay(true);
  } else if (command == "relay:off") {
    controlRelay(false);
  } else if (command.startsWith("motor")) {
    int motorNum = command.charAt(5) - '1';  // 6番目の文字を取得し数値を1減らす(motorIndexに対応)
    if (motorNum < 0 || motorNum >= num_motors) {
      Serial.println("Invalid motor number.");
      return;
    }

    int sideIndex = 0;
    if (command.length() >= 7 && (command.charAt(6) == 'A' || command.charAt(6) == 'B')) {
      sideIndex = (command.charAt(6) == 'B') ? 1 : 0;
    }

    bool turnOn = command.endsWith(":on");
    bool turnOff = command.endsWith(":off");

    if (turnOn || turnOff) {
      controlMotor(motorNum, sideIndex, turnOn);
    } else {
      Serial.println("Invalid motor command.");
    }
  } else if (command == "reset") {
    currentState = INIT;
  } else {
    Serial.println("Unknown command received.");
  }
}


void loop() {
  switch(currentState){
    case INIT:
      Serial.println("initialized!!");
      digitalWrite(relayPin,LOW);
      for(int i = 0; i<num_motors; i++){
        digitalWrite(motorPins[i][0],LOW);
        if(motorPins[i][1] != -1){
          digitalWrite(motorPins[i][1],LOW);
        }
      }
      cmd = "";
      currentState = WAIT_CMD;
      break;

    case WAIT_CMD:
      while(Serial.available()){
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
          if(cmd.length() > 0){
            processCommand(cmd);
            cmd = "";
          }
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