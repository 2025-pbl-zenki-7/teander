#include <Arduino.h>
#include <Servo.h>

enum State {
  INIT,
  WAIT_CMD
};
State currentState = INIT;

enum MotorIndex{
  motor1,
  motor2,
  motor3,
  num_motors  // motor4と5は除外
};

const int relayPin = 13;

const int motorPins[num_motors] = {
  6,  //motor1
  7,  //motor2
  8   //motor3
};

// サーボ関連
Servo servo4;
Servo servo5;
const int servo4Pin = 9;
const int servo5Pin = 10;

unsigned long relayOffTime = 0;
unsigned long motorOffTime = 0;
unsigned long servo4OffTime = 0;
unsigned long servo5OffTime = 0;

const unsigned long relayActiveDuration = 2000;
const unsigned long motorActiveDuration = 3000;
const unsigned long servoActiveDuration = 5000;

String cmd = "";

void setup() {
  pinMode(relayPin, OUTPUT);
  for(int i = 0; i < num_motors; i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  // サーボ初期化
  servo4.attach(servo4Pin);
  servo5.attach(servo5Pin);

  Serial.begin(9600);
}

void logCommand(const String& cmd){
  Serial.println("Command received:"+cmd);
}

/*
対応コマンド
watering
motor1
motor2
motor3
servo4
servo5
reset
*/
void processCommand(String command){
  command.trim();
  logCommand(command);

  if (command == "watering") {
    digitalWrite(relayPin,HIGH);
    relayOffTime = millis() + relayActiveDuration;
  } else if (command.startsWith("motor")) {
    int motorNum = command.charAt(5) - '1';
    digitalWrite(motorPins[motorNum],HIGH);
    motorOffTime = millis() + motorActiveDuration;
  } else if (command == "servo4") {
    servo4.write(180);  // 任意の動作角度
    servo4OffTime = millis() + servoActiveDuration;
  } else if (command == "servo5") {
    servo5.write(90);
    servo5OffTime = millis() + servoActiveDuration;
  } else if (command == "reboot") {
    currentState = INIT;
  } else {
    Serial.println("Unknown command received.");
  }
}

void loop() {
  switch(currentState) {
    case INIT:
      Serial.println("initialized!!");
      digitalWrite(relayPin, LOW);
      for(int i = 0; i < num_motors; i++) {
        digitalWrite(motorPins[i], LOW);
      }
      servo4.write(0);
      servo5.write(0);
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
      // リレー自動オフ
      if(relayOffTime > 0 && millis() >= relayOffTime) {
        digitalWrite(relayPin,LOW);
        relayOffTime = 0;
      }
      // モーター自動オフ
      if(motorOffTime > 0 && millis() >= motorOffTime){
        digitalWrite(motorPins[0], LOW);
        digitalWrite(motorPins[1], LOW);
        digitalWrite(motorPins[2], LOW);

        motorOffTime = 0;
      }
      // サーボ自動オフ
      if (servo4OffTime > 0 && millis() >= servo4OffTime) {
        servo4.write(0);
        servo4OffTime = 0;
      }
      if (servo5OffTime > 0 && millis() >= servo5OffTime) {
        servo5.write(0);
        servo5OffTime = 0;
      }
      break;

    default:
      Serial.println("Unknown state, resetting...");
      currentState = INIT;
      break;
  }

  delay(50);  // 応答性と負荷のバランス
}