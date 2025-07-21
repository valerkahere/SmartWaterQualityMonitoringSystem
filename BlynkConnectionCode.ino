/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL4SMgA6DPM"
#define BLYNK_TEMPLATE_NAME "Test"
#define BLYNK_AUTH_TOKEN "V3fxHZ7iM3zJPq23uZLT3_rJ8N9BCR9R"


#include <SPI.h>
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "I";
char pass[] = "adriianka";

int pinValue;
BLYNK_WRITE(V0) {
  pinValue = param.asInt();
}

BlynkTimer timer;

void sendSensor()
{
  Blynk.virtualWrite(V1, random(1, 100));
  Blynk.virtualWrite(V2, random(100, 200));
}
void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
  Serial.println(pinValue);
  delay(500);
}
