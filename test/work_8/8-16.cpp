/*=========================================

工作8-16 使用可變電阻來控制LED的亮度

=========================================*/
#include <Arduino.h>
#define led 11  // pwm腳位
#define potPin A0  //可變電阻腳位

void Variable_Resistance() {
    int sensorValue = analogRead(potPin);
    Serial.println(sensorValue, DEC);
    sensorValue = map(sensorValue, 0, 1023, 0, 255);  // 轉換 0-1023 到 0-255
    analogWrite(led, sensorValue);
    delay(150);
}

void setup() {
    Serial.begin(9600);
    pinMode(led, OUTPUT);
}

void loop() {
    Variable_Resistance();
}