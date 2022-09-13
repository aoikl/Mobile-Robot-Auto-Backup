#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "RTClib.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
RTC_DS1307 rtc;

//======
const char* string_menu_list =  // setup the selection list   设置菜单界面显示类容
    "Time\n"
    "Time\n"
    "Time\n";

uint8_t current_selection1 = 0;  // set the default selectes
int8_t button_event = 0;         // a variable to check the button event

void check_button_event() {  // the fuction of check the button event
    if (button_event == 0)
        button_event = u8g2.getMenuEvent();
    //这个函数的功能是为了实现检测按钮事件，当按下HOME键时结束当前执行程序返回至菜单界面
}
void menu_display() {
    u8g2.setFont(u8g2_font_6x12_tr);
    current_selection1 = u8g2.userInterfaceSelectionList(  // show the menu
        "Menu",
        1,
        string_menu_list);
}

void hello() {  // show the "Hello World!", you instead of it to use anything you want display
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.drawStr(0, 24, "Hello World!");
        check_button_event();
        if (button_event == U8X8_MSG_GPIO_MENU_HOME)  // check the button event of HOME BUTTON, If: return menu ,No: keep state
            current_selection1 = 0;

    } while (u8g2.nextPage());
    // delay(1000);
}

void time_display() {  // to display time

    DateTime now = rtc.now();
    u8g2.setFontDirection(0);
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.setCursor(20, 10);
        u8g2.print("Welcome!");
        u8g2.setCursor(0, 30);
        u8g2.print("Date:");
        u8g2.setCursor(50, 30);
        u8g2.print(now.year());
        u8g2.print('/');
        u8g2.print(now.month());
        u8g2.print('/');
        u8g2.println(now.day());
        u8g2.setCursor(0, 50);
        u8g2.print("Time:");
        u8g2.setCursor(50, 50);
        u8g2.print(now.hour());
        u8g2.print(':');
        u8g2.print(now.minute());
        u8g2.print(':');
        u8g2.println(now.second() + 10);
        check_button_event();
        if (button_event == U8X8_MSG_GPIO_MENU_HOME)
            current_selection1 = 0;

    } while (u8g2.nextPage());
}

//======
void setup() {
    // put your setup code here, to run once:
    u8g2.begin(/*select_pin */ A0, /*next_pin */ A1, /* prev_pin*/ U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE, /*home_pin*/ A2);  //设置A0 pin 为选择按钮， A1 pin 为切换到下一个选项 ， A2 pin 为HOME键
    u8g2.setFont(u8g2_font_6x12_tr);                                                                                               //设置显示字体
    u8g2.enableUTF8Print();                                                                                                        //启用 UTF8打印
    rtc.begin();                                                                                                                   //初始化时钟模块
}

void loop() {
    // put your main code here, to run repeatedly:
    if (button_event > 0)
        button_event = 0;  //按下HOME键后，button_event值会变为U8X8_PIN_NONE，此时会跳出当前程序，将值改为0

    switch (current_selection1) {  // to chose which display to show
                                   //选择要显示的界面（事件、温度、湿度、距离）
        case 0:
            menu_display();
            break;

        case 1:

            time_display();
            break;
    }
}