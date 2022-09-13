/*=========================================

工作9-8     使用Encoder回傳轉速
工作9-12    Tera term結合Excel繪圖

=========================================*/

#include <Arduino.h>

volatile double Counter = 0;
double Resolution = 1024;
double Time_Previous = 0;

double Caculate_rpm() {
    if (Counter >= 1) {
        double Time_Current = (double)millis() / 1000;
        double Interval = (Time_Current - Time_Previous);
        double Vrpm = (double)60 * Counter / Resolution / Interval;
        Time_Previous = Time_Current;
        Counter = 0;
        return Vrpm;
    }
}

void blink() {
    Counter++;
}

void setup() {
    Serial.begin(9600);
    attachInterrupt(0, blink, RISING);
}

void loop() {
    Serial.println(Caculate_rpm());
}
