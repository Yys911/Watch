/*
 Name:		watch.ino
 Created:	2021/2/27 22:30:44
 Author:	yiyison, ps2g19
*/


#include "watch.h"
#include "TimerOne.h"

#define ssid "qwe"
#define password "123"
#define heartrate_pin A0


#define heartrate_pin A0
#define vibrate_pin A1
#define beep_pin A2


Display Display;
Response_from_sever Response_from_sever;
Data_monitoring Data_monitoring;


char ssid[] = "qqq";    //network SSID (name)
char password[] = "***";    //network password


// the setup function runs once when you press reset or power the board
void setup() {
    /*
      I/O pin setups
    */
    Serial.begin(9600);
    Wire.begin();
    dis.ini();    //initialise screen
    con_to_wifi(ssid, password);    //connect to specified wifi
    attachInterrupt(digitalPinToInterrupt(5), RECEIVE, mode);    //set RECEIVE as interrupt function
    Timer1.initialize(30000000);    //initialise Timer1, interrupt every 30s
    Timer1.attachInterrupt(SEND);    //set SEND as interrupt function
    Data_monitoring.init_step_tracker();
}

// the loop function runs over and over again until power down or reset
void loop() {
  Data_monitoring.step_tracker();
}
