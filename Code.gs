// === CONFIGURATION ===
const AQICN_TOKEN = "YOUR_TOKEN"; // Your token 
const SPREADSHEET_ID = 'YOUR_ID'; // Your Sheet ID

function doGet(e) { 
  Logger.log("Received request: " + JSON.stringify(e)); // Log incoming request
  var result = 'Ok';

  var sheet = SpreadsheetApp.openById(SPREADSHEET_ID).getActiveSheet();

  // MODIFIED: Add "Water Safety" header and adjust column count/check
  const expectedHeaders = ["Date", "Turbidity", "Water Temperature", "Water Safety", "AQI Sligo", "PM2.5 Sligo", "PM10 Sligo", "Temperature Sligo", "Humidity level"]; // Added "Water Safety"
  const headerRange = "A1:I1"; // Now 9 columns
  const headerColumnCount = expectedHeaders.length;

  if (sheet.getLastRow() === 0 || sheet.getRange("A1").getValue() !== expectedHeaders[0] || sheet.getRange("D1").getValue() !== expectedHeaders[3]) {
    // Add headers if first row is empty or first/fourth header doesn't match
    sheet.getRange(headerRange).setValues([expectedHeaders]);
    sheet.getRange(headerRange).setFontWeight("bold").setHorizontalAlignment("center");
    sheet.setFrozenRows(1);
    Logger.log("Headers set/updated.");
  }

  var newRow = sheet.getLastRow() + 1;
  var rowData = [];
  
  // Column A: Timestamp
  rowData[0] = new Date(); 

  // Arduino data
  if (e && e.parameter) {
    // Column B: Turbidity
    rowData[1] = e.parameter.turbidity || ""; 
    // Column C: Temperature (Assuming this was meant to be Temp, header says pH?) - Mapped to pH column based on header
    rowData[2] = e.parameter.temperature || ""; 
    // --- ADDED: Column D: Water Safety Status ---
    rowData[3] = e.parameter.safetyStatus || ""; // Get the safetyStatus parameter
    // --- END ---
  } else {
    rowData[1] = ""; // Turbidity
    rowData[2] = ""; // Temperature/pH
    rowData[3] = ""; // Water Safety
  }

  // AQICN Data - Indices shifted due to added column
  var aqData = tryMultipleAqiEndpoints();
  // Column E: AQI Sligo
  rowData[4] = aqData.aqi;
  // Column F: PM2.5 Sligo
  rowData[5] = aqData.pm25;
  // Column G: PM10 Sligo
  rowData[6] = aqData.pm10;
  // Column H: Temperature Sligo
  rowData[7] = aqData.temperature;
  // Column I: Humidity level
  rowData[8] = aqData.humidity;

  Logger.log("Row data to write: " + JSON.stringify(rowData));

  // Write new row to spreadsheet - MODIFIED: Use headerColumnCount
  var newRange = sheet.getRange(newRow, 1, 1, headerColumnCount);
  newRange.setValues([rowData]);
  
  Logger.log("Data written successfully to row " + newRow);
 
  return ContentService.createTextOutput("Success: Data logged including Safety Status and AQI");
}

// REVISED: Try multiple possible endpoints for Sligo, including sligo-town, with enhanced logging
function tryMultipleAqiEndpoints() {
  const endpoints = [
    // ADDED: The specific endpoint likely tested successfully in the browser
    `https://api.waqi.info/feed/ireland/sligo-town/?token=${AQICN_TOKEN}`, 
    // Existing endpoints
    `https://api.waqi.info/feed/sligo/?token=${AQICN_TOKEN}`,
    `https://api.waqi.info/feed/ireland/sligo/?token=${AQICN_TOKEN}`,
    `https://api.waqi.info/feed/geo:54.277;-8.474/?token=${AQICN_TOKEN}` 
  ];
  
  var lastError = "No endpoints attempted."; // Default error message
  
  Logger.log("Attempting to fetch AQI data using token: " + AQICN_TOKEN); // Log the token being used (verify it looks right)

  for (let i = 0; i < endpoints.length; i++) {
    let endpoint = endpoints[i];
    Logger.log(`Attempting endpoint #${i + 1}: ${endpoint}`); // Log the exact URL
    try {
      // Fetch response WITH status code
      const response = UrlFetchApp.fetch(endpoint, {muteHttpExceptions: true});
      const responseCode = response.getResponseCode();
      const responseText = response.getContentText();
      
      Logger.log(`Endpoint #${i + 1} responded with HTTP status code: ${responseCode}`); // Log the status code
      // Optional: Log the raw response text for debugging, but can be noisy
      // Logger.log(`Endpoint #${i + 1} raw response: ${responseText}`); 

      // Check for successful HTTP status (200 OK) before parsing JSON
      if (responseCode === 200) {
        const data = JSON.parse(responseText);
        // Check if the API's internal status is "ok"
        if (data.status === "ok") {
          Logger.log("Successful API call with endpoint: " + endpoint);
          // Return the extracted data
          return {
            aqi: data.data.aqi || "N/A",
            pm25: data.data.iaqi && data.data.iaqi.pm25 ? data.data.iaqi.pm25.v : "N/A",
            pm10: data.data.iaqi && data.data.iaqi.pm10 ? data.data.iaqi.pm10.v : "N/A",
            temperature: data.data.iaqi && data.data.iaqi.t ? data.data.iaqi.t.v : "N/A",
            humidity: data.data.iaqi && data.data.iaqi.h ? data.data.iaqi.h.v : "N/A"
          };
        } else {
          lastError = `API Error (Status ${data.status}) from endpoint: ${endpoint}`;
          Logger.log(`Endpoint #${i + 1} failed - API Status: ${data.status}`);
        }
      } else {
        // Handle non-200 HTTP responses (like 403 Forbidden, 404 Not Found, 429 Too Many Requests, or 5xx Server Errors)
        lastError = `HTTP Error ${responseCode} from endpoint: ${endpoint}`;
        Logger.log(`Endpoint #${i + 1} failed - HTTP Status: ${responseCode}. Response: ${responseText}`);
        // Specifically check for "Invalid key" in the response if it's a 4xx error
        if (responseCode >= 400 && responseCode < 500 && responseText.includes("Invalid key")) {
           Logger.log(">>> Detected 'Invalid key' response from server for this endpoint.");
           // You could decide to stop trying other endpoints if you get an explicit Invalid Key
           // break; 
        }
      }
    } catch (err) {
      // Catch errors during the fetch itself (e.g., network timeout, DNS error)
      lastError = `Fetch Exception for endpoint ${endpoint}: ${err.message}`;
      Logger.log(`Endpoint #${i + 1} failed - Exception: ${err}`);
    }
  } // End of loop
  
  // If all endpoints fail, log the last encountered error and return the error object
  Logger.log("All endpoints failed. Last error: " + lastError);
  return {aqi:"API Error", pm25:"N/A", pm10:"N/A", temperature:"N/A", humidity:"N/A"};
}



// Test function to check API endpoints (No changes needed here)
function testEndpoints() {
  const result = tryMultipleAqiEndpoints();
  Logger.log(JSON.stringify(result));
}


