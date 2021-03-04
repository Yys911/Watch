/*
  modified 24 Feb 2021
  by Yishun Zhao
 */

#pragma once

#ifndef _WATCH_h
#define _WATCH_h

#if defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#else
#define SerialMonitorInterface Serial
#endif


#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>		//library for displaying data
#include <DFRobot_Heartrate.h>		//library for heart rate sensor
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>

#define Weight 60
#define Height 170
#define Stride 60
#define time 60000
#define heartrate_pin 3
#define vibrate_pin 4
#define beep_pin 5
#define xStill 1636      
#define yStill 192
#define zStill 17536
#define MPU 0x68	         //since AD0 = LOW
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 32     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # for screen(or -1 if sharing Arduino reset pin)
#define AWS_IOT_PUBLISH_TOPIC   "arduino/outgoing"
#define AWS_IOT_SUBSCRIBE_TOPIC "arduino/incoming"


enum Var_name { Stepping, Heart_rate, StepCount, Calories_burning, Sleeping };


uint16_t mean_val(uint16_t* nums);
void connectAWS();
void publishMessage();
void messageHandler(String& topic, String& payload);
void v_b(byte* sensor);

class Data_monitoring {
public:
	friend void send(Data_monitoring data_moni);			//Function to send data to the server
	char get_data(Var_name name);
	void step_tracker();
	void init_screen();
	void init_step_tracker();
	void heartrate_tracker();
	void calculating();
	void whether_sleeping();
	void display_data();  //display function is used to display massage on screen

private:
	bool stepping;
	bool sleeping;
	uint16_t heart_rate;
	uint16_t stepCount;
	uint16_t calories_burning;
	uint16_t fat_burning;
	uint16_t distance;

};

#endif
