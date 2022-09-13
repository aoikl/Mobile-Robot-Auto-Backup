/*========================================

            Include

*interface
command ending: \r
char delay: 5

*Repeat
Delay: 0
Period: 5
========================================*/
/*[------------------------------------------------]

    WeMos D1 Mini Motor Drive Shield (TB6612)

[------------------------------------------------]*/
#include <Arduino.h>
#include <SPI.h>
#include "WEMOS_Motor.h"
// I2C位址
Motor Left_FRONT(0x2E, _MOTOR_A, 1000);   // LeftFront
Motor Left_REAR(0x2E, _MOTOR_B, 1000);    // LeftRear
Motor Right_FRONT(0x30, _MOTOR_B, 1000);  // RightFront
Motor Right_REAR(0x30, _MOTOR_A, 1000);   // RightRear

//馬達速度控制，範圍(-100,100)
void WheelSpeed(int speedL, int speedR) {
    //左輪
    if (speedL > 0) {
        Left_FRONT.setmotor(_CW, speedL);
        Left_REAR.setmotor(_CW, speedL);

    } else if (speedL < 0) {
        speedL = -speedL;
        Left_FRONT.setmotor(_CCW, speedL);
        Left_REAR.setmotor(_CCW, speedL);
    } else {
        Left_FRONT.setmotor(_STANDBY);
        Left_REAR.setmotor(_STANDBY);
    }
    //右輪
    if (speedR > 0) {
        Right_FRONT.setmotor(_CCW, speedR);
        Right_REAR.setmotor(_CCW, speedR);
    } else if (speedR < 0) {
        speedR = -speedR;
        Right_FRONT.setmotor(_CW, speedR);
        Right_REAR.setmotor(_CW, speedR);
    } else {
        Right_FRONT.setmotor(_STANDBY);
        Right_REAR.setmotor(_STANDBY);
    }
}
/*[------------------------------------------------]

    AsyncElegantOTA
    192.168.137.

[------------------------------------------------]*/
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
//手機WIFI的帳號密碼
const char* ssid = "ご注文はWIFIですか?";
const char* password = "111111111";
//上次更新程式的時間
const char compile_date[] = __DATE__ " " __TIME__;
//客戶端讀取用緩存
String readBuff = "", readBuff_subStr = "", readBuff_subStr2 = "";
int readBuff_Int = 0, readBuff_Int2 = 0;
//測試連接的次數
uint8_t WifiTryCount = 0;
//聲明服務器
WiFiServer TCPclient_server(443);
WiFiClient client = TCPclient_server.available();
AsyncWebServer OTA_server(80);
// NodeMCU-32 LED燈引腳
#define LED_BUTTON 2

// WIFI初始化
void WIFI_Init() {
    pinMode(LED_BUTTON, OUTPUT);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);  //關閉STA模式下WIFI休眠，提高響應速度
    WiFi.disconnect();
    delay(250);
    WiFi.begin(ssid, password);
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        if (WifiTryCount++ >= 20) {  //嘗試20次未連上網路，則重新啟動
            ESP.restart();
        }
    }
    Serial.println("[Connected]");
    Serial.println("IP address:");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.println(443);
    Serial.println("");

    AsyncElegantOTA.begin(&OTA_server);  //開啟ElegantOTA上傳網頁
    OTA_server.begin();
    TCPclient_server.begin();
    TCPclient_server.setNoDelay(true);
    Serial.println("WIFI Init!");
    Serial.println("WaitingForConnection");
    while (!client) {  //  等待連接並閃燈
        client = TCPclient_server.available();
        digitalWrite(LED_BUTTON, HIGH);
        delay(300);
        digitalWrite(LED_BUTTON, LOW);
        delay(300);
    }
    delay(100);
    digitalWrite(LED_BUTTON, LOW);
    client.println("=>[Client connented]");
    client.println("Compile timestamp: ");
    client.println(compile_date);
}

//判斷使用者是否按下緊急停止
boolean stop = false;
void EstopCheck() {
    if (client.available()) {
        readBuff = client.readStringUntil('\r');
        client.print(readBuff + '\r');
        if (readBuff.startsWith("STOP")) {
            readBuff = "";
            stop = true;  //若接收到STOP則跳出迴圈
        }
    }
}
/*[------------------------------------------------]

    HC-SR04 & Buzzer

[------------------------------------------------]*/
//超音波和蜂鳴器模組的引腳
#define Trig 16
#define Echo 17
#define Buzzer 4

//超音波模組初始化
void Ultrasound_Init() {
    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
    pinMode(Buzzer, OUTPUT);
    digitalWrite(Trig, LOW);
}

//計算當前距離
float GetDistance() {
    float distance = 0, duration = 0;
    digitalWrite(Trig, HIGH);
    delay(50);
    digitalWrite(Trig, LOW);
    duration = pulseIn(Echo, HIGH);
    distance = duration * 0.01715;
    // client.println((String)distance + "cm");
    return distance;
}

//蜂鳴器發聲
void Buzz(long frequency, long length) {
    long delayValue = 1000000 / frequency / 2;
    long cycles = frequency * length / 1000;
    for (long i = 0; i < cycles; i++) {
        digitalWrite(Buzzer, HIGH);
        delayMicroseconds(delayValue);
        digitalWrite(Buzzer, LOW);
        delayMicroseconds(delayValue);
    }
}
/*[-----------------------------------------------]

    Mode

[------------------------------------------------]*/
//自動倒車模式
void ModeBackUp() {
    client.println("=>[ModeBackUp]");
    float MaxDetect = 100;                     //最大離牆距離
    float Dist = 0, preDist = 0;               //當前與上次距離
    float differ = 0;                          //當前與上次距離差值
    boolean selfpark = false, backup = false;  //自動停車可行性、倒車狀態的判斷
    int count = 0;                             //偵測次數
    int stage = 1;                             //自動停車階段
    long time = millis();
    stop = false;
    //逆轉並判斷離牆距離，若太遠則不自動停車
    do {
        EstopCheck();
        Dist = GetDistance();
        if (Dist > MaxDetect) {
            WheelSpeed(-40, 40);
            Dist = GetDistance();
        } else {
            WheelSpeed(0, 0);
            selfpark = true;
        }
    } while ((client.connected()) && (millis() - time < 5000) && (selfpark == false) && (stop == false));

    //若太遠則直接完成，後續selfpark不執行
    if (selfpark != true) {
        client.println("Exceeded maximum limit!");
    } else {
        preDist = GetDistance();
        WheelSpeed(-40, 40);
        Dist = GetDistance();  //先逆轉並計算差值
        differ = preDist - Dist;
    }

    //用differ的正負做順逆轉判斷
    while ((client.connected()) && selfpark && (stop == false)) {
        EstopCheck();
        client.println("differ=" + (String)differ);
        differ = preDist - Dist;
        if (stage == 1) {  // stage1
            if (differ > 0) {
                preDist = GetDistance();
                WheelSpeed(-20, 20);  // 逆時針轉
                Dist = GetDistance();
            } else {  // stage2，並轉stage3
                client.println("stage=3");
                preDist = GetDistance();
                WheelSpeed(20, -20);  // 改順時針轉
                Dist = GetDistance();
                stage = 3;
                differ = preDist - Dist;  //轉之前要先計算距離
            }
        }

        if (stage == 3) {  // stage3
            if (differ > 0) {
                preDist = GetDistance();
                WheelSpeed(20, -20);  //順時針轉
                Dist = GetDistance();
            } else {  // stage4，並轉stage1
                preDist = GetDistance();
                WheelSpeed(-20, 20);  // 改逆時針轉
                Dist = GetDistance();
                stage = 1;
                differ = preDist - Dist;  //轉之前要先計算距離
            }
        }

        //在differ正負轉換時，若超音波離牆越垂直，differ會越小
        if (abs(differ) < 0.1) {
            count++;  //足夠小就增加
        }

        //修正到一定次數即表示車子離牆足夠垂直，開始倒車
        if (count > 10) {
            WheelSpeed(0, 0);
            selfpark = false;
            backup = true;
        }
    }

    //倒車並發出警示聲
    while ((client.connected()) && backup && (stop == false)) {
        EstopCheck();
        Dist = GetDistance();
        client.println((String)Dist + ":cm");
        if (Dist > 6) {  //超音波離車尾距離
            Buzz(800, 100);
            WheelSpeed(-(int)(Dist), -(int)(Dist));  //離牆越近越慢
            Buzz(1200, 100);
        } else {
            Buzz(1500, 50);  //短發兩聲代表完成
            delay(100);
            Buzz(1500, 50);
            backup = false;
        }
    }
    WheelSpeed(0, 0);
    client.println("=>[Complete ModeBackUp]");
}
/*========================================

    Setup & Loop

========================================*/
void setup() {
    Serial.begin(115200);
    Wire.setClock(400000);
    WIFI_Init();
    Ultrasound_Init();
}

void loop() {
    while (client.connected()) {  //如果客戶端處於連接狀態
        ModeBackUp();
    }
    WheelSpeed(0, 0);
    Serial.println("=>[Client lost]");
    Serial.println("RESTART...");
    delay(250);
    ESP.restart();  //客戶端斷線則重啟，以方便連接
}