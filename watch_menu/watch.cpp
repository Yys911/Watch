#include "watch.h"  //add the header file made for the product

DFRobot_Heartrate heartrate(DIGITAL_MODE); //set the heart rate module as a global variable

/////////// screen module ///////////////// 
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // set up the tft display
SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;
uint8_t menuPage = 0;            // set start up menu page to 0

///////////////// set up Wi-Fi ////////////////////
WiFiClientSecure net = WiFiClientSecure(); // create secure client
MQTTClient client = MQTTClient(256);       // create MQTT client



uint16_t mean_val(uint16_t* nums) {
    
    int sum = 0;
    for (int i = 0; i < 20; i++) {
        sum = nums[i] + sum;
    }
    uint16_t mean = sum / 20;
    return mean;
}

/////////////// SERVER CONNECTIONS /////////////////////
void connectAWS()
{
    //set wifi and start it
    WiFi.mode(WIFI_STA);                   
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);   
    Serial.println("Connecting to Wi-Fi");

    //check it has connected and wait for it to connect
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

    // Check the device is connected to the AWS Client if not wait for it
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

void publishMessage(String Data)
{
    /*StaticJsonDocument<200> doc;
    doc["time"] = millis();
    doc["sensor_a0"] = analogRead(34);
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer); // print to client*/
    
    //re-check that the Wi-Fi and the aws is connected and wait for them to be before trying to send the packet to the server
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    while (!client.connected()) {
        client.connect(THINGNAME);
        Serial.print(".");
        delay(100);
    }

    //publish the packet in the correct format for the server to understand
    //print in the serial monitor if it works or say it didnt work
    if(client.publish(AWS_IOT_PUBLISH_TOPIC, "{\n\"packet\":\"12346" + Data + "\"\n}")){
      Serial.print("Publishing message : {\n\"packet\":\"12346" + Data + "\"\n}");
      //Serial.println("12346" + Data);
    }else{
      Serial.print("publish Failed!");
    }
    //client.loop();
}

void messageHandler(String& topic, String& payload) {
    // print on serial monitor that there is an incoming message
    Serial.println("incoming: " + topic + " - " + payload);

    //setup and display on the screen
    tft.setTextWrap(true);
    tft.setRotation(3);               //make screen teh right orientation
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 30);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(3);
    tft.println(payload);

    
    feedback();                       //set buzzer and motor off for 1 sec
    delay(3000);                       // wait for 4 second with feedback delay, so user can read notification
    tft.fillScreen(ST77XX_BLACK);
    //  StaticJsonDocument<200> doc;
    //  deserializeJson(doc, payload);
    //  const char* message = doc["message"];
}

void client_loop(){
  //client loop
  //check client for publish or subscribe messages and handle them
  client.loop();
}

/////////////// BUZZER/MOTOR FEEDBACK  /////////////////////
void init_feedback(){
  //setup the buzzer and motor pins as outputs
  pinMode(BUZZER, OUTPUT);
  pinMode(MOTOR, OUTPUT);
}

void feedback(){
  //set the buzzer and vibrator off at once for 1 second
  digitalWrite(BUZZER, HIGH);   // turn the BUZZER on (HIGH is the voltage level)
  digitalWrite(MOTOR, HIGH);   // turn the VIBRATION MOTOR on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(BUZZER, LOW);    // turn the BUZZER off by making the voltage LOW
  digitalWrite(MOTOR, LOW);    // turn the VIBRATION MOTOR off by making the voltage LOW
}

/*
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
}*/
///////////////=========== DATA HANDELING ================/////////////////////
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

/////////////// STEP TRACKING  /////////////////////

void Data_monitoring::init_step_tracker() {
    //set up the accelerometer to start sending over I2C
    Wire.beginTransmission(MPU);	//start transmission to accelerometer
    Wire.write(0x6B); 				//reset accelerometer
    Wire.write(0);
    Wire.endTransmission(true);		//end transsmission
    stepping = false; // for knowing if you ar ein the middle of a step or not
    stepCount = 0;
}

void Data_monitoring::step_tracker() {
    //start communication with teh acellerometer
    Wire.beginTransmission(MPU);	//begin i2c transsmoion from accelerometer
    Wire.write(0x3B);  				//go to the regeister where where data stored
    Wire.endTransmission(false);	//reset transsmission just incase
    Wire.requestFrom(MPU, 6, true);  	//request 6 bytes from MPU(accelereometer

    //put the acceleromet values in the variables
    int16_t accX = Wire.read() << 8 | Wire.read();//read both x-axis accerlerometer bytes and put them together to be 16 bits
    int16_t accY = Wire.read() << 8 | Wire.read();//same for y-axis
    int16_t accZ = Wire.read() << 8 | Wire.read();//same for z-axis
    int16_t totalAc = sqrt(accX * accX + accY * accY + accZ * accZ);//calc magnitude of the acceleration 

    //check if the user is stepping by having a hystorysis switch which increases the step count
    if (totalAc > 21000 && !stepping) {	//hystorisis switching points for steps
        stepping = true;
        stepCount++;
    }
    else if (totalAc < 15000 && stepping) {
        stepping = false;
    }

    //Serial.println(stepCount);		//print step count to monitor for debuging
    //Serial.println(totalAc);    //print step count to monitor for debuging
    //return stepCount;				//return value
}

/////////////// HEART RATE TRACKING  /////////////////////

void Data_monitoring::heartrate_tracker() {
    
    /*uint16_t rateValue[20];
    for (int i = 0; i < 20; i++) {
        heartrate.getValue(heartrate_pin);    //A0 foot sampled values
        rateValue[i] = heartrate.getRate();    //Get heart rate value
    }
    heart_rate = mean_val(rateValue);*/

    //get heart rate and remove 0 values
    uint8_t rateValue;
    heartrate.getValue(heartrate_pin);
    rateValue = heartrate.getRate(); // Get heart rate value 
    if(rateValue){                   // only update the values if they are not 0, to remove false readings
      Serial.println(rateValue);
      heart_rate=rateValue;
    } 
}

/////////////// CALCULATIONS  /////////////////////
void Data_monitoring::calculating() {
    //possible calculation to send to the server not needed in the end
    calories_burning = (Weight / 2000) * stepCount;
    fat_burning = calories_burning / 18;
    distance = Stride * stepCount / 100000;
}

void Data_monitoring::whether_sleeping() {
    //values if we were to use it as a sleep monitor, not implemented
    if (50 < heart_rate < 60 && !stepping) {
        sleeping = true;
    }
}

////////////// TIME ///////////////////////////////////

void time_sync_loop()
{   
    struct tm timeinfo;
    timeval tv;
    timezone tz;
    timezone tz_utc = {0,0};

    // Set a fixed time of 2020-09-26 00:00:00, UTC
    tv.tv_sec = time_t(1601078400);
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;
    settimeofday(&tv, &tz_utc);

    // Get the time and print it
    time_t now = time(nullptr);
    gmtime_r(&now, &timeinfo);

    Serial.print("Function: ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void Data_monitoring::printLocalTime(){
  //get local time info through wifi
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  /*Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();*/

  //update time variables
  strftime(hour,3, "%H", &timeinfo);    //save the time variables to display
  strftime(minuet,3, "%M", &timeinfo);
  strftime(day,3, "%d", &timeinfo);
  strftime(month,10, "%B", &timeinfo);  // alphabetic month name
  strftime(month_d,3, "%m", &timeinfo); // numeric month name
  strftime(year,5, "%Y", &timeinfo);
}

/////////////// SCREEN HANDELING  /////////////////////

//interrupt to navigate on menu pages between values 0-4
void IRAM_ATTR isrMenuUp() {
  //interrupt routine to make the menu page go up
  if(menuPage<4){
    menuPage ++;
  }
  SerialMonitorInterface.println(menuPage);
  delay(50);
}
void IRAM_ATTR isrMenuDown() {
  //interrupt routine to make the menu page go down
  if(menuPage>0){
    menuPage --;
  }
  SerialMonitorInterface.println(menuPage);
  delay(50);
}

void Data_monitoring::init_screen() {

    //setup the screen
    tft.init(SCREEN_HEIGHT, SCREEN_WIDTH);           // Init ST7789 320x240
    
    //format the loading screen 
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextWrap(false);
    tft.setRotation(ROTATION);               //make screen teh right orientation
    //tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(20, 30);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(4);
    tft.print("WORK");
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(4);
    tft.print("-OUT");
    tft.setCursor(20, 80);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(4);
    tft.print("FROM");
    tft.setCursor(20, 130);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(4);
    tft.print("HOME");
   
    //initiate the sd reader
    ImageReturnCode stat; // Status from image-reading functions
    if(!SD.begin(SD_CS, SD_SCK_MHZ(10))) { // Breakouts require 10 MHz limit due to longer wires
      SerialMonitorInterface.println(F("SD begin() failed"));
      for(;;); // Fatal error, do not continue
    }
    SerialMonitorInterface.println(F("OK!"));
}

void Data_monitoring::display_data() {

    //initiate the sd card
    ImageReturnCode stat; // Status from image-reading functions

    //menu switch case
    switch (menuPage){
      //clock screen
      case 0:    
        printLocalTime();
        //if any printed value has changed update the screen
        if(minuet[0] != prev_minuet[0] || minuet[1] != prev_minuet[1]|| minuet[2] != prev_minuet[2] || menuPage != prev_page ){
          for(int i=0; i<3; i++){
            prev_minuet[i] = minuet[i];
          }
          prev_page=menuPage;
          tft.fillScreen(ST77XX_BLACK);
          tft.setTextWrap(false);
          tft.setRotation(ROTATION);               //make screen teh right orientation
          tft.setCursor(60, 20);
          tft.setTextColor(ST77XX_WHITE);
          tft.setTextSize(3);
          tft.println("Date");
          tft.setCursor(50, 50);
          tft.print( day);
          tft.print(" - ");
          tft.println( month);
          tft.setTextSize(3);
          tft.setCursor(60, 80);
          tft.println("Time");
          tft.setTextSize(5);
          tft.setCursor(50, 120);
          tft.print( hour);
          tft.print(" : ");
          tft.println( minuet);
          
        }
        break;
      // heart rate screen with first picture 
      case 1:
        // only update the screen when values change
        if((heart_rate != prev_heart_rate )|| (menuPage != prev_page )){
          prev_heart_rate = heart_rate;
          prev_page=menuPage;
          detachInterrupt(MENU_UP);
          detachInterrupt(MENU_DOWN);
          Serial.print(F("Loading cat1.bmp to screen..."));
          stat = reader.drawBMP("/cat1.bmp", tft, 0, 0);
          reader.printStatus(stat);   // How'd we do?
          attachInterrupt(MENU_UP, isrMenuUp, FALLING);
          attachInterrupt(MENU_DOWN, isrMenuDown, FALLING);
          tft.setTextWrap(false);
          tft.setRotation(ROTATION);               //make screen teh right orientation
          //tft.fillScreen(ST77XX_BLACK);
          tft.setCursor(85, 50);
          tft.setTextColor(ST77XX_BLACK);
          tft.setTextSize(2);
          tft.println("BPM = ");
          if(heart_rate){
            tft.setCursor(85, 70);
            tft.println(heart_rate);
          }else{
            tft.setCursor(85, 70);
            tft.println("---");
          }
        }
        break;
      // heart rate screen with second picture
      case 2:
        // only update the screen when values change
        if((heart_rate != prev_heart_rate )|| (menuPage != prev_page )){
          prev_heart_rate = heart_rate;
          prev_page=menuPage;
          detachInterrupt(MENU_UP);
          detachInterrupt(MENU_DOWN);
          Serial.print(F("Loading cat2.bmp to screen..."));
          stat = reader.drawBMP("/cat2.bmp", tft, 0, 0);
          reader.printStatus(stat);   // How'd we do?
          attachInterrupt(MENU_UP, isrMenuUp, FALLING);
          attachInterrupt(MENU_DOWN, isrMenuDown, FALLING);
          tft.setTextWrap(false);
          tft.setRotation(ROTATION);               //make screen teh right orientation
          //tft.fillScreen(ST77XX_BLACK);
          tft.setCursor(200, 30);
          tft.setTextColor(ST77XX_BLACK);
          tft.setTextSize(2);
          tft.println("BPM = ");
          if(heart_rate){
            tft.setCursor(200, 50);
            tft.println(heart_rate);
          }else{
            tft.setCursor(200, 50);
            tft.println("---");
          }
        }
        break;
      //menu screen for showing steps taken 
      case 3:
        //only update teh screen when steps change
        if(stepCount != prev_steps || menuPage != prev_page ){
          prev_steps = stepCount;
          prev_page=menuPage;
          tft.fillScreen(ST77XX_BLACK);
          tft.setTextWrap(false);
          tft.setRotation(ROTATION);               //make screen teh right orientation
          //tft.fillScreen(ST77XX_BLACK);
          tft.setCursor(20, 30);
          tft.setTextColor(ST77XX_WHITE);
          tft.setTextSize(3);
          tft.println("Step Taken = ");
          tft.setCursor(20, 60);
          tft.setTextSize(5);
          if(stepCount){
            tft.println(stepCount);
          }else{
            tft.println("---");
          }
        }
        break;
      // menu for heart rate without picture
      case 4:
        if(heart_rate != prev_heart_rate || menuPage != prev_page ){
          prev_heart_rate = heart_rate;
          prev_page=menuPage;
          tft.fillScreen(ST77XX_BLACK);
          tft.setTextWrap(false);
          tft.setRotation(ROTATION);               //make screen the right orientation
          //tft.fillScreen(ST77XX_BLACK);
          tft.setCursor(0, 30);
          tft.setTextColor(ST77XX_RED);
          tft.setTextSize(3);
          tft.println("Heart rate = ");
          if(heart_rate){
            tft.println(heart_rate);
          }else{
            tft.println("---");
          }
          tft.println("page = ");
          tft.println(menuPage);
        }
        break;
      //screen if the menu varible somehow goes out of range of expected values
      default:
        if(heart_rate != prev_heart_rate || menuPage != prev_page ){
          prev_heart_rate = heart_rate;
          prev_page=menuPage;
          tft.fillScreen(ST77XX_BLACK);
          tft.setTextWrap(false);
          tft.setRotation(ROTATION);               //make screen teh right orientation
          //tft.fillScreen(ST77XX_BLACK);
          tft.setCursor(0, 30);
          tft.setTextColor(ST77XX_RED);
          tft.setTextSize(3);
          tft.println("Heart rate = ");
          if(heart_rate){
            tft.println(heart_rate);
          }else{
            tft.println("---");
          }
          tft.println("page = ");
          tft.println(menuPage);
        }
        break;
    }

    //prev_heart_rate = heart_rate;
    //prev_page=menuPage;


}
