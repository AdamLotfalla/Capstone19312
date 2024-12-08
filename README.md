# Technical report

![Full prototype image](https://github.com/user-attachments/assets/558f5677-3928-454f-80e0-fbe090bcd433)

## Notes on the figure
1. The EPD supports 4 danger icons and makes the text of the danger parameter red

| Icon | Description | Usage |
| --- | --- | --- |
| ⚙ | Indicates sensor fault | Used with the DHT22 when the sensor is not connected properly and displays nan |
| ▲ | Indicates higher sensor readings than the upper threashold | used with DHT22 and TSL2561 to indicate high temperature, humidity, or light levels |
| ▼ | Indicates lower sensor readings than the lower threashold | used with DHT22 to indicate low temperature or humidity |
| ⚠ | Indicates danger; not high or low to a certain threashold | used when more people enter the tomb than recommended |

2. The outer PIR sensor is 11 cm from the entrance, and the two PIRs are put 22 cm apart
3. The ceiling of room J can be opened and the system is put there.
4. The buzzer has 2 tones
   - A continuous tone due to the trigger of the TSL2561's interrupt because a flashlight was opened.
   - A 3-step tone that indicates sensor high value danger-if present-each cycle.
5. The LED is set currently to ligth up when the ESP is connected to Wi-Fi on the start of the code. However, this functionality does not count for Wi-Fi disconnection in the middle of code due to time constraints

   - To make this code from the ```void setup(){}``` to the start of the ```void loop(){}```
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
5. Restart visual studio
6. Open the PlatformIO tab on the left bar
7. In the panel below, find **Open** and select it
8. A tab will open. Select **Open Project**
9. Browse for the extracted folder and open it
10. Wait for it to open
11. Open [Google Sheets](sheets.google.com) and make a new sheet
12. Go to **Extensions > Apps Script**
13. Set up your file and copy the code from the file called **Apps script** in the folder you downloaded

## Modifying the code
1. Go to **Src > main.cpp** file in the file explorer tab on the top of the left bar
2. Under **Wi-Fi definitions**, change the ssid and password between the quotations
```diff
- Make sure that your Wi-Fi is only 2.4Ghz and not 2.4/5Ghz; otherwise, it won't work!
```
3. In the Apps Script page, edit the **sheet_id** and **sheet_name** fom your Google Sheet. (The sheet id is the part of the link after the **https://docs.google.com/spreadsheets/d/**)
5. If your want to use another timezone, edit lines **13** and **14**. It should look like that:
```js
// Date in column A
  Utilities.formatDate(new Date(), /* put your timezone like this: "Africa/Cairo" */, 'M/d/yyyy'),
// Time in column B
  Utilities.formatDate(new Date(), /* put your timezone like this: "Africa/Cairo" */, 'HH:mm:ss')  
```
6. Click the **Deploy** button above, fill the required information and copy the deployment link
7. Go to **Src > main.cpp** on line **258**. Replace the first string part of the variable **URL** with the deployment link you got like this:
```c++
  URL = "/* your deployment code here */" + String(temperature) + "&humd=" + String(humidity) +
      "&npeople=" + String(People_count) + "&Light=" + String(Light);
```
8. Go to lines from 64 to 69 to edit reading threasholds
```c++
const int Temperature_upper_threashold = 21;
const int Temperature_lower_threashold = 16;
const int Humidity_upper_threashold = 60;
const int Humidity_lower_threashold = 40;
const int People_count_upper_threashold = 50;
const int Light_upper_threashold = 500;
```
## Running the code
1. connect a USB between the ESP32's port and your computer
```diff
- Make sure that your USB cable supports data transfer, not only power
```
If you are not sure if your cable supports it or not: connect your ESP normally, go to Windows search and search for **Device Manager**, open it and expand the **Ports** list. If the list changes content after your remove/plug in your USB, then you are good to go. If not, your cable cannot upload code and can only power up the ESP. 

2. On VS code, click on the upload symbol "→" on the left of the bottom bar.
3. Wait for it to finish uploading

Once your upload was done, your system should be good to go and could be connected to the power bank 
