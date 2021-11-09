# ECE411-Team3
Practicum Project for ECE 411


The App will attempt to connect ESP32 to the internet.  But if the ESP32 is unable to connect it will automaticlly create an Access Point called "AutoconnectAP".
Direct your computer to connect to that access point and use your web browser to go to http://192.168.132.1, click 'configure' and enter your wifi credentials.  The credentials stay local and stored on the ESP32.
To erase the credentials stored on the ESP32:     
    1.) uncomment the line:     
  ```//ESPAsync_wifiManager.resetSettings();   //reset saved settings:```     
    2.) upload and reset the ESP32, your credentials will be deleted.     
    3.) recomment the line     
    4.) reupload and reset the ESP32     

Once connected the device will present an IP address where the website is hosted on your local network.



Current program functionality:
    - Connects to the internet
    - Retrieves time from NIST server
    - Retrives temp and humidity from the sensor
    -- Samples every x seconds set by DHT_UPDATE_DELAY (currently every 5s)
    --- This functionality is currently based on the NIST time from the server.
    --- Should be changed to just using millis()
    - Prints temp and humidity on the OLED
    -- Uses U9g2 library.
    --- lots of examples in the examples folder.  
    --- left some example code in the program commented out
    - Buttons move the stepper motor
    -- BUTTON_A moves the stepper clockwise
    -- BUTTON_B moves the stepper counter-clockwise
    -- Buttons labeled as BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D
    --- Button pins are 26,25,33,14 respectivly
    - Website can control stepper motor
    -- Through some exprimenting it was found how the data is transmitted
    --- This is documented in the .js file and the .cpp file.
    
Needs to be done:
    -Menu
    -- Setup
    --- set time
    --- set leave time
    --- set arrive time
    --- set range for stepper motor to turn heater on/off
    -- Move stepper motor
    --- Add/subtract movement from the stepper range variable
    -- Current Temp/Humidity
    --- Display the current temp and humidity 
    - Change the timing function to millis()
    - Retrive all data from website
    
