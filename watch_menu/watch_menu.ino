/*
 Name:		watch.ino
 Created:	2021/2/27 22:30:44
 Last modified:   2021/3/3
 Author:	yiyison, ps2g19
*/


#include "watch.h"

Data_monitoring data_moni;


// the setup function runs once when you press reset or power the board
void setup() {
    /*
      I/O pin setups
    */

    Serial.begin(115200);
    Wire.begin();
    data_moni.init_screen();
    data_moni.init_step_tracker();
    init_feedback();
    configTime(GMTOFFSET_SEC, DAYLIGHTOFFSET_SEC, NTPSERVER);
    //printLocalTime();

    #if defined(SERVER_ON)
    connectAWS();
    #endif

    pinMode(MENU_UP, INPUT_PULLUP);
    pinMode(MENU_DOWN, INPUT_PULLUP);
    attachInterrupt(MENU_UP, isrMenuUp, FALLING);
    attachInterrupt(MENU_DOWN, isrMenuDown, FALLING);
}

// the loop function runs over and over again until power down or reset
void loop() {
    client_loop();
    //connectAWS();
    data_moni.step_tracker(); 
    data_moni.heartrate_tracker();
    #if defined(SERVER_ON)
    if (millis() % TIME1 == 0) {
        publishMessage(String(data_moni.heart_rate)); 
    }
    #endif
    /*if (millis() % TIME2 == 0) {
        Serial.println("getting heartrate");
        data_moni.heartrate_tracker();
    }*/
    data_moni.display_data();
}
