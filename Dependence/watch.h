/*
  modified 24 Feb 2021
  by Yishun Zhao
 */


#ifndef _WATCH_h
#define _WATCH_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif


//#include "sensors.h"				//the library declears sensors functions
#include <WiFi.h>
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>		//library for displaying data
#include <ArduinoJson.h>			//library for serialization and deserialization
#include <DFRobot_Heartrate.h>		//library for heart rate sensor

//#define heartrate_pin A0


void con_to_wifi(char* ssid, char* password);
void mean(double* nums);
void SEND(void);					//Function to send data to the server
void RECEIVE(void);					//Function to receive data from the server


DFRobot_Heartrate heartrate(DIGITAL_MODE);


class Data_monitoring {
public:
	void step_tracker();
	void heartrate_tracker();
	void calories_buring_tracker();
	void whether_sleeping();

private:
	uint8_t heart_rate;
	int daily_steps;
	int calories_burning;
	bool sleeping;

};


class Display {
public:
	void ini();
	void display_function();  //display function is used to display massage on screen

private:
	char data_need_to_be_displayed[];
};


class Response_from_sever {
public:
	void vibrate_function();
	void beep_function();

private:
	int lasting_time;
}; 
#pragma once
