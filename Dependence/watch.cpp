#include "watch.h"
#include <string>
using namespace std;

#define heartrate_pin 3
#define vibrate_pin 4
#define beep_pin 5

void con_to_wifi(char* ssid, char* password) {
    WiFi.setPins(8, 2, A3, -1);
    while (!SerialMonitorInterface);

    // Attempt to connect to Wifi network:
    oled.putString("Connecting Wifi: ");
    oled.putString(ssid);

    // Connect to WiFi, and loop until connection is secured
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        oled.putString(".");
        delay(500);
    }
    oled.clearDisplay();
}

uint16_t mean_val(uint16_t* nums) {
    int sum = 0;
    for (int i = 0; i < 20; i++) {
        sum = nums[i] + sum;
    }
    uint16_t mean = sum / 20;
    return mean;
}

void screen_ini() {
    oled.init();                      // Initialze SSD1306 OLED display
    oled.clearDisplay();              // Clear screen
    oled.setTextXY(0, 0);              // Set cursor position
}

void SEND(void) {

}

void RECEIVE(void) {

}


void Data_monitoring::init_step_tracker() {
    Wire.begin();					//create i2c wire
    Wire.beginTransmission(MPU);	//start transmission to accelerometer
    Wire.write(0x6B); 				//reset accelerometer
    Wire.write(0);
    Wire.endTransmission(true);		//end transsmission
    stepping = false;
    stepCount = 0;
}

void Data_monitoring::step_tracker() {
    Wire.beginTransmission(MPU);	//begin i2c transsmoion from accelerometer
    Wire.write(0x3B);  				//go to the regeister where where data stored
    Wire.endTransmission(false);	//reset transsmission just incase
    Wire.requestFrom(MPU, 6, true);  	//request 6 bytes from MPU(accelereometer

    int16_t accX = Wire.read() << 8 | Wire.read();//read both x-axis accerlerometer bytes and put them together to be 16 bits
    int16_t accY = Wire.read() << 8 | Wire.read();//same for y-axis
    int16_t accZ = Wire.read() << 8 | Wire.read();//same for z-axis
    int16_t totalAc = sqrt(accX * accX + accY * accY + accZ * accZ);//calc magnitude of the acceleration 

    if (totalAc > 26504 && !stepping) {	//hystorisis switching points for steps
        stepping = true;
        stepCount++;
    }
    else if (totalAc < 22427 && stepping) {
        stepping = false;
    }

    Serial.println(stepCount);		//print step count to monitor for debuging

    //return stepCount;				//return value
}

void Data_monitoring::heartrate_tracker() {
    uint16_t rateValue[20];
    for (int i = 0; i < 20; i++) {
        heartrate.getValue(heartrate_pin);    //A0 foot sampled values
        rateValue[i] = heartrate.getRate();    //Get heart rate value
    }
    heart_rate = mean_val(rateValue);
}

void Data_monitoring::calories_buring_tracker() {

}

char Data_monitoring::get_data(Var_name name) {
    switch (name) {
    case Stepping:
        return char(stepping);
        break;

    case Heart_rate:
        return char(heart_rate);
        break;
    case StepCount:
        return char(stepCount);
        break;
    case Calories_burning:
        return char(calories_burning);
        break;
    case Sleeping:
        return char(sleeping);
        break;
    }
}

void Data_monitoring::whether_sleeping() {
    if (50 < heart_rate < 60 && !stepping) {
        sleeping = true;
    }
}

void Data_monitoring::display_data() {
    oled.clearDisplay();              // Clear screen
    oled.setTextXY(0, 0);              // Set cursor position
    oled.putString("Heart rate: "); oled.putNumber(heart_rate); oled.putString(" bpm");
    oled.putString("Steps: "); oled.putNumber(stepCount);
    oled.putString("Calories buring: "); oled.putNumber(calories_burning); oled.putString(" kcal");
}


void Response_from_sever::v_b(char sensor) {
    int pin = 0;
    if (sensor == 'v') {
        pin = vibrate_pin;
    }
    else if (sensor == 'b') {
        pin = beep_pin;
    }
    else {

    }

    for (int i = 0; i < 3; i++) {
        digitalWrite(pin, HIGH);
        delay(100);
        digitalWrite(pin, LOW);
        delay(100);
        digitalWrite(pin, HIGH);
        delay(100);
        digitalWrite(pin, LOW);
        delay(1000 - 300);
    }
}
