// Global variables
const AQICN_TOKEN = "YOUR_TOKEN_HERE"; // Replace with your actual AQICN API token
const SPREADSHEET_ID = '1se7rWs55vOttLGmBw_8x33rYeCIS2SM_FHJAbLdqBNM'; // Your spreadsheet ID

// Handle GET requests from Arduino
function doGet(e) { 
  Logger.log(JSON.stringify(e));  // view parameters

  var result = 'Ok'; // assume success

  if (e === undefined || e.parameter === undefined) {
    Logger.log("No parameters received");
    result = 'No Parameters';
    return ContentService.createTextOutput(result);
  }
  
  // Get the spreadsheet and sheet
  var sheet = SpreadsheetApp.openById(SPREADSHEET_ID).getActiveSheet();
  
  // Check if headers exist, if not add them
  if (sheet.getLastRow() === 0) {
    setupHeaders(sheet);
  }
  
  var newRow = sheet.getLastRow() + 1;
  var rowData = [];
  
  // Column A: Timestamp
  rowData[0] = new Date();
  
  // Process Arduino parameters - Extract turbidity and temperature
  if (e.parameter.turbidity !== undefined) {
    rowData[1] = stripQuotes(e.parameter.turbidity); // Turbidity in column B
  } else {
    rowData[1] = ""; 
  }
  
  if (e.parameter.temperature !== undefined) {
    rowData[2] = stripQuotes(e.parameter.temperature); // Temperature in column C
  } else {
    rowData[2] = ""; 
  }
  
  // Get Sligo air quality data and add to rowData
  try {
    const aqData = fetchAirQualityData();
    
    // Add air quality data to columns D-H
    rowData[3] = aqData.aqi || "N/A";                    // AQI
    rowData[4] = aqData.pm25 || "N/A";                   // PM2.5
    rowData[5] = aqData.pm10 || "N/A";                   // PM10
    rowData[6] = aqData.temperature || "N/A";            // Temperature from API
    rowData[7] = aqData.humidity || "N/A";               // Humidity
    
    Logger.log("Air quality data fetched successfully");
  } catch (error) {
    // Handle API errors by placing "Error" in the AQI field
    Logger.log("Error fetching air quality data: " + error);
    rowData[3] = "Error";
    rowData[4] = "";
    rowData[5] = "";
    rowData[6] = "";
    rowData[7] = "";
  }
  
  Logger.log("Row data to be written: " + JSON.stringify(rowData));
  
  // Write new row to the sheet
  sheet.getRange(newRow, 1, 1, rowData.length).setValues([rowData]);
  
  // Return result of operation
  return ContentService.createTextOutput("Success: Data logged with AQI information");
}

// Fetch air quality data from AQICN API for Sligo
function fetchAirQualityData() {
  const city = "sligo";
  const endpoint = `https://api.waqi.info/feed/${city}/?token=${AQICN_TOKEN}`;
  
  try {
    const response = UrlFetchApp.fetch(endpoint);
    const data = JSON.parse(response.getContentText());
    
    // Check if the API returned valid data
    if (data.status === "ok") {
      Logger.log("API response: " + JSON.stringify(data));
      
      // Extract relevant data
      return {
        aqi: data.data.aqi,
        pm25: data.data.iaqi.pm25 ? data.data.iaqi.pm25.v : "N/A",
        pm10: data.data.iaqi.pm10 ? data.data.iaqi.pm10.v : "N/A",
        temperature: data.data.iaqi.t ? data.data.iaqi.t.v : "N/A",
        humidity: data.data.iaqi.h ? data.data.iaqi.h.v : "N/A"
      };
    } else {
      Logger.log("API returned error: " + data.status);
      throw new Error("API returned: " + data.status);
    }
  } catch (error) {
    Logger.log("Error fetching air quality data: " + error);
    throw error;
  }
}

// Set up spreadsheet headers
function setupHeaders(sheet) {
  const headers = [
    "Time", 
    "Turbidity", 
    "Temperature (Arduino)", 
    "Air Quality Index",
    "PM2.5",
    "PM10",
    "Temperature (API)",
    "Humidity"
  ];
  
  sheet.appendRow(headers);
  sheet.getRange("A1:H1").setFontWeight("bold").setHorizontalAlignment("center");
  sheet.setFrozenRows(1);
}

// Function to get the latest air quality data independently
// This can be triggered on a time-based schedule
function updateAirQualityData() {
  var sheet = SpreadsheetApp.openById(SPREADSHEET_ID).getActiveSheet();
  
  try {
    const aqData = fetchAirQualityData();
    
    // Create a new row with timestamp and AQI data only
    var rowData = [
      new Date(),    // Time
      "",            // Leave turbidity blank
      "",            // Leave temperature blank
      aqData.aqi,    // AQI
      aqData.pm25,   // PM2.5
      aqData.pm10,   // PM10
      aqData.temperature, // Temperature 
      aqData.humidity     // Humidity
    ];
    
    // Add the new row
    sheet.appendRow(rowData);
    Logger.log("Air quality data updated independently");
  } catch (error) {
    Logger.log("Failed to update air quality data: " + error);
  }
}

// Remove leading and trailing quotes
function stripQuotes(value) {
  return value.replace(/^["']|['"]$/g, "");
}

// For testing in the script editor
function testAirQuality() {
  const result = fetchAirQualityData();
  Logger.log(JSON.stringify(result));
}
