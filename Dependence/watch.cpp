#include "watch.h"


void con_to_wifi(char* ssid, char* password) {
    WiFi.setPins(8, 2, A3, -1);
    while (!SerialMonitorInterface);

    // Attempt to connect to Wifi network:
    SerialMonitorInterface.print("Connecting Wifi: ");
    SerialMonitorInterface.println(ssid);

    // Connect to WiFi, and loop until connection is secured
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        SerialMonitorInterface.print(".");
        delay(500);
    }
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

void Data_monitoring::init_step_tracker(){
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
	Wire.requestFrom(MPU,6,true);  	//request 6 bytes from MPU(accelereometer
	
	accX=Wire.read()<<8|Wire.read();//read both x-axis accerlerometer bytes and put them together to be 16 bits
	accY=Wire.read()<<8|Wire.read();//same for y-axis
	accZ=Wire.read()<<8|Wire.read();//same for z-axis
	totalAc = sqrt(accX*accX + accY*accY + accZ*accZ);//calc magnitude of the acceleration 

	if(totalAc>26504 && !stepping){	//hystorisis switching points for steps
		stepping = true;
		stepCount++;
	}else if(totalAc<22427 && stepping){
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
    heart_rate = mean(rateValue);
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

