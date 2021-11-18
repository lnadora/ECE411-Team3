#include "DHT.h"                            //包含"DHT.h"头文件
#include <Wire.h>                           //包含"Wire.h"头文件
#include "WiFi.h"                           //包含"WiFi.h"头文件
#include "SPIFFS.h"                         //包含"SPIFFS.h"头文件
#include <Arduino.h>                        //包含"Arduino.h"头文件
#include <U8g2lib.h>                        //包含"Arduino.h"头文件
#include <AsyncTCP.h>                       //包含"AsyncTCP.h"头文件
#include <WiFiManager.h>                    //包含"WiFiManager.h"头文件
#include <CheapStepper.h>                   //包含"CheapStepper.h"头文件
#include <Arduino_JSON.h>                   //包含"Arduino_JSON.h"头文件
#include <Adafruit_Sensor.h>                //包含"Adafruit_Sensor.h"头文件
#include <ESPAsyncWebServer.h>              //包含"ESPAsyncWebServer.h"头文件 nclude "ESPAsyncWebServer.h" header file
#define IN1               19                //步进电机控制引脚   Stepper motor control pin
#define IN2               18                //步进电机控制引脚   Stepper motor control pin
#define IN3               5                 //步进电机控制引脚   Stepper motor control pin
#define IN4               17                //步进电机控制引脚   Stepper motor control pin
#define DHTPIN            27                //dht11控制引脚     dht11 control pin
#define buttonA           33                //按键引脚          Key pin
#define buttonB           26                //按键引脚          Key pin
#define buttonC           25                //按键引脚          Key pin
#define buttonD           14                //按键引脚          Key pin
#define DHTTYPE           DHT11             //dht11引脚         dht11 pin
#define tempGetTimeMax    2000              //dht11获取温度时间   dht11 get temperature time
#define printTimeMax      500               //串口打印时间        Serial print time
#define upTimeMax         500               //上传web时间        Upload web time 
#define showIPTimeMax     3000              //有IP网络时候的显示时间  Display time when there is an IP network
//#define temCheckTimeMax 120000            //温度检查时间120s      Temperature check time 120s
#define temCheckTimeMax   200               //测试用 温度检查时间 200ms Temperature check time for test 200ms
float setValue              = 20;           //初始设置温度        Initial setting temperature

unsigned long upTime        = 0;            //更新数据时间变量    Update data time variable
unsigned long printTime     = 0;            //串口打印时间变量    Serial printing time variable
unsigned long showIPTime    = 0;            //ip时间变量          ip time variable
unsigned long tempGetTime   = 0;            //获取温度时间变量    Get temperature time variable
unsigned long temCheckTime  = 0;            //温检时间变量        Temperature check time variable
float hum                   = 20;           //湿度                humidity
float tem                   = 20;           //温度                temperature
int   position              = 0;            //开关位置            Switch position
bool  sysOpenFlag           = true;         //系统启动温检控制      System start temperature check control
bool  showIPFlag            = false;        //ip通信标志位          ip通信标志位
bool  moveDir               = true;         //步进电机方向        Step motor direction
const char* PARAM_MESSAGE   = "message";    //setValue信息        setValue information
DHT dht(DHTPIN, DHTTYPE);                   //dht11设定           dht11 settings
AsyncWebServer   server(80);                //在端口80上创建AsyncWebServer对象  Create AsyncWebServer object on port 80
AsyncEventSource events("/events");         //创建事件源/events     Create event source/events
CheapStepper stepper (IN1, IN2, IN3, IN4);  //步进电机引脚设定        Stepper motor pin settings
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   //OLED
struct Button {
  int buttonState     = LOW;                //按钮状态变量      Button state variables
  int lastButtonState = HIGH;               //按钮状态初始化    Button state initialization
  int buttonPushNum   = 0;                  //记录按钮按键次数  Record the number of button presses
  boolean flag;                             //判断标志          Judgment mark
  long lastDebounceTime = 0;                //记录抖动变量        Record jitter variables
  long debounceDelay    = 50;               //抖动时间变量50ms    Jitter time variable 50ms
};
Button button[5];                           //新建1个按钮         Create a new button

void initSPIFFS() {                                     //初始化SPIFFS  Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");//失败  fail
  }
  Serial.println("SPIFFS mounted successfully");        //成功          success
}
void initWiFi() {                                       //初始化WiFi    Initialize WiFi
  WiFi.mode(WIFI_STA);                                  //STA模式       STA mode
  WiFiManager wm;                                       //声明          statement
  //wm.resetSettings();                                 //重置wifi连接信息  Reset wifi connection information
  bool res;                                             //获取连接情况    Get connection status
  res = wm.autoConnect("TemCtrl", "12345678");          //wifi 名称 密码  wifi name password
  if (!res) {
    Serial.println("Failed to connect");                //未连接        not connected
    // ESP.restart();
  }
  else {
    Serial.println("connected...yeey :)");              //连接完成          Connection complete
    Serial.println(WiFi.localIP());                     //WiFiip串口打印    WiFiip serial printing
    showIPFlag = true;                                  //Display once IP
    dht.begin();                                        //DHT11 initialization
    server.addHandler(&events);                         //Link event source and server
    server.begin();                                     //Start server
  }
}
void oledShow(float tem, float set) {                   //OLED display
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
void oledInit() {                                       //OLED initialization display                    
  u8g2.clearBuffer();                                                        
  u8g2.setFont(u8g2_font_ncenB08_tr);                                          
  u8g2.drawStr(0, 10, "Temp:      ");                                                                          
  u8g2.drawStr(0, 20, "Set Value: ");           
  u8g2.drawStr(0, 30, "SystemState: Start");               
  u8g2.sendBuffer();
}
void stepRun(int angle) {                               // -360° - 0 - 360°
  int   turnAngle = 0;                                  //Rotational angle
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
void oledShowIp() {                                     //Display IP
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "IP:");
  u8g2.setCursor(32, 10);
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
}
void keyCheck(int button_Pin, int button_num) {
  int reading = digitalRead(button_Pin);                             //Read the button status and store the variable
  if (reading != button[button_num].lastButtonState) {               //Check if the button status changes
    button[button_num].lastDebounceTime = millis();                  //If you change, update time
  }
  if ((millis() - button[button_num].lastDebounceTime) > button[button_num].debounceDelay) {
    if (reading != button[button_num].buttonState) {                 //If the button status changes:
      button[button_num].buttonState = reading;                      //Record button status
      if (button[button_num].buttonState == LOW) {                   //The LED status is changed only when the stable button status is
        button[button_num].buttonPushNum++;                          //At this point, the number of keys is added
        button[button_num].flag = true;                              //Record button status to button logo Flag
        if (button_Pin == buttonA) {                               //Advance and after 200 ms
          sysOpenFlag = true;                                           //Switch status
          u8g2.clearBuffer();
          u8g2.drawStr(0, 10, "Temp:");                                      
          u8g2.setCursor(70, 10);                                          
          u8g2.print(String(tem));                                                    
          u8g2.drawStr(0, 20, "Set Value:");                     
          u8g2.setCursor(70, 20);                                                                
          u8g2.print(String(setValue)); 
          u8g2.drawStr(0, 30, "                 ");                                                                                                         
          u8g2.sendBuffer(); 
          u8g2.drawStr(0, 30, "SystemState: ON  ");                                                                                                         
          u8g2.sendBuffer();  
          Serial.println("A");
          char buf[20];
          sprintf(buf, "%s", "System State: ON");                             //OFF
          events.send(buf, "systemstate", millis());
        }
        if (button_Pin == buttonB) {                               //Advance and after 200 ms
          sysOpenFlag = false;                                           //Switch status
          u8g2.clearBuffer();
          u8g2.drawStr(0, 10, "Temp:");                                      
          u8g2.setCursor(70, 10);                                          
          u8g2.print(String(tem));                                                    
          u8g2.drawStr(0, 20, "Set Value:");                     
          u8g2.setCursor(70, 20);                                                                
          u8g2.print(String(setValue)); 
          u8g2.drawStr(0, 30, "SystemState: OFF ");                                                                                                         
          u8g2.sendBuffer();   
          Serial.println("B");
          char buf[20];
          sprintf(buf, "%s", "System State: OFF");                             //OFF
          events.send(buf, "systemstate", millis());
        }
        if (button_Pin == buttonC) {
          Serial.println("C");
        }
        if (button_Pin == buttonD) {
          Serial.println("D");
        }
      }
      else {
        //button[button_num].flag = false;                             //If the button does not change, then the record button status is false.
      }
    }
  }
  button[button_num].lastButtonState = reading;                      // Save processing results:
}

void setup() {
  Serial.begin(115200);                                 //Initialization baud rate 115200
  pinMode(buttonA,INPUT_PULLUP);                        //Pin setting input pull
  pinMode(buttonB,INPUT_PULLUP);                        //Pin setting input pull
  pinMode(buttonC,INPUT_PULLUP);                        //Pin setting input pull
  pinMode(buttonD,INPUT_PULLUP);                        //Pin setting input pull
  u8g2.begin();                                         //OLED initialization
  oledInit();                                           //OLED display initialization
  initWiFi();                                           //Initialize WiFi
  initSPIFFS();                                         //Initialization file system
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {         //Handling web server
    request->send(SPIFFS, "/index.html", "text/html");                   //Connect SPIFFS and INDEX.html
  });
  server.serveStatic("/", SPIFFS, "/");
  server.on("/showip", HTTP_GET, [](AsyncWebServerRequest * request) {   //Create an address instruction to display IP
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
    if (sysOpenFlag == true && (!isnan(tem))) {                                //Refresh OLED is refreshed when IP and readings
        if (position == 0 && tem < setValue) {                                    //When the POS is at 0 (tentative air conditioner, the real-time temperature is less than the set temperature. Cold, open heating. (Turning off air conditioning, no heating)
          position = 1;                                                           //Update switch position
          stepRun(180);                                                           //Open heating
        }
        if (position == 1 && tem > setValue) {                                    //When POS is at 1 (tentative air conditioner opening), the real-time temperature is greater than the set temperature. Hot, close the air conditioner. (Turn on air conditioner, heating)
          position = 0;                                                           //Update switch position
          stepRun(-180);                                                          // Close Heating
        }
    }
    Serial.println(message);
    request->send(200, "text/plain", "OK");
  });
  events.onConnect([](AsyncEventSourceClient * client) {              // Processing Web Server Events
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });

  upTime = millis();                                     //Update time
  temCheckTime = millis();                               //Temperature test time
  tempGetTime  = millis();                               //Temperature time
  showIPTime   = millis();                               //IP time
  printTime    = millis();                               //printing time
}

void loop() {                  
  keyCheck(buttonA, 1);                   //Button detection
  keyCheck(buttonB, 2);                   //Button detection                       / Main program
  keyCheck(buttonC, 3);                   //Button detection
  keyCheck(buttonD, 4);                   //Button detection
  if (showIPFlag == true) {                                                     //Display IP instruction Get
    oledShowIp();                                                               //OLED display
  }
  if ( (showIPFlag == true) && ((millis() - showIPTime) > showIPTimeMax  )) {   //IP display completion
    showIPTime = millis();                                                      //Update time
    showIPFlag = false;                                                         //Reset display flag
    oledShowIp();                                                               //Screen display IP
  }
  if ((millis() - upTime) > upTimeMax) {                                      //Upload data to web
    upTime = millis();                                                        //Update time
    char buf[5], buf2[5];                                                     //New char []
    if (!isnan(tem)) {                                                        //Temperature is not empty
      sprintf(buf, "%s", String(tem));                                        //INT turn char []
      events.send(buf, "temp", millis());                                     //Upload real-time temperature
    }                                                                         
    delay(100);                                                               //Delay
    sprintf(buf2, "%s", String(setValue));                                    //Conversion variable
    events.send(buf2, "temptag", millis());                                   //Upload target temperature
  }
  if (millis() - tempGetTime >= tempGetTimeMax) {                             //Get temperature data
    tempGetTime = millis();                                                   //Update time
    tem = dht.readTemperature();                                              //Get temperature
    Serial.print(F("Temperature: "));                                         //Serial port printing
    Serial.print(tem);Serial.println(F("°C "));
    if (showIPFlag != true && (!isnan(tem))) {                                //Refresh OLED is refreshed when IP and readings
      oledShow(tem, setValue);
    }
  }
  // The system starts will detect if the temperature is exceeded / below temperature. 
   // POS 0 1 default 0 heating air conditioner not 0 is heating air conditioner 'Close' position 1 is heating air conditioner 'open'
  if (sysOpenFlag == true) {
    // Tips When the system is turned on, detect once every 2 minutes because the switch air conditioner is not right.
    if (millis() - temCheckTime >= temCheckTimeMax) {                          // 2 minutes to detect once

      temCheckTime = millis();                                                  // Update time
      if (sysOpenFlag == true && (!isnan(tem))) {                               // Do not display IP and readings, refresh OLED
        if (position == 0 && tem < setValue) {                                    //When the POS is at 0 (tentative air conditioner, the real-time temperature is less than the set temperature. Cold, open heating. (Turning off air conditioning, no heating)
          position = 1;                                                           //Update switch position
          stepRun(180);                                                           //Open heating
        }
        if (position == 1 && tem > setValue) {                                    //When POS is at 1 (tentative air conditioner opening), the real-time temperature is greater than the set temperature. Hot, close the air conditioner. (Turn on air conditioner, heating)
          position = 0;                                                           //Update switch position
          stepRun(-180);                                                          //Close heating
        }
      }
    }
  }
  
}
