#include "watch.h"


void con_to_wifi(char* ssid, char* password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WIFI.....");
    }
    Serial.println("Connected");
}


uint8_t mean(uint8_t* nums) {
    int sum = 0;
    for (int i = 0; i < 20; i++) {
        sum = nums[i] + sum;
    }
    uint8_t mean = sum / 20;
    return mean;
}


void SEND(void) {

}


void RECEIVE(void) {

}



void Data_monitoring::step_tracker() {

}

void Data_monitoring::heartrate_tracker() {
    uint8_t rateValue[20];
    for (int i = 0; i < 20; i++) {
        heartrate.getValue(heartrate_pin);    //A0 foot sampled values
        rateValue[i] = heartrate.getRate();    //Get heart rate value
    }
    heart_rate = mean(rateValue);
    //delay(20);
}

void Data_monitoring::calories_buring_tracker() {

}

void Data_monitoring::whether_sleeping() {
    if (50 < heart_rate < 60) {
        sleeping = true;
    }
}


void Display::ini() {
    oled.init();                      // Initialze SSD1306 OLED display
    oled.clearDisplay();              // Clear screen
    oled.setTextXY(0, 0);              // Set cursor position
}

void Display::display_function() {

}


void Response_from_sever::vibrate_function() {

}

void Response_from_sever::beep_function() {

}
