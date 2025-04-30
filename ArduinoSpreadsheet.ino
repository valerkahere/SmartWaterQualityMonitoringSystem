#include "WiFiS3.h"
#include <WiFiSSLClient.h> // Use WiFiSSLClient for HTTPS

// ---- WiFi Credentials ----
char ssid[] = "crack"; 
char pass[] = "rostiklox";     
// ------------------------------------------

int status = WL_IDLE_STATUS; // the WiFi radio's status

// ---- Google Apps Script Web App URL details ----
const char* host = "script.google.com";
const int httpsPort = 443;
String scriptPath = "/macros/s/AKfycbzpHCkhFseSgTUMGNdR7Zpez8XpEZa1L1rYHbiR1etMdsl3jQNY-aRARy9uNtVypHyWKw/exec";
// -----------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial port to connect

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the WiFi firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void sendDataToSheet(long turbidity, float temperature) {
  WiFiSSLClient client;
  
  Serial.print("Connecting to ");
  Serial.println(host);
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }
  
  // Changed parameter names to match column headers
  String url = scriptPath + "?turbidity=" + String(turbidity) + "&temperature=" + String(temperature);
  
  Serial.println("Full request URL:");
  Serial.println("https://" + String(host) + url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ArduinoWiFi/1.1\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent");
  
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("Closing connection");
  client.stop();
}

void loop() {
  // Generate sensor data (fake values)
  long turbidity = random(0, 100);  // Changed variable name from fakeValue1
  float temperature = random(0, 1000) / 10.0;  // Changed variable name from fakeValue2
  
  Serial.print("Generated sensor data: ");
  Serial.print("Turbidity: ");
  Serial.print(turbidity);
  Serial.print(", Temperature: ");
  Serial.println(temperature);

  // Send data using Apps Script method
  if (WiFi.status() == WL_CONNECTED) {
      sendDataToSheet(turbidity, temperature);
  } else {
      Serial.println("WiFi Disconnected. Cannot send data.");
  }

  delay(60000); // Send data every minute
}
