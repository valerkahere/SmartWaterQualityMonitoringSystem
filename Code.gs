function doGet(e) { 
  Logger.log(JSON.stringify(e));  // view parameters

  var result = 'Ok'; // assume success

  if (e === undefined || e.parameter === undefined) {
    Logger.log("No parameters received - likely running from editor");
    result = 'No Parameters';
    return ContentService.createTextOutput(result);
  }
  
  // Make sure this ID matches your current spreadsheet
  var id = '1se7rWs55vOttLGmBw_8x33rYeCIS2SM_FHJAbLdqBNM';
  var sheet = SpreadsheetApp.openById(id).getActiveSheet();
  
  // Check if headers exist, if not add them
  if (sheet.getLastRow() === 0) {
    sheet.appendRow(["Time", "Turbidity", "Temperature"]);
    // Format headers to be bold and centered
    sheet.getRange("A1:C1").setFontWeight("bold").setHorizontalAlignment("center");
    sheet.setFrozenRows(1); // Freeze the header row
  }
  
  var newRow = sheet.getLastRow() + 1;
  var rowData = [];
  
  // Add timestamp in Time column
  rowData[0] = new Date();
  
  // Extract parameters with the new names
  if (e.parameter.turbidity !== undefined) {
    rowData[1] = stripQuotes(e.parameter.turbidity); // Turbidity parameter
  } else {
    rowData[1] = ""; // Empty if parameter not provided
  }
  
  if (e.parameter.temperature !== undefined) {
    rowData[2] = stripQuotes(e.parameter.temperature); // Temperature parameter
  } else {
    rowData[2] = ""; // Empty if parameter not provided
  }
  
  Logger.log("Row data to be written: " + JSON.stringify(rowData));
  
  // Write new row below
  var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
  newRange.setValues([rowData]);
  
  // Return result of operation
  return ContentService.createTextOutput("Success: Data logged with headers");
}

/**
* Remove leading and trailing single or double quotes
*/
function stripQuotes(value) {
  return value.replace(/^["']|['"]$/g, "");
}

// Test function for debugging within the editor
function testDoGet() {
  var e = {
    parameter: {
      turbidity: "75",
      temperature: "23.5"
    }
  };
  doGet(e);
}

