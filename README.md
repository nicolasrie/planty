![](assets/planty_banner.png)

*Planty* is an smart plant monitoring system powered by an ESP32-C3. It tracks your plant's vital signs and displays them locally on a OLED screen and a local web dashboard.

## Features
- Dual Sensor Tracking to know exactly what your plant needs
- OLED Live Dashboard right next to your plant
- Local Web Server

## Hardware
- ESP32 C3 SuperMini
- 0,96 Inch OLED Display
- LDR
- HW-038 Water Sensor

## How it works
1. Planty boots up, shows his logo on the OLED, mounts LittleFS and connects to your WiFi.
2. Every cycle reads averaged sensor data, maps values to percentages (0-100%), updates the OLED display, and handles incoming web clients.
3. Type the Planty's local IP address into any browser on your home network to check your plant's status.

## Required Arduino Libaries
Make sure you have the following libraries installed via the Arduino IDE Library Manager:
- `Adafruit GFX Library`
- `Adafruit SSD1306`
- `WebServer`
- `LittleFS`

## Getting Started
### 1. Configure WiFi Credentials
Create a file named `secrets.h` in your project root directory and add your Wi-Fi details:
```cpp
#define WiFi_SSID "Your_WiFi_Name"
#define WiFi_Password "Your_WiFi_Password"
```
### 2. Upload the Web Files
Use the LittleFS Data Upload tool in your Arduino IDE (Cmd + Shift + P -> Upload LittleFS in IDE 2.x) to flash the website files to the ESP32-C3.

### 3. Flash the Code
Open the main sketch in your Arduino IDE, select your ESP32-C3 Dev Module, and upload the code.

## Attribution
[Tea leaf icon](https://www.flaticon.com/free-icons/tea-leaf)