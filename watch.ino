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

    Serial.begin(9600);
    Wire.begin();
    data_moni.init_screen();
    data_moni.init_step_tracker();

    //attachInterrupt(digitalPinToInterrupt(5), receive, RISING);    //set RECEIVE as interrupt function

    connectAWS();
}

// the loop function runs over and over again until power down or reset
void loop() {
    client.loop();
    data_moni.step_tracker(); 
    if (millis() % time == 0) {
        publishMessage();
    }
}
