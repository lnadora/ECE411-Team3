# ECE411-Team3
Practicum Project for ECE 411


The App will attempt to connect ESP32 to the internet.  But if the ESP32 is unable to connect it will automaticlly create an Access Point called "AutoconnectAP".
Direct your computer to connect to that access point and use your web browser to go to http://192.168.132.1, click 'configure' and enter your wifi credentials.  The credentials stay local and stored on the ESP32.
To erase the credentials stored on the ESP32:

    1.) uncomment the line:
  //ESPAsync_wifiManager.resetSettings();   //reset saved settings
    2.) upload and reset the ESP32, your credentials will be deleted.  
    3.) recomment the line
    4.) reupload and reset the ESP32

Once connected the device will present an IP address where the website is hosted on your local network.



