int turbidityPin = A0; // Analog pin connected to sensor A0
int sensorValue = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(turbidityPin);
  Serial.print("Turbidity Value: ");
  Serial.println(sensorValue);
  delay(1000);
}
