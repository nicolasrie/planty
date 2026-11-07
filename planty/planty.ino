#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Include Graphics
#include "graphics.h"

#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <time.h>

// Web Server on port 80
WebServer server(80);

#include "secrets.h"

// OLED Display Pins
int i2c_sda = 5;
int i2c_scl = 6;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int waterSensorPin = 0;
int ldrPin = 2;

int waterPercent = 0;
int ldrPercent = 0;

long millis_lastMeasurement = 0;
long millis_lastWiFiAttempt = 0;

long millis_lastStorage = 0;
long storageInterval = 120000;
int maxFileSize = 500 * 1024; // 500 KB

void setup() {
  // Starting Serial Communication
  Serial.begin(9600);

  // Starting I2C on SCL and SDA Pin
  Wire.begin(i2c_sda, i2c_scl);

  // Initialise display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 Allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.drawBitmap(0, 0, graphic_leafs, 128, 64, SSD1306_WHITE);
  display.display();

  if(!LittleFS.begin(true)){
    Serial.println("Failed to mount LittleFS");
    return;
  }

  // Connecting to WiFi and starting webserver
  Serial.println("Connecting to WiFi");
  WiFi.begin(WiFi_SSID, WiFi_Password);
  WiFi.mode(WIFI_STA);
  long wifi_startAttempt = millis();
  // Check every 50ms for 10s if connected to WiFi
  while(WiFi.status() != WL_CONNECTED && millis() - wifi_startAttempt < 10000){
    delay(50);
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Connected!");
    Serial.print("IP-Adress: ");
    Serial.println(WiFi.localIP());
  } else{
    Serial.println("WiFi connection failed!");
  }
  WiFi.setAutoReconnect(true);

  // Configure timezone and synchronize time using NTP
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org");

  // Configure the web server depending on the selected dashboard mode
  if (External_Dashboard == true) {
    // Register the API endpoint for external dashboard access
    server.on("/api/status", HTTP_GET, handleApi);
  } else {
    // Register the routes for the internal dashboard
    server.on("/", HTTP_GET, handleRoot);
    server.on("/style.css", HTTP_GET, handleCss);
  }

  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - millis_lastMeasurement >= 2000){
    millis_lastMeasurement = millis();
    watersensor();
    ldr();

    //Show sensor values on display
    drawDashboard();

    if (millis() - millis_lastStorage >= storageInterval) {
      millis_lastStorage = millis();
      saveMeasurement();
    }
  }

  // If not connected with WiFi, try to connect
  if(WiFi.status() != WL_CONNECTED && millis() - millis_lastWiFiAttempt >= 10000){
    millis_lastWiFiAttempt = millis();

    Serial.println("Trying to reconnect to WiFi...");
    WiFi.begin(WiFi_SSID, WiFi_Password);
  }
}

void watersensor(){
  long totalValue = 0;
  // Generate the average of all values (water sensor)
    for (int i = 0; i < 5; i = i + 1){
    // Read the current value of the water sensor
    totalValue += analogRead(waterSensorPin);
    delay(10);
  }
  int waterSensorValue = totalValue / 5;

  //Convert value into percent
  waterPercent = map(waterSensorValue, 0, 4095, 0, 100);
  waterPercent = constrain(waterPercent, 0, 100);

  Serial.print("WaterSensor Value: ");
  Serial.println(waterSensorValue);
  Serial.print("Water Percentage: ");
  Serial.println(waterPercent);
}

void ldr(){
  long totalValue = 0;
  // Generate the avarage of all values (ldr)
  for (int i = 0; i < 5; i = i + 1){
    // Read the current value of the ldr
    totalValue += analogRead(ldrPin);
    delay(10);
  }
  int ldrValue = totalValue / 5;

  //Convert value into percent
  ldrPercent = map(ldrValue, 400, 4095, 0, 100);
  ldrPercent = constrain(ldrPercent, 0, 100);

  Serial.print("Brightness: ");
  Serial.print(ldrValue);
  Serial.print(" (");
  Serial.print(ldrPercent);
  Serial.println("%)");
}

void drawDashboard(){
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 2);
  display.print("Plant Health");

  display.drawLine(0, 14, 128, 14, SSD1306_WHITE);

  // water sensor values
  display.setCursor(0, 28);
  display.print("Soil Moisture: ");
  display.setCursor(96, 28);
  display.print(waterPercent);
  display.print("%");

  // light level values
  display.setCursor(0, 45);
  display.print("Light Level: ");
  display.print(ldrPercent);
  display.print("%");

  display.display();
}

void handleRoot() {
  // html
  File file = LittleFS.open("/index.html", "r");
  if(!file){
    server.send(404, "text/plain", "File not found");
    return;
  }

  String html = file.readString();
  file.close();

  html.replace("%WATER_VAL%", String(waterPercent));
  html.replace("%LIGHT_VAL%", String(ldrPercent));

  server.send(200, "text/html", html);
}

void handleCss() {
  // css
  File file = LittleFS.open("/style.css", "r");
  if(!file){
    server.send(404, "text/plain", "File not found");
    return;
  }
  server.streamFile(file, "text/css");
  file.close();
}

// Handle API requests and return current sensor values as JSON
void handleApi() {
  time_t now;
  time(&now);

  String json = "{";
  json += "\"water\": " + String(waterPercent) + ",";
  json += "\"light\": " + String(ldrPercent) + ",";
  json += "\"timestamp\": " + String((long)now);
  json += "}";

  server.send(200, "application/json", json);
}

// Save measured data
void saveMeasurement() {
  const char* path = "/measurements.csv";

  // Check file size
  File file = LittleFS.open(path, "r");

  if (file && file.size() >= maxFileSize) {
    file.close();

    File oldFile = LittleFS.open(path, "r");
    File newFile = LittleFS.open("/temp.csv", "w");

    if (!oldFile || !newFile) {
      Serial.println("File rotation failed!");
      return;
    }

    // Skip header
    oldFile.readStringUntil('\n');

    // Keep only newer measurements
    size_t skipBytes = oldFile.size() / 5;

    while (skipBytes > 0 && oldFile.available()) {
      if (oldFile.read() == '\n') {
        skipBytes--;
      }
    }

    newFile.println("timestamp,water,light");

    while (oldFile.available()) {
      newFile.write(oldFile.read());
    }

    oldFile.close();
    newFile.close();

    LittleFS.remove(path);
    LittleFS.rename("/temp.csv", path);
  } else if (file) {
    file.close();
  }

  // Save new measurement
  file = LittleFS.open(path, "a");

  if (!file) {
    Serial.println("Failed to open measurement file");
    return;
  }

  if (file.size() == 0) {
    file.println("timestamp,water,light");
  }

  time_t now;
  time(&now);

  file.print((long)now);
  file.print(",");
  file.print(waterPercent);
  file.print(",");
  file.println(ldrPercent);

  file.close();
}