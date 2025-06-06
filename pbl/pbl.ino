const int motorCwPin = 9;
const int motorCcwPin = 10;
int motorSpeed = 1023;
// 0 ~ 1023

String cmd = "";

void setup() {
  pinMode(motorCwPin, OUTPUT);
  pinMode(motorCcwPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("serail begin!");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      cmd.trim(); // 改行や空白を除去
      if (cmd == "cw") {
        Serial.println("motor cw");
        analogWrite(motorCwPin, motorSpeed);
        analogWrite(motorCcwPin, 0);
      } else if (cmd == "ccw") {
        Serial.println("motor ccw");
        analogWrite(motorCwPin, 0);
        analogWrite(motorCcwPin, motorSpeed);
      } else if (cmd == "stop") {
        Serial.println("motor stop");
        analogWrite(motorCwPin, 0);
        analogWrite(motorCcwPin, 0);
      }else if (cmd == "break") {
        Serial.println("motor break");
        analogWrite(motorCwPin, motorSpeed);
        analogWrite(motorCcwPin, motorSpeed);
      } else {
        Serial.println("unknown command: " + cmd);
      }
      cmd = "";  // 入力リセット
    } else {
      cmd += c;  // 入力文字列に追加
    }
  }
}