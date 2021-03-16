/*
 Name:		watch.ino
 Created:	2021/2/27 22:30:44
 Last modified:   2021/3/3
 Author:	yiyison, ps2g19
*/


#include "watch.h"

//set the data monitoring calss as global variable
Data_monitoring data_moni;


// the setup function runs once when you press reset or power the board
void setup() {
    /*
      I/O pin setups
    */

    // set up the serial monitor
    Serial.begin(115200);

    //set up all the modules
    Wire.begin();
    data_moni.init_screen();
    data_moni.init_step_tracker();
    init_feedback();
    configTime(GMTOFFSET_SEC, DAYLIGHTOFFSET_SEC, NTPSERVER);
    //printLocalTime();

    time_sync_loop();
    //connect to wifif and server
    #if defined(SERVER_ON)
    connectAWS();
    #endif

    //set up menu buttons and interuppts
    pinMode(MENU_UP, INPUT_PULLUP);
    pinMode(MENU_DOWN, INPUT_PULLUP);
    attachInterrupt(MENU_UP, isrMenuUp, FALLING);
    attachInterrupt(MENU_DOWN, isrMenuDown, FALLING);

    //delay(5000);
}

// the loop function runs over and over again until power down or reset
void loop() {
    //check server for messages to be sent or recived
    client_loop();
    //connectAWS();

    //get heart rate and steps
    data_moni.step_tracker(); 
    data_moni.heartrate_tracker();

    // if it has been set time since last message published send a new one
    #if defined(SERVER_ON)
    if (millis() % TIME1 == 0) {
        //publish message in correct packet with time infomation so the server can use the most up to date values
        publishMessage(String(data_moni.year) + String(data_moni.month_d) + String(data_moni.day) + String(data_moni.hour) + String(data_moni.minuet) + "2"+ String(data_moni.stepCount)); 
    }
    #endif
    /*if (millis() % TIME2 == 0) {
        Serial.println("getting heartrate");
        data_moni.heartrate_tracker();
    }*/
    
    //put the infomation on the display
    data_moni.display_data();
}
