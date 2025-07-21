#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4  // Change this to the pin where your sensor is connected

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  delay(1000);
    Serial.begin(9600);
    Serial.println("Searching for DS18B20 sensors...");

    sensors.begin();
    int deviceCount = sensors.getDeviceCount();
    Serial.print("Found ");
    Serial.print(deviceCount);
    Serial.println(" sensors.");

    if (deviceCount == 0) {
        Serial.println("⚠️ No sensors found! Check wiring.");
        return;
    }

    DeviceAddress tempDeviceAddress;
    
    for (int i = 0; i < deviceCount; i++) {
        if (sensors.getAddress(tempDeviceAddress, i)) {
            Serial.print("Sensor ");
            Serial.print(i);
            Serial.print(" Address: ");
            printAddress(tempDeviceAddress);
            Serial.println();
        } else {
            Serial.println("❌ Unable to find address for Device ");
            Serial.print(i);
        }
    }
}

void loop() {}

// Function to print the address
void printAddress(DeviceAddress deviceAddress) {
    for (uint8_t i = 0; i < 8; i++) {
        if (deviceAddress[i] < 16) Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
    }
}
