#include "WiFiS3.h"
#include <WiFiSSLClient.h> // Use WiFiSSLClient for HTTPS

// WiFi Credentials
char ssid[] = "crack"; 
char pass[] = "rostiklox";     

int status = WL_IDLE_STATUS; // the WiFi radio's status

// Google Apps Script Web App URL details
const char* host = "script.google.com";
const int httpsPort = 443;
// Make sure this is your deployed script URL (after new deployment)
String scriptPath = "/macros/s/AKfycbz9aEEitd31oRgB128rYCZxl3UT6cLYrlxCqUJAcS9Ozx_JjanGst9RzSMhPBGkimX-kQ/exec";

void setup() {
  Serial.begin(9600);
  delay(1000);  // Short delay to stabilize

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  // attempt to connect to WiFi network:
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    // wait for connection:
    delay(5000);
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendDataToSheet(long turbidity, float temperature) {
  WiFiSSLClient client;
  
  Serial.println("\n------------------------------");
  Serial.print("Connecting to ");
  Serial.println(host);
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }
  
  // Build URL with parameters
  String url = scriptPath + "?turbidity=" + String(turbidity) + "&temperature=" + String(temperature);
  
  Serial.println("Request URL:");
  Serial.println("https://" + String(host) + url);
  
  // Improved HTTP request with more headers
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ArduinoWiFi/1.1\r\n" +
               "Connection: close\r\n" +
               "\r\n");

  Serial.println("Request sent. Waiting for response...");
  
  // Wait for server response with better timeout handling
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println(">>> Response timeout!");
      client.stop();
      return;
    }
  }

  // Process and display response headers
  Serial.println("Response received:");
  String responseStatus = "";
  bool headersDone = false;
  
  while (client.available()) {
    String line = client.readStringUntil('\n');
    line.trim();
    
    if (responseStatus == "" && line.startsWith("HTTP/1.")) {
      responseStatus = line;
      Serial.println(responseStatus);
    }
    
    // Empty line marks end of headers
    if (line == "") {
      headersDone = true;
    }
    
    // Print response body
    if (headersDone && line.length() > 0) {
      Serial.println("BODY: " + line);
    }
  }
  
  Serial.println("Connection closed");
  client.stop();
}

void loop() {
  // Generate sensor data (fake values)
  long turbidity = random(0, 100);
  float temperature = random(0, 1000) / 10.0;
  
  Serial.println("\n****************************");
  Serial.println("Generating new sensor data:");
  Serial.print("Turbidity: ");
  Serial.print(turbidity);
  Serial.print(", Temperature: ");
  Serial.println(temperature);

  // Send data using Apps Script method
  if (WiFi.status() == WL_CONNECTED) {
    sendDataToSheet(turbidity, temperature);
  } else {
    Serial.println("WiFi Disconnected. Attempting to reconnect...");
    status = WiFi.begin(ssid, pass);
  }

  delay(30000); // Send data every minute
}
