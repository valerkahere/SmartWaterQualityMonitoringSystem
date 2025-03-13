/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL4SMgA6DPM"
#define BLYNK_TEMPLATE_NAME "Test"
#define BLYNK_AUTH_TOKEN "V3fxHZ7iM3zJPq23uZLT3_rJ8N9BCR9R"

#include <SPI.h>
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi Credentials
char ssid[] = "I";
char pass[] = "adriianka";

// Pin Definitions
#define TURBIDITY_PIN A0
#define ONE_WIRE_BUS 2

// OneWire and DallasTemperature setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempSensor;

BlynkTimer timer;

void sendSensor()
{
    int rawValue = analogRead(TURBIDITY_PIN);
    float voltage = rawValue * (5.0 / 1023.0);
    float ntu = mapVoltageToNTU(voltage);

    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);
    
    // Send sensor values to Blynk
    Blynk.virtualWrite(V1, ntu);
    Blynk.virtualWrite(V2, temperatureC);
}

void setup()
{
    Serial.begin(9600);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    sensors.begin();
    
    timer.setInterval(1000L, sendSensor);
}

void loop()
{
    Blynk.run();
    timer.run();
}

float mapVoltageToNTU(float voltage)
{
    return (voltage - 1.2) * (1000 / (4.5 - 1.2));
}
