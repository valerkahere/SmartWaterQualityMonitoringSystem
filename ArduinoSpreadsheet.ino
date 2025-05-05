#include "WiFiS3.h"
#include <WiFiSSLClient.h> // Use WiFiSSLClient for HTTPS

// WiFi Credentials
char ssid[] = "YOUR_WIFI_NAME"; // <-- Put your WiFi name here
char pass[] = "yOUR_WIFI_PASSWORD"; // <-- Put your WiFi password here

int status = WL_IDLE_STATUS; // the WiFi radio's status

// Google Apps Script Web App URL details
const char* host = "script.google.com";
const int httpsPort = 443;
// MAKE SURE this is your deployed script URL (after new deployment)
String scriptPath = "/macros/s/YOUR_SCRIPT_URL/exec"; // <-- PASTE SCRIPT URL

// --- DEFINE YOUR THRESHOLDS HERE ---
const long TURBIDITY_CLEAR_THRESHOLD = 50; // Example: Turbidity < 50 is "clear"
const float TEMP_COLD_THRESHOLD = 15.0;   // Example: Temp < 15C is "cold"
const float TEMP_GOOD_MAX_THRESHOLD = 25.0; // Example: Temp <= 25C (and >=15C) is "good"
// Temperatures > TEMP_GOOD_MAX_THRESHOLD will be "warm"
// --- END OF THRESHOLDS ---


void setup() {
  Serial.begin(9600);
  delay(1000);  // Short delay to stabilize

  // Check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  // Attempt to connect to WiFi network:
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// MODIFIED: Added 'finalResult' parameter
void sendDataToSheet(long turbidity, float temperature, String safetyStatus) {
  WiFiSSLClient client;
  
  Serial.println("\n------------------------------");
  Serial.print("Connecting to ");
  Serial.println(host);
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }
  
  // MODIFIED: Added 'safetyStatus' parameter to the URL
  String url = scriptPath + "?turbidity=" + String(turbidity) +
               "&temperature=" + String(temperature) +
               "&safetyStatus=" + safetyStatus; // Added safetyStatus parameter
               
  // Quick URL encoding for safetyStatus (handles potential spaces, though unlikely for SAFE/UNSAFE)
  url.replace(" ", "%20"); 

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
    
    if (line == "") { headersDone = true; }
    
    if (headersDone && line.length() > 0) {
      Serial.println("BODY: " + line);
    }
  }
  
  Serial.println("Connection closed");
  client.stop();
}

void loop() {
  // Generate sensor data (fake values)
  long turbidity = random(0, 1000); // Range 0 to 1000
  float temperature = random(0, 100); // Range 0 to 100 C

  Serial.println("\n****************************");
  Serial.println("Generating new sensor data:");
  Serial.print("Turbidity: ");
  Serial.print(turbidity);
  Serial.print(", Temperature: ");
  Serial.println(temperature);

  // --- ADDED: Calculate Status Strings ---
  String turbidityStatus;
  String temperatureStatus;

  // Determine Turbidity Status
  if (turbidity < TURBIDITY_CLEAR_THRESHOLD) {
    turbidityStatus = "clear";
  } else {
    turbidityStatus = "cloudy"; // Assuming anything not clear is cloudy
  }
  Serial.print("Turbidity Status: ");
  Serial.println(turbidityStatus);

  // Determine Temperature Status
  if (temperature < TEMP_COLD_THRESHOLD) {
    temperatureStatus = "cold";
  } else if (temperature <= TEMP_GOOD_MAX_THRESHOLD) {
    temperatureStatus = "good";
  } else {
    temperatureStatus = "warm";
  }
  Serial.print("Temperature Status: ");
  Serial.println(temperatureStatus);
  // --- END OF Status Calculation ---


  // --- IMPLEMENTED: Your Analysis Snippet ---
  String finalResult; // This will hold SAFE, NOT IDEAL, or UNSAFE
  Serial.print("Overall Analysis: ");
  if (turbidityStatus == "clear" && temperatureStatus == "good") {
    finalResult = "SAFE";
    Serial.println("Water is SAFE to drink.");
  } else if (turbidityStatus == "clear" && (temperatureStatus == "cold" || temperatureStatus == "warm")) {
    finalResult = "NOT IDEAL";
    Serial.println("Water is drinkable but temperature is not ideal.");
  } else { // Covers cloudy turbidity OR clear but not good/cold/warm (though that last case is unlikely with current logic)
    finalResult = "UNSAFE";
    Serial.println("Water is UNSAFE to drink.");
  }
  // --- END OF Analysis Snippet ---

  // Send data using Apps Script method
  if (WiFi.status() == WL_CONNECTED) {
    // MODIFIED: Pass the 'finalResult' to the function
    sendDataToSheet(turbidity, temperature, finalResult);
  } else {
    Serial.println("WiFi Disconnected. Attempting to reconnect...");
    status = WiFi.begin(ssid, pass);
  }

  delay(30000); // Send data every 30 seconds
}
