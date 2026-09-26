int waterSensorPin = 0;

void setup() {
  // Starting Serial Communication
  Serial.begin(9600);
}

void loop() {
  // Reading the current value of the water sensor
  int sensorValue = analogRead(waterSensorPin);

  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  delay(500);
}
