#define TURBIDITY_PIN A0  // Sensor output connected to A0
#define THRESHOLD 500     // Adjust based on your needs

void setup() {
  Serial.begin(9600);
  pinMode(TURBIDITY_PIN, INPUT);
}

void loop() {
  int sensorValue = analogRead(TURBIDITY_PIN); // Read the analog value
  Serial.print("Analog Value: ");
  Serial.print(sensorValue);

  if (sensorValue > THRESHOLD) {
    Serial.println(" -> Clear (HIGH)");
  } else {
    Serial.println(" -> Turbid (LOW)");
  }

  delay(1000);
}
