int waterSensorPin = 0;
int ldrPin = 2;
long totalValue;

void setup() {
  // Starting Serial Communication
  Serial.begin(9600);
}

void loop() {
  totalValue = 0;
  watersensor();
  totalValue = 0;
  ldr();
  delay(500);
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
  int waterPercent = map(waterSensorValue, 0, 4095, 0, 100);
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
  int ldrPercent = map(ldrValue, 400, 4095, 0, 100);
  ldrPercent = constrain(ldrPercent, 0, 100);

  Serial.print("Brightness: ");
  Serial.print(ldrValue);
  Serial.print(" (");
  Serial.print(ldrPercent);
  Serial.println("%)");
}
