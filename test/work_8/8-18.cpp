/*=========================================

工作8-18 LED 漸亮漸暗

=========================================*/
#include <Arduino.h>
#define led 11         // pwm腳位
#define potPin A0      //可變電阻腳位
int cmd;               //使用者輸入的數值
void LED_PWM_AUTO() {  // LED 漸亮漸暗
    Serial.print("[LED漸亮]");
    for (int i = 0; i < 255; i = i + 5) {
        Serial.print("LED PWM: ");
        Serial.println(i);
        analogWrite(led, i);
        delay(100);
    }
    Serial.print("[LED漸暗]");
    for (int i = 255; i > 0; i = i - 5) {
        Serial.print("LED PWM: ");
        Serial.println(i);
        analogWrite(led, i);
        delay(100);
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(led, OUTPUT);
    Serial.println("LED漸亮漸暗...");
}

void loop() {
    LED_PWM_AUTO();  // LED 漸亮漸暗
}