/****************************************************************************************************************************
  Async_AutoConnect_ESP32_minimal.ino
  For ESP8266 / ESP32 boards
  Built by Khoi Hoang https://github.com/khoih-prog/ESPAsync_WiFiManager
  Licensed under MIT license
 *****************************************************************************************************************************/
#if !(defined(ESP32) )
  #error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif
#include <ESPAsync_WiFiManager.h>              //https://github.com/khoih-prog/ESPAsync_WiFiManager
AsyncWebServer webServer(80);
#if !( USING_ESP32_S2 || USING_ESP32_C3 )
DNSServer dnsServer;
#endif

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/stepper-motor-esp32-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Stepper.h>

// Stepper Motor Settings
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);


AsyncWebServer Server(80);


// Search for parameters in HTTP POST request
const char* PARAM_INPUT_1 = "direction";
const char* PARAM_INPUT_2 = "steps";

// Variables to save values from HTML form
String direction;
String steps;

// Variable to detect whether a new request occurred
bool newRequest = false;

// HTML to build the web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Stepper Motor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <h1>Stepper Motor Control</h1>
    <form action="/" method="POST">
      <input type="radio" name="direction" value="CW" checked>
      <label for="CW">Clockwise</label>
      <input type="radio" name="direction" value="CCW">
      <label for="CW">Counterclockwise</label><br><br><br>
      <label for="steps">Number of steps:</label>
      <input type="number" name="steps">
      <input type="submit" value="GO!">
    </form>
</body>
</html>
)rawliteral";


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200); while (!Serial); delay(200);
  Serial.print("\nStarting Async_AutoConnect_ESP32_minimal on " + String(ARDUINO_BOARD)); Serial.println(ESP_ASYNC_WIFIMANAGER_VERSION);
#if ( USING_ESP32_S2 || USING_ESP32_C3 )
  ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, NULL, "Async_AutoConnect");
#else
  ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer, "Async_AutoConnect");
#endif  
  //ESPAsync_wifiManager.resetSettings();   //reset saved settings
  ESPAsync_wifiManager.setAPStaticIPConfig(IPAddress(192,168,132,1), IPAddress(192,168,132,1), IPAddress(255,255,255,0));
  Serial.print("\nConnect to AutoConnectAP and point your browser to 192.168.132.1");
  ESPAsync_wifiManager.autoConnect("AutoConnectAP");
  if (WiFi.status() == WL_CONNECTED) { Serial.print(F("\nConnected. Local IP: ")); Serial.println(WiFi.localIP()); }
  else { Serial.println(ESPAsync_wifiManager.getStatus(WiFi.status())); }
  
myStepper.setSpeed(5);

// Handle request (form)
  Server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        // HTTP POST input1 value (direction)
        if (p->name() == PARAM_INPUT_1) {
          direction = p->value().c_str();
          Serial.print("Direction set to: ");
          Serial.println(direction);
        }
        // HTTP POST input2 value (steps)
        if (p->name() == PARAM_INPUT_2) {
          steps = p->value().c_str();
          Serial.print("Number of steps set to: ");
          Serial.println(steps);
        }
      }
    }
    request->send(200, "text/html", index_html);
    newRequest = true;
  });

  Server.begin();


}

void loop() {  
  // Check if there was a new request and move the stepper accordingly
  if (newRequest){
    if (direction == "CW"){
      // Spin the stepper clockwise direction
      myStepper.step(steps.toInt());
    }
    else{
      // Spin the stepper counterclockwise direction
      myStepper.step(-steps.toInt());
    }
    newRequest = false;
  }
}
