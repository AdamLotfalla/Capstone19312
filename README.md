# Technical report

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
5. If your want to use another timezone, edit lines **13** and **14**
6. Click the **Deploy** button above, fill the required information and copy the deployment link
7. Go to **Src > main.cpp** on line **258**. Replace the first string part of the variable **URL** with the deployment link you got.

## Running the code
1. connect a USB between the ESP32's port and your computer
```diff
- Make sure that your USB cable supports data transfer, not only power
```
If you are not sure if your cable supports it or not: connect your ESP normally, go to Windows search and search for **Device Manager**, open it and expand the **Ports** list. If the list changes content after your remove/plug in your USB, then you are good to go. If not, your cable cannot upload code and can only power up the ESP. 
2. On VS code, click on the upload symbol "→" on the left of the bottom bar.
3. Wait for it to finish uploading

Once your upload was done, your system should be good to go and could be connected to the power bank 
