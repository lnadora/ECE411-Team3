#include "DHT.h"                            //Including "DHT.h"
#include <Wire.h>                           //Including "Wire.h"
#include "WiFi.h"                           //Including "WiFi.h"
#include "SPIFFS.h"                         //Including "SPIFFS.h"
#include <Arduino.h>                        //Including "Arduino.h"
#include <U8g2lib.h>                        //Including "Arduino.h"
#include <AsyncTCP.h>                       //Including "AsyncTCP.h"
#include <WiFiManager.h>                    //Including "WiFiManager.h"
#include <CheapStepper.h>                   //Including "CheapStepper.h"
#include <Arduino_JSON.h>                   //Including "Arduino_JSON.h"
#include <Adafruit_Sensor.h>                //Including "Adafruit_Sensor.h"
#include <ESPAsyncWebServer.h>              //Including "ESPAsyncWebServer.h"
#define IN1               19                //Stepper Motor Pin Setting
#define IN2               18                //Stepper Motor Pin Setting
#define IN3               5                 //Stepper Motor Pin Setting
#define IN4               17                //Stepper Motor Pin Setting
#define DHTPIN            27                //Dht11 Pin Setting
#define buttonA           14                //Button Pin Setting
#define buttonB           26                //Button Pin Setting
#define buttonC           25                //Button Pin Setting
#define buttonD           33                //Button Pin Setting
#define DHTTYPE           DHT11             //Dht11 Pin
#define tempGetTimeMax    2000              //Dht11 Time of getting temperature
#define printTimeMax      500               //Serial print time
#define upTimeMax         500               //Uploading Web time
#define showIPTimeMax     3000              //Time of displaying IP
//#define temCheckTimeMax 120000            //Check Temp 120s
#define temCheckTimeMax   200               //Temperature check time for test 200ms
#define subtraction       0.5               //Add number
#define addend            0.5               //Reduce number

float setValue              = 20;           //Initial setting temperature

unsigned long upTime        = 0;            //Update data time variable
unsigned long printTime     = 0;            //Serial printing time variable
unsigned long showIPTime    = 0;            //ip time variable
unsigned long tempGetTime   = 0;            //Get temperature time variable
unsigned long temCheckTime  = 0;            //Temperature check time variable
float hum                   = 20;           //humidity
float tem                   = 20;           //Temp
int   position              = 0;            //Switch position
bool  sysOpenFlag           = true;         //System start temperature check control
bool  showIPFlag            = false;        //ip communication flag
bool  moveDir               = true;         //Step motor direction
const char* PARAM_MESSAGE   = "message";    //setValue Information
DHT dht(DHTPIN, DHTTYPE);                   //dht11 setting
AsyncWebServer   server(80);                //Create AsyncWebServer object on port 80
AsyncEventSource events("/events");         //Create event source/events
CheapStepper stepper (IN1, IN2, IN3, IN4);  //Stepper motor pin setting
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   //OLED
struct Button {
  int buttonState     = LOW;                //Button state variables
  int lastButtonState = HIGH;               //Button state initialization
  int buttonPushNum   = 0;                  //Record the number of button presses
  boolean flag;                             //Judgment mark
  long lastDebounceTime = 0;                //Record jitter variables
  long debounceDelay    = 50;               //Jitter time variable 50ms
};
Button button[5];                           //Create a new button

void initSPIFFS() {                                     //Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");//fail
  }
  Serial.println("SPIFFS mounted successfully");        //success
}
void initWiFi() {                                       //Initialize WiFi
  WiFi.mode(WIFI_STA);                                  //STA mode
  WiFiManager wm;                                      
  //wm.resetSettings();                                 //Reset wifi connection information
  bool res;                                             //Get connection status
  res = wm.autoConnect("TemCtrl", "12345678");          //wifi name password
  if (!res) {
    Serial.println("Failed to connect");                //No connect
    // ESP.restart();
  }
  else {
    Serial.println("connected...yeey :)");              //Connection complete
    Serial.println(WiFi.localIP());                     //WiFiip serial printing
    showIPFlag = true;                                  //Show ip once
    dht.begin();                                        //dht11 initialization
    server.addHandler(&events);                         //Connect event source and server
    server.begin();                                     //Start the server
  }
}
void oledShow(float tem, float set) {                   //Oled display
  u8g2.clearBuffer();                                      
  u8g2.drawStr(0, 10, "Temp:");                                      
  u8g2.setCursor(70, 10);                                          
  u8g2.print(String(tem));                                                    
  u8g2.drawStr(0, 20, "Set Value:");                     
  u8g2.setCursor(70, 20);                                                                
  u8g2.print(String(set));                                                                 
  if (sysOpenFlag == true) {                                                     
    u8g2.drawStr(0, 30, "SystemState: ON");                                      
  }                                                                            
  if (sysOpenFlag == false) {                                                    
    u8g2.drawStr(0, 30, "SystemState: OFF");                                         
  }                                                                            
  u8g2.sendBuffer();                                                                           
}                                                                                    
void oledInit() {                                       //Oled initialization display                   
  u8g2.clearBuffer();                                                        
  u8g2.setFont(u8g2_font_ncenB08_tr);                                          
  u8g2.drawStr(0, 10, "Temp:      ");                                                                          
  u8g2.drawStr(0, 20, "Set Value: ");           
  u8g2.drawStr(0, 30, "SystemState: Start");               
  u8g2.sendBuffer();
}
void stepRun(int angle) {                               // -360° - 0 - 360°
  int   turnAngle = 0;                                  //Rotation angle
  if (angle >= 0 && angle < 360) {
    turnAngle = map(angle, 0, 360, 0, 4095);
    moveDir = true;
  }
  if (angle >= -360 && angle < 0) {
    turnAngle = map(angle, -360, 0, 4095, 0);
    moveDir = false;
  }
  for (int s = 0; s < turnAngle; s++) {
    stepper.step(moveDir);
  }
}
void oledShowIp() {                                     //Show IP
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "IP:");
  u8g2.setCursor(32, 10);
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
}
void keyCheck(int button_Pin, int button_num) {
  int reading = digitalRead(button_Pin);                             //Read the button state and store it in a variable
  if (reading != button[button_num].lastButtonState) {               //Check whether the button status has changed
    button[button_num].lastDebounceTime = millis();                  //If it changes, update the time
  }
  if ((millis() - button[button_num].lastDebounceTime) > button[button_num].debounceDelay) {
    if (reading != button[button_num].buttonState) {                 //If the button status changes:
      button[button_num].buttonState = reading;                      //Record key status
      if (button[button_num].buttonState == LOW) {                   //Change the LED state only when the button state is stable
        button[button_num].buttonPushNum++;                          //At this time, the number of keys is increased automatically
        button[button_num].flag = true;                              //Record the key state to the key flag flag
        if (button_Pin == buttonA) {                               //Go forward and detect current after 200ms
          sysOpenFlag = true;                                           //Change state
          u8g2.clearBuffer();
          u8g2.drawStr(0, 10, "Temp:");                                      
          u8g2.setCursor(70, 10);                                          
          u8g2.print(String(tem));                                                    
          u8g2.drawStr(0, 20, "Set Value:");                     
          u8g2.setCursor(70, 20);                                                                
          u8g2.print(String(setValue)); 
          u8g2.drawStr(0, 30, "                 ");                                                                                                         
          u8g2.sendBuffer(); 
          u8g2.drawStr(0, 30, "SystemState: ON  ");                      //OLED  ON                                                                                   
          u8g2.sendBuffer();  
          Serial.println("A");
          char buf[20];
          sprintf(buf, "%s", "System State: ON");                        //web   ON
          events.send(buf, "systemstate", millis());
        }
        if (button_Pin == buttonB) {                                     //Go forward and detect current after 200ms
          sysOpenFlag = false;                                           //Switch state
          u8g2.clearBuffer();
          u8g2.drawStr(0, 10, "Temp:");                                      
          u8g2.setCursor(70, 10);                                          
          u8g2.print(String(tem));                                                    
          u8g2.drawStr(0, 20, "Set Value:");                     
          u8g2.setCursor(70, 20);                                                                
          u8g2.print(String(setValue)); 
          u8g2.drawStr(0, 30, "SystemState: OFF ");                      //OLED OFF                                                                                   
          u8g2.sendBuffer();   
          Serial.println("B");
          char buf[20];
          sprintf(buf, "%s", "System State: OFF");                       //web  OFF
          events.send(buf, "systemstate", millis());
          if (position == 1 ) {                                          //When pos is at 1 (the air conditioner is on), turn off the air conditioner.
            position = 0;                                                //Update switch position
            stepRun(-180);
            stepRun(-180);
            stepRun(-180);
            stepRun(-180);                                               //Turn off heating
          }
        }
        if (button_Pin == buttonC) {
          Serial.println("C");
          setValue -= subtraction;                                       //Set temperature -0.5
          char buf[6];
          sprintf(buf, "%s", String(setValue));                          //Conversion variable
          events.send(buf, "temptag", millis());                         //Upload set temperature
        }
        if (button_Pin == buttonD) {
          Serial.println("D");
          setValue += addend;                                            //Set temperature +0.5
          char buf[6];
          sprintf(buf, "%s", String(setValue));                          //Conversion variable
          events.send(buf, "temptag", millis());                         //Upload set temperature
        }
      }
      else {
        //button[button_num].flag = false;                               //If the key is not changed, then the key state is recorded as false
      }
    }
  }
  button[button_num].lastButtonState = reading;                          //Save the processing result:
}

void setup() {
  Serial.begin(115200);                                 //Initialize the baud rate to 115200
  pinMode(buttonA,INPUT_PULLUP);                        //Pin setting input pull-up
  pinMode(buttonB,INPUT_PULLUP);                        //Pin setting input pull-up
  pinMode(buttonC,INPUT_PULLUP);                        //Pin setting input pull-up
  pinMode(buttonD,INPUT_PULLUP);                        //Pin setting input pull-up
  u8g2.begin();                                         //oled initialization
  oledInit();                                           //oled display initialization
  initWiFi();                                           //Initialize WIFI
  initSPIFFS();                                         //Initialize the file system
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {         //Handling the web server
    request->send(SPIFFS, "/index.html", "text/html");                   //Connect SPIFFS and index.html
  });
  server.serveStatic("/", SPIFFS, "/");
  server.on("/showip", HTTP_GET, [](AsyncWebServerRequest * request) {   //Create an address command display ip
    showIPTime = millis();
    showIPFlag = true;                                                   //IPshow
    Serial.println("showIP");
    request->send(200, "text/plain", "OK");
  });
  server.on("/open", HTTP_GET, [](AsyncWebServerRequest * request) {     //Create an address
    sysOpenFlag = true ;
    char buf[20];
    sprintf(buf, "%s", "System State: ON");                              //ON
    events.send(buf, "systemstate", millis());
    request->send(200, "text/plain", "OK");
  });
  server.on("/close", HTTP_GET, [](AsyncWebServerRequest * request) {    //Create an address
    sysOpenFlag = false ;
    if (position == 1 ) {                                          //When pos is at 1 (the air conditioner is on), turn off the air conditioner.
        position = 0;                                                //Update switch position
        stepRun(-180);
        stepRun(-180);
        stepRun(-180);
        stepRun(-180);                                               //Off heater
    }
    char buf[20];
    sprintf(buf, "%s", "System State: OFF");                             //OFF
    events.send(buf, "systemstate", millis());
    request->send(200, "text/plain", "OK");
  });
  server.on("/setvalue", HTTP_GET, [](AsyncWebServerRequest * request) { //Create an address
    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
      //Serial.println(request->getParam(PARAM_MESSAGE))->value());
      message = request->getParam(PARAM_MESSAGE)->value();
    } else {
      message = "No message sent";
    }
    setValue = message.toFloat();
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Temp:");                                      
    u8g2.setCursor(70, 10);                                          
    u8g2.print(String(tem));                                                    
    u8g2.drawStr(0, 20, "Set Value:");                     
    u8g2.setCursor(70, 20);                                                                
    u8g2.print(String(setValue)); 
    if(sysOpenFlag == true ){
      u8g2.drawStr(0, 30, "SystemState: OFF ");    
    }
    if(sysOpenFlag == false ){
      u8g2.drawStr(0, 30, "SystemState: OFF ");    
    }
    u8g2.sendBuffer();   
    if (sysOpenFlag == true && (!isnan(tem))) {                                //When the ip and the number of readings are not displayed, refresh the oled
        if (position == 0 && tem < setValue) {                                    //When pos is at 0 (tentative air-conditioning is off) and the real-time temperature is lower than the set temperature. When it's cold, turn on the heating. (Turn off the air conditioner, no heating)
          position = 1;                                                           //Update switch position
          stepRun(180);
          stepRun(180);
          stepRun(180);                                                           //Turn on heating
          stepRun(180);
        }
        if (position == 1 && tem > setValue) {                                    //When pos is at 1 (tentative air-conditioning is turned on) and the real-time temperature is greater than the set temperature. It's hot, turn off the air conditioner. (Turn on the air conditioner, heating)
          position = 0;                                                           //Update switch position
          stepRun(-180);
          stepRun(-180);
          stepRun(-180);
          stepRun(-180);                                                        //Turn off heating
        }
    }
    Serial.println(message);
    request->send(200, "text/plain", "OK");
  });
  events.onConnect([](AsyncEventSourceClient * client) {              //Web service events
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });

  upTime = millis();                                     //Update time
  temCheckTime = millis();                               //Temperature inspection time
  tempGetTime  = millis();                               //Temperature time
  showIPTime   = millis();                               //ip time
  printTime    = millis();                               //printing time
}

void loop() {                  
  keyCheck(buttonA, 1);                   //Button detection
  keyCheck(buttonB, 2);                   //Button detection                         
  keyCheck(buttonC, 3);                   //Button detection
  keyCheck(buttonD, 4);                   //Button detection
  if (showIPFlag == true) {                                                     //Show ip command get
    oledShowIp();                                                               //OLED Display
  }
  if ( (showIPFlag == true) && ((millis() - showIPTime) > showIPTimeMax  )) {   //ip display finished
    showIPTime = millis();                                                      //Update time
    showIPFlag = false;                                                         //Reset the display flag
    oledShowIp();                                                               //Screen display IP
  }
  if ((millis() - upTime) > upTimeMax) {                                      //Upload data to the web
    upTime = millis();                                                        //Update time
    char buf[5], buf2[5];                                                     //New char[]
    if (!isnan(tem)) {                                                        //Temperature is not empty
      sprintf(buf, "%s", String(tem));                                        //int to char[]
      events.send(buf, "temp", millis());                                     //Upload real-time temperature
    }                                                                         
    delay(100);                                                               //Delay for a while
    sprintf(buf2, "%s", String(setValue));                                    //Conversion variable
    events.send(buf2, "temptag", millis());                                   //Upload target temperature
  }
  if (millis() - tempGetTime >= tempGetTimeMax) {                             //Get temperature data
    tempGetTime = millis();                                                   //Update time
    tem = dht.readTemperature();                                              //Get temperature
    Serial.print(F("Temperature: "));                                         //Serial printing
    Serial.print(tem);Serial.println(F("°C "));
    if (showIPFlag != true && (!isnan(tem))) {                                //When the ip and the number of readings are not displayed, refresh the oled
      oledShow(tem, setValue);
    }
  }
  //The system starts to detect whether the temperature is over/under the temperature
  //pos 0 1 Default 0 Heating air conditioner is not turned on 0 is heating air conditioner'off' position 1 is heating air conditioner'on' position
  if (sysOpenFlag == true) {
    //When the system is tentatively turned on, it will be checked every 2 minutes, because turning on and off the air conditioner does not immediately change the temperature
    if (millis() - temCheckTime >= temCheckTimeMax) {                           //Check once every 2 minutes

      temCheckTime = millis();                                                  //Update time
      if (sysOpenFlag == true && (!isnan(tem))) {                                //When the ip and the number of readings are not displayed, refresh the oled
        if (position == 0 && tem < setValue-1) {                                  //When pos is at 0 (tentative air-conditioning is off) and the real-time temperature is lower than the set temperature. When it's cold, turn on the heating. (Turn off the air conditioner, no heating)
          position = 1;                                                           //Update switch position
          stepRun(180);
          stepRun(180);
          stepRun(180);
          stepRun(180);                                                           //Turn on heating
        }
        if (position == 1 && tem > setValue+1) {                                    //When pos is at 1 (tentative air-conditioning is turned on) and the real-time temperature is greater than the set temperature. It's hot, turn off the air conditioner. (Turn on the air conditioner, heating)
          position = 0;                                                           //Update switch position
          stepRun(-180);
          stepRun(-180);                                                          //Turn off heating
          stepRun(-180);
          stepRun(-180);
        }
      }
    }
  }
  
}
