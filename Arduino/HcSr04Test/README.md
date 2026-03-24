# HC-SR04 ESP32 Test Project

This folder contains a test sketch for reading an HC-SR04 ultrasonic sensor with an ESP32 and displaying the data on a web dashboard. It also includes functionality to toggle a component (the onboard LED) via the web interface.

## Hardware Required
- **ESP32 Development Board**
- **HC-SR04 Ultrasonic Sensor**

## Pinout
- **HC-SR04 Trig Pin**: GPIO 5
- **HC-SR04 Echo Pin**: GPIO 18
- **LED/Control Pin**: GPIO 2 (ESP32 Onboard LED)

*Note: The HC-SR04 requires a 5V supply and outputs 5V. To prevent damage to the ESP32 (which is 3.3V tolerant on some boards but strictly 3.3V on most), use a voltage divider on the Echo pin if required.*

## Setup Instructions
1. Open `HcSr04Test.ino` in the Arduino IDE (or PlatformIO).
2. Change the `ssid` and `password` variables to match your Wi-Fi network.
3. Flash the code to your ESP32.
4. Open the Serial Monitor (115200 baud) to find the ESP32's IP address.
5. Enter the IP address in a web browser to access the Web Dashboard.
