/***********************************************************************************
 ***********************************************************************************
                                  Includes
 ***********************************************************************************
 **********************************************************************************/

#include <Arduino.h>
#include <stdlib.h>
#include <stepper.h>
#include <FS.h>
#include <WiFi.h>
#include <ESPAsync_WiFiManager.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <string.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

/***********************************************************************************
 ***********************************************************************************
                                  Define Pins
 ***********************************************************************************
 **********************************************************************************/
//#define LED_BUILTIN 2 // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED

#define PIN_D0 0 // Pin D0 mapped to pin GPIO0/BOOT/ADC11/TOUCH1 of ESP32
#define PIN_D1 1 // Pin D1 mapped to pin GPIO1/TX0 of ESP32
#define PIN_D2 2 // Pin D2 mapped to pin GPIO2/ADC12/TOUCH2 of ESP32
#define PIN_D3 3 // Pin D3 mapped to pin GPIO3/RX0 of ESP32
#define PIN_D4 4 // Pin D4 mapped to pin GPIO4/ADC10/TOUCH0 of ESP32
#define PIN_D5 5 // Pin D5 mapped to pin GPIO5/SPISS/VSPI_SS of ESP32
#define PIN_D6 6 // Pin D6 mapped to pin GPIO6/FLASH_SCK of ESP32
#define PIN_D7 7 // Pin D7 mapped to pin GPIO7/FLASH_D0 of ESP32
#define PIN_D8 8 // Pin D8 mapped to pin GPIO8/FLASH_D1 of ESP32
#define PIN_D9 9 // Pin D9 mapped to pin GPIO9/FLASH_D2 of ESP32

#define PIN_D10 10 // Pin D10 mapped to pin GPIO10/FLASH_D3 of ESP32
#define PIN_D11 11 // Pin D11 mapped to pin GPIO11/FLASH_CMD of ESP32
#define PIN_D12 12 // Pin D12 mapped to pin GPIO12/HSPI_MISO/ADC15/TOUCH5/TDI of ESP32
#define PIN_D13 13 // Pin D13 mapped to pin GPIO13/HSPI_MOSI/ADC14/TOUCH4/TCK of ESP32
#define PIN_D14 14 // Pin D14 mapped to pin GPIO14/HSPI_SCK/ADC16/TOUCH6/TMS of ESP32
#define PIN_D15 15 // Pin D15 mapped to pin GPIO15/HSPI_SS/ADC13/TOUCH3/TDO of ESP32
#define PIN_D16 16 // Pin D16 mapped to pin GPIO16/TX2 of ESP32
#define PIN_D17 17 // Pin D17 mapped to pin GPIO17/RX2 of ESP32
#define PIN_D18 18 // Pin D18 mapped to pin GPIO18/VSPI_SCK of ESP32
#define PIN_D19 19 // Pin D19 mapped to pin GPIO19/VSPI_MISO of ESP32

#define PIN_D21 21 // Pin D21 mapped to pin GPIO21/SDA of ESP32
#define PIN_D22 22 // Pin D22 mapped to pin GPIO22/SCL of ESP32
#define PIN_D23 23 // Pin D23 mapped to pin GPIO23/VSPI_MOSI of ESP32
#define PIN_D24 24 // Pin D24 mapped to pin GPIO24 of ESP32
#define PIN_D25 25 // Pin D25 mapped to pin GPIO25/ADC18/DAC1 of ESP32
#define PIN_D26 26 // Pin D26 mapped to pin GPIO26/ADC19/DAC2 of ESP32
#define PIN_D27 27 // Pin D27 mapped to pin GPIO27/ADC17/TOUCH7 of ESP32

#define PIN_D32 32 // Pin D32 mapped to pin GPIO32/ADC4/TOUCH9 of ESP32
#define PIN_D33 33 // Pin D33 mapped to pin GPIO33/ADC5/TOUCH8 of ESP32
#define PIN_D34 34 // Pin D34 mapped to pin GPIO34/ADC6 of ESP32

// Only GPIO pin < 34 can be used as output. Pins >= 34 can be only inputs
// See .../cores/esp32/esp32-hal-gpio.h/c
//#define digitalPinIsValid(pin)          ((pin) < 40 && esp32_gpioMux[(pin)].reg)
//#define digitalPinCanOutput(pin)        ((pin) < 34 && esp32_gpioMux[(pin)].reg)
//#define digitalPinToRtcPin(pin)         (((pin) < 40)?esp32_gpioMux[(pin)].rtc:-1)
//#define digitalPinToAnalogChannel(pin)  (((pin) < 40)?esp32_gpioMux[(pin)].adc:-1)
//#define digitalPinToTouchChannel(pin)   (((pin) < 40)?esp32_gpioMux[(pin)].touch:-1)
//#define digitalPinToDacChannel(pin)     (((pin) == 25)?0:((pin) == 26)?1:-1)

#define PIN_D35 35 // Pin D35 mapped to pin GPIO35/ADC7 of ESP32
#define PIN_D36 36 // Pin D36 mapped to pin GPIO36/ADC0/SVP of ESP32
#define PIN_D39 39 // Pin D39 mapped to pin GPIO39/ADC3/SVN of ESP32

#define PIN_RX0 3 // Pin RX0 mapped to pin GPIO3/RX0 of ESP32
#define PIN_TX0 1 // Pin TX0 mapped to pin GPIO1/TX0 of ESP32

#define PIN_SCL 22 // Pin SCL mapped to pin GPIO22/SCL of ESP32
#define PIN_SDA 21 // Pin SDA mapped to pin GPIO21/SDA of ESP32

/***********************************************************************************
 ***********************************************************************************
                                      WiFi Setup
 ***********************************************************************************
 **********************************************************************************/
AsyncWebServer webServer(80);
DNSServer dnsServer;

/***********************************************************************************
 ***********************************************************************************
                                      NTP Setup
 ***********************************************************************************
 **********************************************************************************/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String formattedDate;
String dayStamp;
String timeStamp;

/***********************************************************************************
 ***********************************************************************************
                                      OLED Setup
 ***********************************************************************************
 **********************************************************************************/
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/PIN_SCL, /* data=*/PIN_SDA, /* reset=*/U8X8_PIN_NONE);

/***********************************************************************************
 ***********************************************************************************
                                  Button Setup
 ***********************************************************************************
 **********************************************************************************/
#define BUTTON_A PIN_D26
#define BUTTON_B PIN_D25

/***********************************************************************************
 ***********************************************************************************
                                  Webserver setup
 ***********************************************************************************
 **********************************************************************************/
bool ledState = 0;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
String message = "";
// Variables to save values from HTML form
String direction = "STOP";
String steps;
String timeOn;

bool newRequest = false;

// Initialize SPIFFS
void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else
  {
    Serial.println("SPIFFS mounted successfully");
  }
}

void notifyClients(String state)
{
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    // message is the data sent to the ESP when the 'submit' button is pressed
    message = (char *)data;
    // right now we have three variables we recieve in the message string
    // they are seperated by an "&"
    // indexOfFirst is the position of the first "&"
    // indexOfSec is the positon of the second "&"
    int indexOfFirst = message.indexOf("&");
    int indexOfSec = message.indexOf("&", indexOfFirst + 1);
    // steps is the value between the beginning of the string and "&"
    steps = message.substring(0, message.indexOf("&"));
    // direction is the value between the first "&" and second
    direction = message.substring(indexOfFirst + 1, indexOfSec);
    // timeOn is the value between the second "&" and the end of the string
    timeOn = message.substring(indexOfSec + 1, message.length());
    Serial.print("steps");
    Serial.println(steps);
    Serial.print("direction");
    Serial.println(direction);
    Serial.print("timeOn");
    Serial.println(timeOn);
    notifyClients(direction);
    newRequest = true;
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    // Notify client of motor current state when it first connects
    notifyClients(direction);
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

/***********************************************************************************
 ***********************************************************************************
                                  Stepper Motor initialize
 ***********************************************************************************
 **********************************************************************************/
const int stepsPerRevolution = 2048;
#define IN1 PIN_D19
#define IN2 PIN_D18
#define IN3 PIN_D5
#define IN4 PIN_D17

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

/***********************************************************************************
 ***********************************************************************************
                                  DHT11 Setup
 ***********************************************************************************
 **********************************************************************************/
#define DHTTYPE DHT11  // DHT 11
#define DHTPIN PIN_D27 // Pin 27

DHT dht(DHTPIN, DHTTYPE);

/***********************************************************************************
 ***********************************************************************************
                                  setup Function
 ***********************************************************************************
 **********************************************************************************/
void setup()
{
  // begin serial
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(200);

  // Setup Display
  u8g2.begin();

  // Setup Button

  pinMode(BUTTON_A, INPUT_PULLDOWN);
  pinMode(BUTTON_B, INPUT_PULLDOWN);

  // Setup WiFi
  Serial.print("\nStarting Async_AutoConnect_ESP32_minimal on " + String(ARDUINO_BOARD));
  Serial.println(ESP_ASYNC_WIFIMANAGER_VERSION);
  ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer, "Async_AutoConnect");
  // ESPAsync_wifiManager.resetSettings();   //reset saved settings
  ESPAsync_wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 132, 1), IPAddress(192, 168, 132, 1), IPAddress(255, 255, 255, 0));
  ESPAsync_wifiManager.autoConnect("AutoConnectAP");
  u8g2.clearBuffer();                    // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);    // choose a suitable font
  u8g2.drawStr(0, 10, "Connect to AP!"); // write something to the internal memory
  u8g2.drawStr(0, 20, "192.168.132.1");
  u8g2.sendBuffer(); // transfer internal memory to the display
  Serial.print("\nConnect to the AutoConnectAP Access Point and point your browser to:192.168.132.1\n");
  delay(2000);
  if (WiFi.status() == WL_CONNECTED)
  {

    Serial.print(F("Connected. Local IP: "));
    Serial.println(WiFi.localIP());
    u8g2.clearBuffer();                        // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);        // choose a suitable font
    u8g2.drawStr(0, 10, "Connected to Wifi!"); // write something to the internal memory
    u8g2.drawStr(0, 20, WiFi.localIP().toString().c_str());
    u8g2.sendBuffer(); // transfer internal memory to the display
  }
  else
  {
    Serial.println(ESPAsync_wifiManager.getStatus(WiFi.status()));
  }

  // setup websocket
  initWebSocket();
  // setup SPIFFS
  initSPIFFS();
  // set the stepper speed
  myStepper.setSpeed(5);
  // Setup DHT
  dht.begin();
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.serveStatic("/", SPIFFS, "/");

  server.begin();

  // Setup for time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(-28800);
}

/***********************************************************************************
 ***********************************************************************************
                                  loop Function
 ***********************************************************************************
 **********************************************************************************/
void loop()
{
  //int buttonStateA = 0;
  //int buttonStateB = 0;
  int currentTime;
  static int updateTime = 0;

  // Time client setup
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }

  /*   // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    // We need to extract date and time
    formattedDate = timeClient.getFormattedDate();
    Serial.println(formattedDate);

    // Extract date
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    Serial.print("DATE: ");
    Serial.println(dayStamp);
    // Extract time
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    Serial.print("HOUR: ");
    Serial.println(timeStamp); */

  if (newRequest)
  {
    if (direction == "CW")
    {
      myStepper.step(steps.toInt());
      Serial.print("CW");
    }
    else
    {
      myStepper.step(-steps.toInt());
    }
    newRequest = false;
    notifyClients("stop");
  }
  ws.cleanupClients();

  if (digitalRead(BUTTON_A) == LOW)
    myStepper.step(100);
  if (digitalRead(BUTTON_B) == LOW)
    myStepper.step(-100);
  // Wait a few seconds between measurements.
  

  if (timeClient.getEpochTime() > updateTime)
  {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f))
    {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);
    char str[25];
    u8g2.clearBuffer();
    Serial.print(F("Humidity: "));
    u8g2.drawStr(0, 10, "Humidity: ");
    dtostrf(h, 3, 2, str);
    u8g2.drawStr(100, 10, str);
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    u8g2.drawStr(0, 20, "Temperature: ");
    dtostrf(f, 3, 2, str);
    u8g2.drawStr(100, 20, str);
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
    u8g2.sendBuffer();
    currentTime = timeClient.getEpochTime();
    updateTime = currentTime + 5;
  }
}