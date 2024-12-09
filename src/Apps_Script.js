function doGet(e) {
    var result = 'Ok';
    if (!e.parameter || e.parameter == 'undefined') {
      return ContentService.createTextOutput('No Parameters');
    }
  
    const sheet_id = '1S6_IL7yn1GuPK6xeMa3itCuH75X0g4UemVOapFCJXmc';
    const sheet_name = "First_sheet";
    const sheet_target = SpreadsheetApp.openById(sheet_id).getSheetByName(sheet_name);
    const newRow = sheet_target.getLastRow() + 1;
    
    const rowDataLog = [
      Utilities.formatDate(new Date(), "Africa/Cairo", 'M/d/yyyy'), // Date in column A
      Utilities.formatDate(new Date(), "Africa/Cairo", 'HH:mm:ss')    // Time in column B
    ];
    
    rowDataLog[2] = e.parameter.temp || '';  // Temperature in column C
    rowDataLog[3] = e.parameter.humd || '';  // Humidity in column D
    rowDataLog[4] = e.parameter.npeople || '';  // Number of people in column E
    rowDataLog[5] = e.parameter.Light || ''; // Light intensity in column F
  
    if (e.parameter.sts === 'write') {
      sheet_target.getRange(newRow, 1, 1, rowDataLog.length).setValues([rowDataLog]);
    }
    return ContentService.createTextOutput(result);
  }
  