const int turbidityPin = A0; // Analog pin connected to sensor

void setup() {
  Serial.begin(9600);
  // No need for analogReadResolution() (Yun uses 10-bit ADC by default)
}

void loop() {
  int rawValue = analogRead(turbidityPin); // Range: 0–1023

  // Convert ADC value to voltage (Yun uses 5V reference)
  float voltage = rawValue * (5.0 / 1023.0); 

  // Convert voltage to NTU (calibration required)
  float ntu = mapVoltageToNTU(voltage); // Replace with your calibration

  Serial.print("Raw ADC: "); Serial.print(rawValue);
  Serial.print(" | Voltage: "); Serial.print(voltage, 2);
  Serial.print("V | NTU: "); Serial.println(ntu);
  delay(1000);
}

// Custom calibration function (adjust based on your sensor)
float mapVoltageToNTU(float voltage) {
  // Example calibration:
  // Clean water (0 NTU) might read ~1.2V
  // High turbidity (1000 NTU) might read ~4.5V
  return (voltage - 1.2) * (1000 / (4.5 - 1.2)); // Linear approximation
}
