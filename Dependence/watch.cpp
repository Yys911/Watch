#include "watch.h"

DFRobot_Heartrate heartrate(DIGITAL_MODE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);


uint16_t mean_val(uint16_t* nums) {
    
    int sum = 0;
    for (int i = 0; i < 20; i++) {
        sum = nums[i] + sum;
    }
    uint16_t mean = sum / 20;
    return mean;
}

void connectAWS()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Configure WiFiClientSecure to use the AWS IoT device credentials
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);

    // Create a message handler
    client.onMessage(messageHandler);

    Serial.print("Connecting to AWS IOT");

    while (!client.connect(THINGNAME)) {
        Serial.print(".");
        delay(100);
    }

    if (!client.connected()) {
        Serial.println("AWS IoT Timeout!");
        return;
    }

    // Subscribe to a topic
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

    Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
    StaticJsonDocument<200> doc;
    doc["time"] = millis();
    doc["sensor_a0"] = analogRead(34);
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to client

    client.publish(AWS_IOT_PUBLISH_TOPIC, "Hello World!");
    Serial.println("Publishing message");
}

void messageHandler(String& topic, String& payload) {
    Serial.println("incoming: " + topic + " - " + payload);

    //  StaticJsonDocument<200> doc;
    //  deserializeJson(doc, payload);
    //  const char* message = doc["message"];
}

void v_b(byte* sensor) {

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
