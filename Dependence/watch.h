/*
  modified 24 Feb 2021
  by Yishun Zhao
 */


#include "sensors.h"				//the library declears sensors functions
#include <WiFi.h>
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>		//library for displaying data
#include <ArduinoJson.h>			//library for serialization and deserialization


void Debounce_function();
void SEND(void);					//Function to send data to the server
void RECEIVE(void);					//Function to receive data from the server


class Data_monitoring_class {
public:
	void step_tracker();
	void heartrate_tracker();
	void calories_buring_tracker();

private:
	int daily_steps;
	int calories_burning;
	bool whether_sleeping;

};


class Display_class {
public:
	void display_function();  //display function is used to display massage on screen

private:
	char data_need_to_be_displayed[];
};


class Response_from_sever_class {
public:
	void vibrate_function();
	void beep_function();

private:
	int lasting_time;
}; #pragma once
