/*
 Name:		watch.ino
 Created:	2021/2/27 22:30:44
 Author:	yiyison, ps2g19
*/


#include "watch.h"
//#include "watch.h"
//#include "TimerOne.h"

#define ssid "qwe"
#define password "123"

#define MODE CHANGE


Display dis;
Data_monitoring data_monitoring;


// the setup function runs once when you press reset or power the board
void setup() {
    /*
      I/O pin setups
    */
    Serial.begin(9600);
    Wire.begin();
    dis.ini();    //initialise screen
    con_to_wifi(ssid, password);    //connect to specified wifi
    attachInterrupt(digitalPinToInterrupt(5), RECEIVE, MODE);    //set RECEIVE as interrupt function
    //Timer1.initialize(30000000);    //initialise Timer1, interrupt every 30s
    //Timer1.attachInterrupt(SEND);    //set SEND as interrupt function
    data_monitoring.init_step_tracker();
}

// the loop function runs over and over again until power down or reset
void loop() {
  data_monitoring.step_tracker();
}
