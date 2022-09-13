/*=========================================

工作8-5 讀值並判斷輸入是數字、大小寫字母、符號

=========================================*/

#include <Arduino.h>

char cmd;                      //使用者輸入值
void distinguish(char _cmd) {  //輸入判斷函式
    if (_cmd >= 48 && _cmd <= 57) {
        Serial.println("");
        Serial.print("輸入為數字 ");
        Serial.println(cmd);
    } else if (_cmd >= 65 && _cmd <= 90) {
        Serial.println("");
        Serial.print("輸入為大寫字母 ");
        Serial.println(cmd);
    } else if (_cmd >= 97 && _cmd <= 122) {
        Serial.println("");
        Serial.print("輸入為小寫字母 ");
        Serial.println(cmd);
    } else {
        Serial.println("");
        Serial.print("輸入為符號 ");
        Serial.println(cmd);
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("[工作8-5]");
    Serial.println("等待輸入...");
    Serial.print("");
}

void loop() {
    while (Serial.available() == 0) {
    }
    cmd = Serial.read();
    distinguish(cmd);
}