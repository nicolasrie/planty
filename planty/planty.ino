int waterSensorPin = 0;
int ldrPin = 2;

void setup() {
  // Starting Serial Communication
  Serial.begin(9600);
}

void loop() {
  // Generate the average of all values
  long totalValue = 0;
  for (int i = 0; i < 5; i = i + 1){
    // Read the current value of the water sensor
    totalValue += analogRead(waterSensorPin);
    delay(10);
  }
  int sensorValue = totalValue / 5;

  //Convert value into percent
  int waterPercent = map(sensorValue, 0, 4095, 0, 100);
  waterPercent = constrain(waterPercent, 0, 100);

  // Generate the avarage of all values (ldr)
  for (int i = 0; i < 5; i = i + 1){
    // Read the current value of the ldr
    totalValue += analogRead(ldrPin);
    delay(10);
  }
  int ldrValue = totalValue / 5;

  //Convert value into percent
  int ldrPercent = map(ldrValue, 400, 4095, 0, 100);
  ldrPercent = constrain(ldrPercent, 0, 100);

  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  Serial.print("Water Percentage: ");
  Serial.println(waterPercent);

  Serial.print("Brightness: ");
  Serial.print(ldrValue);
  Serial.print(" (");
  Serial.print(ldrPercent);
  Serial.println("%)");

  delay(500);
}
