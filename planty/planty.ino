int waterSensorPin = 0;

void setup() {
  // Starting Serial Communication
  Serial.begin(9600);
}

void loop() {
  // Reading the current value of the water sensor
  int sensorValue = analogRead(waterSensorPin);

  //Convert value into percent
  int waterPercent = map(sensorValue, 0, 4095, 0, 100);
  waterPercent = constrain(waterPercent, 0, 100);

  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  Serial.print("Water Percentage: ")
  Serial.println(waterPercent)

  delay(500);
}
