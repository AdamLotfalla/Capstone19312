# Technical report

![Full prototype image](https://github.com/user-attachments/assets/558f5677-3928-454f-80e0-fbe090bcd433)

## Notes on the figure
**①** The EPD supports 4 danger icons and makes the text of the danger parameter red

| Symbol | Description | Usage |
| --- | --- | --- |
| ⚙ | Indicates sensor fault | Used with the DHT22 when the sensor is not connected properly and displays **nan** |
| ▲ | Indicates higher sensor readings than the upper threashold | Used with DHT22 and TSL2561 to indicate high temperature, humidity, or light levels |
| ▼ | Indicates lower sensor readings than the lower threashold | Used with DHT22 to indicate low temperature or humidity |
| ⚠ | Indicates danger; not high or low to a certain threashold | Used when more people enter the tomb than recommended |

**②** The outer PIR sensor is 11 cm from the gate, and the two PIRs are put 22 cm apart.

**③** The ceiling of room J can be opened and the system is put there.

**④** The buzzer has 2 tones
   - A continuous tone due to the trigger of the TSL2561's interrupt because a flashlight was opened.
   - A 3-step tone that indicates sensor high value danger-if present-each cycle.

**⑤** The LED is set currently to light up when the ESP is connected to Wi-Fi on the start of the code. However, we made this functionality not count for Wi-Fi disconnection in the middle of code due to time constraints. To make that happen, move the section of code from the ```void setup(){}``` to the start of the ```void loop(){}```
```c++
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(led, HIGH);
  }
  else{
    digitalWrite(led, LOW);
  }
```
## Getting the code ready
1. Install VS Code from [this link](https://code.visualstudio.com/download)
2. Open the program, go to the extensions tab on the left bar, and search for PlatformIO
3. Download PlatformIO and wait for it to complete
4. Download the .zip file from this repository's releases and extract it anywhere you want
5. Restart Visual Studio
6. Open the PlatformIO tab on the left bar
7. In the panel below, find **Open** and select it
8. A tab will open. Select **Open Project**
9. Browse for the extracted folder and open it
10. Wait for it to open
11. Open [Google Sheets](sheets.google.com) and make a new sheet
12. Go to **Extensions > Apps Script**
13. Set up your file and copy the code from the file called **Apps_Script.js** in the folder you extracted

## Modifying the code
1. Go to **Src > main.cpp** file in the file explorer tab on the top of the left bar
2. On the code tab under ```_Wi-Fi definitions_```, change the **SSID** and **password** between quotations
```c++
   #define WIFI_SSID "/* your Wi-Fi SSID here */"
   #define WIFI_PASSWORD "/* your Wi-Fi password here */"
```
```diff
- Make sure that your Wi-Fi is only 2.4Ghz and not 2.4/5Ghz; otherwise, it won't work!
```
3. In the Apps Script page, edit the **sheet_id** and **sheet_name** fom your Google Sheet. (The sheet id is the part of the sheet URL after the **https://docs.google.com/spreadsheets/d/**)
4. If your want to use another timezone, edit lines **13** and **14**. It should look like that:
```js
   Utilities.formatDate(new Date(), /* time zone like this: "Africa/Cairo" */,
   'M/d/yyyy'), // Date in column A 
   Utilities.formatDate(new Date(), /* time zone like this: "Africa/Cairo" */,
   'HH:mm:ss') // Time in column B 
```
5. Click the **Deploy** button above, fill in the required information and copy the deployment link
6. Go to **Src > main.cpp** on line **279**. Replace the first string part of the variable **URL** with the deployment link you got like this:
```c++
  URL = "/* your deployment code here */" + String(temperature) + "&humd=" + String(humidity) + "&npeople=" + String(People_count) + "&Light=" + String(Light);
```
7. Go to lines from **66** to **71** to edit reading threasholds
```c++
const int Temperature_upper_threashold = 24;
const int Temperature_lower_threashold = 16;
const int Humidity_upper_threashold = 60;
const int Humidity_lower_threashold = 40;
const int People_count_upper_threashold = 50;
const int Light_upper_threashold = 100;
```
8. Find using ```Ctrl + F``` the lines with this text ```//uncomment if you want an upper limit for people count``` and optionally uncomment the whole line to put an upper limit to people count (there are 3 lines in total)
## Running the code
1. connect a USB between the ESP32's port and your computer
```diff
- Make sure your USB cable supports data transfer, not only power
```
If you are not sure if your cable supports it or not: connect your ESP normally, go to Windows search and search for **Device Manager**, open it and expand the **Ports** list. If the list changes content after your remove/plug in your USB, then you are good to go. If not, your cable cannot upload code and can only power up the ESP. 

2. On VS code, click on the upload symbol "→" on the left of the bottom bar.
3. Wait for it to finish uploading

Once your upload is done, your system should be good to go and could be connected to the power bank 
