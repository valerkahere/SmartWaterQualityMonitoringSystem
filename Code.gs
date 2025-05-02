// === CONFIGURATION ===
const AQICN_TOKEN = "79dbdddb9819eeaacc4e4c09a9ba7ac5a9b8ecae"; // REPLACE with your new token
const SPREADSHEET_ID = '1se7rWs55vOttLGmBw_8x33rYeCIS2SM_FHJAbLdqBNM';

function doGet(e) { 
  Logger.log(JSON.stringify(e));
  var result = 'Ok';

  var sheet = SpreadsheetApp.openById(SPREADSHEET_ID).getActiveSheet();

  // Add headers if not present
  if (sheet.getLastRow() === 0) {
    setupHeaders(sheet);
  }

  var newRow = sheet.getLastRow() + 1;
  var rowData = [];
  rowData[0] = new Date(); // Timestamp in column A

  // Arduino data
  if (e && e.parameter) {
    rowData[1] = e.parameter.turbidity || "";
    rowData[2] = e.parameter.temperature || "";
  } else {
    rowData[1] = "";
    rowData[2] = "";
  }

  // AQICN Data - Try multiple endpoints for Sligo
  var aqData = tryMultipleAqiEndpoints();
  rowData[3] = aqData.aqi;
  rowData[4] = aqData.pm25;
  rowData[5] = aqData.pm10;
  rowData[6] = aqData.temperature;
  rowData[7] = aqData.humidity;

  sheet.getRange(newRow, 1, 1, rowData.length).setValues([rowData]);
  return ContentService.createTextOutput("Success: Data logged with AQI");
}

// Try multiple possible endpoints for Sligo
function tryMultipleAqiEndpoints() {
  // Array of possible endpoints to try
  const endpoints = [
    `https://api.waqi.info/feed/sligo/?token=${AQICN_TOKEN}`,
    `https://api.waqi.info/feed/ireland/sligo/?token=${AQICN_TOKEN}`,
    `https://api.waqi.info/feed/geo:54.277;-8.474/?token=${AQICN_TOKEN}` // Sligo coordinates
  ];
  
  for (let endpoint of endpoints) {
    try {
      const response = UrlFetchApp.fetch(endpoint, {muteHttpExceptions: true});
      const data = JSON.parse(response.getContentText());
      
      if (data.status === "ok") {
        Logger.log("Successful API call with endpoint: " + endpoint);
        return {
          aqi: data.data.aqi || "N/A",
          pm25: data.data.iaqi.pm25 ? data.data.iaqi.pm25.v : "N/A",
          pm10: data.data.iaqi.pm10 ? data.data.iaqi.pm10.v : "N/A",
          temperature: data.data.iaqi.t ? data.data.iaqi.t.v : "N/A",
          humidity: data.data.iaqi.h ? data.data.iaqi.h.v : "N/A"
        };
      }
      Logger.log("Endpoint failed: " + endpoint + " - Status: " + data.status);
    } catch (err) {
      Logger.log("Error with endpoint " + endpoint + ": " + err);
    }
  }
  
  // If all endpoints fail, return error values
  return {aqi:"API Error", pm25:"N/A", pm10:"N/A", temperature:"N/A", humidity:"N/A"};
}

// Set up spreadsheet headers
function setupHeaders(sheet) {
  const headers = [
    "Time", 
    "Turbidity", 
    "Temperature (Arduino)", 
    "Air Quality Index (Sligo)",
    "PM2.5",
    "PM10",
    "Temperature (API)",
    "Humidity"
  ];
  
  sheet.appendRow(headers);
  sheet.getRange("A1:H1").setFontWeight("bold").setHorizontalAlignment("center");
  sheet.setFrozenRows(1);
}

// Test function to check API endpoints
function testEndpoints() {
  const result = tryMultipleAqiEndpoints();
  Logger.log(JSON.stringify(result));
}
