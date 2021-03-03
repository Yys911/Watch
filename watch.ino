/*
 Name:		watch.ino
 Created:	2021/2/27 22:30:44
 Last modified:   2021/3/3
 Author:	yiyison, ps2g19
*/


#include "watch.h"

Server_related server;
Data_monitoring data_moni;

const char* ssid = "qqq";    //network SSID (name)
const char* password = "***";    //network password
const char* mqttServer = "3.133.147.7";

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

    server.connectWifi(ssid, password);    //connect to specified wifi
    server.connectMQTTServer(mqttServer);
}

// the loop function runs over and over again until power down or reset
void loop() {
    server.loop();
    send(data_moni);
    data_moni.step_tracker();
}
