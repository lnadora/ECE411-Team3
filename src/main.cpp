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
#include <ESPAsyncWebServer.h>              //包含"ESPAsyncWebServer.h"头文件
#define IN1               19                //步进电机控制引脚
#define IN2               18                //步进电机控制引脚
#define IN3               5                 //步进电机控制引脚
#define IN4               17                //步进电机控制引脚
#define DHTPIN            27                //dht11控制引脚
#define buttonA           33                //按键引脚
#define buttonB           26                //按键引脚
#define buttonC           25                //按键引脚
#define buttonD           14                //按键引脚
#define DHTTYPE           DHT11             //dht11引脚
#define tempGetTimeMax    2000              //dht11获取温度时间
#define printTimeMax      500               //串口打印时间
#define upTimeMax         500               //上传web时间
#define showIPTimeMax     3000              //有IP网络时候的显示时间
//#define temCheckTimeMax 120000            //温度检查时间120s
#define temCheckTimeMax   200               //测试用 温度检查时间 200ms
float setValue              = 20;           //初始设置温度

unsigned long upTime        = 0;            //更新数据时间变量
unsigned long printTime     = 0;            //串口打印时间变量
unsigned long showIPTime    = 0;            //ip时间变量
unsigned long tempGetTime   = 0;            //获取温度时间变量
unsigned long temCheckTime  = 0;            //温检时间变量
float hum                   = 20;           //湿度
float tem                   = 20;           //温度
int   position              = 0;            //开关位置
bool  sysOpenFlag           = true;         //系统启动温检控制
bool  showIPFlag            = false;        //ip通信标志位
bool  moveDir               = true;         //步进电机方向
const char* PARAM_MESSAGE   = "message";    //setValue信息
DHT dht(DHTPIN, DHTTYPE);                   //dht11设定
AsyncWebServer   server(80);                //在端口80上创建AsyncWebServer对象
AsyncEventSource events("/events");         //创建事件源/events
CheapStepper stepper (IN1, IN2, IN3, IN4);  //步进电机引脚设定
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   //OLED
struct Button {
  int buttonState     = LOW;                //按钮状态变量
  int lastButtonState = HIGH;               //按钮状态初始化
  int buttonPushNum   = 0;                  //记录按钮按键次数
  boolean flag;                             //判断标志
  long lastDebounceTime = 0;                //记录抖动变量
  long debounceDelay    = 50;               //抖动时间变量50ms
};
Button button[5];                           //新建1个按钮

void initSPIFFS() {                                     //初始化SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");//失败
  }
  Serial.println("SPIFFS mounted successfully");        //成功
}
void initWiFi() {                                       //初始化WiFi
  WiFi.mode(WIFI_STA);                                  //STA模式
  WiFiManager wm;                                       //声明
  //wm.resetSettings();                                 //重置wifi连接信息
  bool res;                                             //获取连接情况
  res = wm.autoConnect("TemCtrl", "12345678");          //wifi 名称 密码
  if (!res) {
    Serial.println("Failed to connect");                //未连接
    // ESP.restart();
  }
  else {
    Serial.println("connected...yeey :)");              //连接完成
    Serial.println(WiFi.localIP());                     //WiFiip串口打印
    showIPFlag = true;                                  //显示一次ip
    dht.begin();                                        //dht11初始化
    server.addHandler(&events);                         //链接事件源和服务器
    server.begin();                                     //启动服务器
  }
}
void oledShow(float tem, float set) {                   //Oled显示
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
void oledInit() {                                       //Oled初始化显示                    
  u8g2.clearBuffer();                                                        
  u8g2.setFont(u8g2_font_ncenB08_tr);                                          
  u8g2.drawStr(0, 10, "Temp:      ");                                                                          
  u8g2.drawStr(0, 20, "Set Value: ");           
  u8g2.drawStr(0, 30, "SystemState: Start");               
  u8g2.sendBuffer();
}
void stepRun(int angle) {                               // -360° - 0 - 360°
  int   turnAngle = 0;                                  //转动角度
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
void oledShowIp() {                                     //显示IP
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "IP:");
  u8g2.setCursor(32, 10);
  u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
}
void keyCheck(int button_Pin, int button_num) {
  int reading = digitalRead(button_Pin);                             //读取按键状态并存储到变量中
  if (reading != button[button_num].lastButtonState) {               //检查下按键状态是否改变
    button[button_num].lastDebounceTime = millis();                  //如果发生改变，更新时间
  }
  if ((millis() - button[button_num].lastDebounceTime) > button[button_num].debounceDelay) {
    if (reading != button[button_num].buttonState) {                 //如果按键状态改变了:
      button[button_num].buttonState = reading;                      //记录按键状态
      if (button[button_num].buttonState == LOW) {                   //只有当稳定的按键状态时才改变LED状态
        button[button_num].buttonPushNum++;                          //此时按键次数自增
        button[button_num].flag = true;                              //记录按键状态到按键标志flag
        if (button_Pin == buttonA) {                               //前进并在200ms后检测电流
          sysOpenFlag = true;                                           //切换状态
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
        if (button_Pin == buttonB) {                               //前进并在200ms后检测电流
          sysOpenFlag = false;                                           //切换状态
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
        //button[button_num].flag = false;                             //如果按键没有改变，那么记录按键状态为false
      }
    }
  }
  button[button_num].lastButtonState = reading;                      // 保存处理结果:
}

void setup() {
  Serial.begin(115200);                                 //初始化波特率115200
  pinMode(buttonA,INPUT_PULLUP);                        //引脚设置输入上拉
  pinMode(buttonB,INPUT_PULLUP);                        //引脚设置输入上拉
  pinMode(buttonC,INPUT_PULLUP);                        //引脚设置输入上拉
  pinMode(buttonD,INPUT_PULLUP);                        //引脚设置输入上拉
  u8g2.begin();                                         //oled初始化
  oledInit();                                           //oled显示初始化
  initWiFi();                                           //初始化WIFI
  initSPIFFS();                                         //初始化文件系统
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {         //处理Web服务器
    request->send(SPIFFS, "/index.html", "text/html");                   //连接SPIFFS和index.html
  });
  server.serveStatic("/", SPIFFS, "/");
  server.on("/showip", HTTP_GET, [](AsyncWebServerRequest * request) {   //创建一个地址 指令显示ip
    showIPTime = millis();
    showIPFlag = true;                                                   //IPshow
    Serial.println("showIP");
    request->send(200, "text/plain", "OK");
  });
  server.on("/open", HTTP_GET, [](AsyncWebServerRequest * request) {     //创建一个地址
    sysOpenFlag = true ;
    char buf[20];
    sprintf(buf, "%s", "System State: ON");                              //ON
    events.send(buf, "systemstate", millis());
    request->send(200, "text/plain", "OK");
  });
  server.on("/close", HTTP_GET, [](AsyncWebServerRequest * request) {    //创建一个地址
    sysOpenFlag = false ;
    char buf[20];
    sprintf(buf, "%s", "System State: OFF");                             //OFF
    events.send(buf, "systemstate", millis());
    request->send(200, "text/plain", "OK");
  });
  server.on("/setvalue", HTTP_GET, [](AsyncWebServerRequest * request) { //创建一个地址
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
    if (sysOpenFlag == true && (!isnan(tem))) {                                //不显示ip和读数有数时，刷新oled
        if (position == 0 && tem < setValue) {                                    //当pos在0（暂定空调关闭） 同时 实时温度小于设定温度  。冷了，开启加热。(关闭空调，不加热）
          position = 1;                                                           //更新开关位置
          stepRun(180);                                                           //开启 加热
        }
        if (position == 1 && tem > setValue) {                                    //当pos在1（暂定空调开启） 同时 实时温度大于设定温度  。热了，关闭空调。(开启空调，加热）
          position = 0;                                                           //更新开关位置
          stepRun(-180);                                                          //关闭 加热
        }
    }
    Serial.println(message);
    request->send(200, "text/plain", "OK");
  });
  events.onConnect([](AsyncEventSourceClient * client) {              //处理Web服务器事件
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });

  upTime = millis();                                     //更新时间
  temCheckTime = millis();                               //温检时间
  tempGetTime  = millis();                               //温度时间
  showIPTime   = millis();                               //ip时间
  printTime    = millis();                               //打印时间
}

void loop() {                  
  keyCheck(buttonA, 1);                   //按键检测
  keyCheck(buttonB, 2);                   //按键检测                           /主程序
  keyCheck(buttonC, 3);                   //按键检测
  keyCheck(buttonD, 4);                   //按键检测
  if (showIPFlag == true) {                                                     //显示ip指令get
    oledShowIp();                                                               //OLED显示
  }
  if ( (showIPFlag == true) && ((millis() - showIPTime) > showIPTimeMax  )) {   //ip显示完成
    showIPTime = millis();                                                      //更新时间
    showIPFlag = false;                                                         //重置显示标志位
    oledShowIp();                                                               //屏幕显示IP
  }
  if ((millis() - upTime) > upTimeMax) {                                      //上传数据到web
    upTime = millis();                                                        //更新时间
    char buf[5], buf2[5];                                                     //新建char[]
    if (!isnan(tem)) {                                                        //温度不为空
      sprintf(buf, "%s", String(tem));                                        //int转char[]
      events.send(buf, "temp", millis());                                     //上传实时温度
    }                                                                         
    delay(100);                                                               //延时一会
    sprintf(buf2, "%s", String(setValue));                                    //转换变量
    events.send(buf2, "temptag", millis());                                   //上传目标温度
  }
  if (millis() - tempGetTime >= tempGetTimeMax) {                             //获取温度数据
    tempGetTime = millis();                                                   //更新时间
    tem = dht.readTemperature();                                              //获取温度
    Serial.print(F("Temperature: "));                                         //串口打印
    Serial.print(tem);Serial.println(F("°C "));
    if (showIPFlag != true && (!isnan(tem))) {                                //不显示ip和读数有数时，刷新oled
      oledShow(tem, setValue);
    }
  }
  //系统启动才会检测是否超过温度/低于温度
  //pos 0 1   默认0 加热空调不开   0是加热空调'关闭'位置  1是加热空调'开启'位置
  if (sysOpenFlag == true) {
    //暂定系统开启时，每2分钟检测一次，因为开关空调不是马上变温
    if (millis() - temCheckTime >= temCheckTimeMax) {                           //2分钟检测一次

      temCheckTime = millis();                                                  //更新时间
      if (sysOpenFlag == true && (!isnan(tem))) {                                //不显示ip和读数有数时，刷新oled
        if (position == 0 && tem < setValue) {                                    //当pos在0（暂定空调关闭） 同时 实时温度小于设定温度  。冷了，开启加热。(关闭空调，不加热）
          position = 1;                                                           //更新开关位置
          stepRun(180);                                                           //开启 加热
        }
        if (position == 1 && tem > setValue) {                                    //当pos在1（暂定空调开启） 同时 实时温度大于设定温度  。热了，关闭空调。(开启空调，加热）
          position = 0;                                                           //更新开关位置
          stepRun(-180);                                                          //关闭 加热
        }
      }
    }
  }
  
}
