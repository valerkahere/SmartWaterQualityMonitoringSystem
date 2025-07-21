const int sensorPin = A0; // Sensor connected to A0
float voltage;
float ntu;

void setup() { 
    Serial.begin(9600);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
}

void loop() {
    int sensorValue = analogRead(sensorPin);
    voltage = sensorValue * (5.0 / 1023.0); // Convert ADC to voltage

    // Apply SEN0189 formula for NTU
    ntu = -1120.4 * pow(voltage, 2) + 5742.3 * voltage - 4352.9;

    // Display values in Serial Monitor
    Serial.print("Raw ADC: ");
    Serial.print(sensorValue);
    Serial.print(", Voltage: ");
    Serial.print(voltage);
    Serial.print(" V, NTU: ");
    Serial.println(ntu);

    // LED indication based on turbidity level
    if (ntu < 5) {
        digitalWrite(2, HIGH);  // Clear water
        digitalWrite(3, LOW);
        digitalWrite(4, LOW);
        Serial.println("Water Status: CLEAR");
    } else if (ntu >= 5 && ntu < 50) {
        digitalWrite(2, LOW);
        digitalWrite(3, HIGH);  // Cloudy water
        digitalWrite(4, LOW);
        Serial.println("Water Status: CLOUDY");
    } else {
        digitalWrite(2, LOW);
        digitalWrite(3, LOW);
        digitalWrite(4, HIGH);  // Dirty water
        Serial.println("Water Status: DIRTY");
    }

    delay(1000);
}
