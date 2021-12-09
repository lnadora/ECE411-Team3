# ECE411-Team3
Practicum Project for ECE 411


The project will attempt to connect ESP32 to the internet. After user use their device(phone,ipad,pc etc) connect with the wifi, Esp32 wifi module will send the Ip to the user.User can according to the IP go into the website and achieve the website interact.
To erase the credentials stored on the ESP32:     
    1.) uncomment the line:     
  ```//ESPAsync_wifiManager.resetSettings();   //reset saved settings:```     
    2.) upload and reset the ESP32, your credentials will be deleted.     
    3.) recomment the line     
    4.) reupload and reset the ESP32     

Once connected the device will present an IP address where the website is hosted on your local network.



Current program functionality:

    - Connects to the internet
    - Retrives temp from the sensor
    -- Samples every x seconds set by DHT_UPDATE_DELAY (currently every 1s)
    --- Should be changed to just using millis()
    - Prints temp,current and set temperature,and Ip  on the OLED
    -- Uses U9g2 library.
    --- lots of examples in the examples folder.  
    --- left some example code in the program commented out
    - Buttons move the stepper motor
    -- BUTTON_A turon on the system
    -- BUTTON_B tuen off the system
    -- BUTTON_C Increase the setting tem
    -- BUTTON_D decrease the setting tem
    -- Buttons labeled as BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D
    --- Button pins are 26,25,33,14 respectivly
    - Website can control stepper motor
    - Website can achieve the stable temperature system
    -- Through some exprimenting it was found how the data is transmitted
    --- This is documented in the .js file and the .cpp file.
    
Needs to be done

    -Menu
    -- Setup
    --- set time
    --- set leave time
    --- set arrive time
    - Change the timing function to millis()
    

    
