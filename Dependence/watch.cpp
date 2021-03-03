#include "watch.h"
#define Weight 60
#define Height 170
#define Stride 60

const unsigned int time = 600000;
unsigned long nowtime = 0;

DFRobot_Heartrate heartrate(DIGITAL_MODE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


uint16_t mean_val(uint16_t* nums) {
    
    int sum = 0;
    for (int i = 0; i < 20; i++) {
        sum = nums[i] + sum;
    }
    uint16_t mean = sum / 20;
    return mean;
}

void callback(char* topic, byte* payload, unsigned int length) {
    //v_b(payload);
}

void send(Data_monitoring data_moni) {

    nowtime = millis();
    if (nowtime == time) {
        mqttClient.publish("Heart_Rate", data_moni.heart_rate);
        mqttClient.publish("Step_Count", data_moni.stepCount);
        mqttClient.publish("Calories_Burning", data_moni.calories_burning);

    }
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

void Data_monitoring::init_screen() {
    
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    // Initialze SSD1306 OLED display
}

void Data_monitoring::init_step_tracker() {
    
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

void Data_monitoring::calculating() {

    calories_burning = (Weight / 2000) * stepCount;
    fat_burning = calories_burning / 18;
    distance = Stride * stepCount / 100000;
}

void Data_monitoring::whether_sleeping() {
    
    if (50 < heart_rate < 60 && !stepping) {
        sleeping = true;
    }
}

void Data_monitoring::display_data() {
    
    display.clearDisplay();              // Clear screen
    display.setCursor(0, 0);              // Set cursor position
    display.print("Heart rate: "); display.print(heart_rate); display.print(" bpm");
    display.setCursor(0, 20);
    display.print("Steps: "); display.print(stepCount);
    display.setCursor(0, 40);
    display.print("Calories buring: "); display.print(calories_burning); display.print(" kcal");
    display.display();
}


void Server_related::connectWifi(const char* ssid, const char* password) {
    
    WiFi.setPins(8, 2, A3, -1);
    while (!SerialMonitorInterface);

    // Attempt to connect to Wifi network:
    display.setCursor(0, 0);
    display.print("Connecting Wifi: ");
    display.setCursor(0, 20);
    display.print(ssid);
    display.display();

    // Connect to WiFi, and loop until connection is secured
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        display.print(".");
        display.display();
        delay(500);
    }
    display.clearDisplay();
}

void Server_related::connectMQTTServer(const char* mqttServer) {
    mqttClient.setServer(mqttServer, 1883);
    if (mqttClient.connect("Watch")) {
        Serial.println("MQTT Server Connected.");
    }
    else {
        Serial.print("MQTT Server Connect Failed. Client State:");
        Serial.println(mqttClient.state());
        delay(3000);
    }
    mqttClient.setCallback(callback);
}

void Server_related::loop(void) {
    mqttClient.loop();
}

void Server_related::v_b(byte* sensor) {
    
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
