/*=========================================

工作9-3 按鈕計數 (p.50)

=========================================*/
#include <Arduino.h>

volatile int Counter = 0;

void blink() {
    Counter++;
}

void setup() {
    Serial.begin(9600);
    attachInterrupt(0, blink, RISING);
}

void loop() {
    Serial.println(Counter);
}