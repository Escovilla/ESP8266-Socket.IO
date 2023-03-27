# ESP8266-Socket.IO Client for Arduino 

Example for arduino to esp8266 

#### Install https://github.com/Links2004/arduinoWebSockets 

for heroku server 👌
```
USE_SERIAL.printf("[SETUP] WiFi Connected %s\n", ip.c_str());
socketIO.begin("your_own_socket", 80, "/socket.io/?EIO=4");
```
Replace with your own server 
```
socketIO.begin("192.168.1.1", 5000, "/socket.io/?EIO=4")
```
