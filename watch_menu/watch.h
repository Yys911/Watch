/*
  modified 24 Feb 2021
  by Yishun Zhao, Peter Strong
 */

#pragma once

#ifndef _WATCH_h
#define _WATCH_h

#define SERVER_ON

//debug for when using different board with native usb
#if defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#else
#define SerialMonitorInterface Serial
#endif

// include the needed header files, to make the use of other modules easier
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "time.h"
#include <sys/time.h>

#include <Wire.h>

// headers for using the screen
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>      // Hardware-specific library for ST7789
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions
#include <SPI.h>

// unsed screen
//#include <Adafruit_SSD1306.h>		//library for displaying data

#include <DFRobot_Heartrate.h>		//library for heart rate sensor


//values for if doing more calaculations 
//un-used
#define Weight 60
#define Height 170
#define Stride 60

#define TIME1 10000 //set time (ms) for sending data to the server
//#define TIME2 1000

//set pins for modules
#define heartrate_pin 34
#define MOTOR 32
#define BUZZER 33

//set values for when the accelerometer is still
#define xStill 1636      
#define yStill 192
#define zStill 17536
//set register start for getting data from accelerometer
#define MPU 0x68	         //since AD0 = LOW


#define SCREEN_WIDTH 320     // TFT display width, in pixels
#define SCREEN_HEIGHT 240    // TFT display height, in pixels
//#define OLED_RESET -1      // Reset pin # for screen(or -1 if sharing Arduino reset pin)
#define TFT_CS         5     // TFT screen Chip select Pin
#define TFT_RST        17    // TFT Reset Pin
#define TFT_DC         16    // TFT D/C Pin
#define SD_CS          4     // TFT SD card slot Chip Select pin
#define MENU_UP        12    // Menu up button pin 
#define MENU_DOWN      13    // Menu down button pin
#define ROTATION       3     // Screen orientation


#define NTPSERVER "pool.ntp.org"  //server to get time from
#define GMTOFFSET_SEC 0           //time zone offset
#define DAYLIGHTOFFSET_SEC 3600   //set day light saving offset


// AWS Topics
#define AWS_IOT_PUBLISH_TOPIC   "arduino/outgoing"
#define AWS_IOT_SUBSCRIBE_TOPIC "arduino/incoming"



enum Var_name { Stepping, Heart_rate, StepCount, Calories_burning, Sleeping };


uint16_t mean_val(uint16_t* nums);
void connectAWS();
void publishMessage(String Data);
void messageHandler(String& topic, String& payload);
void client_loop();
//void v_b(byte* sensor);
void init_feedback();
void feedback();
void IRAM_ATTR isrMenuDown();
void IRAM_ATTR isrMenuUp();
void time_sync_loop();

//class for getting the data
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
  void printLocalTime();
  uint16_t stepCount;
  uint16_t heart_rate;
  char hour[3];
  char minuet[3];
  char day[3];
  char month[10];
  char month_d[3];
  char year[5];
  char prev_minuet[3];
private:
	bool stepping;
	bool sleeping;
  uint16_t prev_heart_rate;
  uint16_t prev_steps;
	uint16_t calories_burning;
	uint16_t fat_burning;
	uint16_t distance;
  uint8_t prev_page;
  //DFRobot_Heartrate heartrate(DIGITAL_MODE); ///< ANALOG_MODE or DIGITAL_MODE

};

#endif
