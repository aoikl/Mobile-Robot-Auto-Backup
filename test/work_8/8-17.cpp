/*=========================================

工作8-17 輸入數值來控制LED的亮度

=========================================*/
#include <Arduino.h>
#define led 11                    // pwm腳位
#define potPin A0                 //可變電阻腳位
int cmd;                          //使用者輸入的數值
void LED_PWM_CONTROL(int _cmd) {  //控制LED的PWM電壓
    switch (_cmd) {
        case 49:  //用對應到ASCII碼判斷輸入
            Serial.println("led pwm: 50");
            analogWrite(led, 50);
            delay(150);
            break;
        case 50:
            Serial.println("led pwm: 150");
            analogWrite(led, 150);
            delay(150);
            break;
        case 51:
            Serial.println("led pwm: 250");
            analogWrite(led, 250);
            delay(150);
            break;
        default:
            Serial.println("輸入錯誤，請重新輸入!");
            break;
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(led, OUTPUT);
    Serial.println("等待輸入...");
}

void loop() {
    while (Serial.available() == 0) {
    }
    cmd = Serial.read();   //讀取使用者輸入數值
    LED_PWM_CONTROL(cmd);  //控制LED的PWM電壓
}