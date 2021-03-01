#include "watch.h"

DFRobot_Heartrate heartrate(DIGITAL_MODE);

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
	int16_t accX, accY, accZ, totalAcc;
	
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
