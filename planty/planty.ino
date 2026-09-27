#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Include Graphics
#include "graphics.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// OLED Display Pins
int i2c_sda = 5;
int i2c_scl = 6;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int waterSensorPin = 0;
int ldrPin = 2;

long totalValue;
int waterPercent = 0;
int ldrPercent = 0;

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
}

void loop() {
  delay(2000);
  totalValue = 0;
  watersensor();

  totalValue = 0;
  ldr();

  //Show sensor values on display
  drawDashboard();
}

void watersensor(){
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