#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#include "GxEPD2_display_selection_new_style.h"

// alternately you can copy the constructor from GxEPD2_display_selection.h or GxEPD2_display_selection_added.h to here
// e.g. for Wemos D1 mini:
//GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEH0154D67

// for handling alternative SPI pins (ESP32, RP2040) see example GxEPD2_Example.ino

const char HelloWorld[] = "Hello World!";

void Show_Partial(String Input, int x, int y, int width, int height){

  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setPartialWindow(x,y,width,height);
  display.firstPage();
  display.setTextColor(GxEPD_WHITE);

  do
  {
    display.fillRect(x,y,width,height, GxEPD_BLACK);
    //display.hasFastPartialUpdate(); // is a read method
    display.setCursor(x, y+20);
    display.print(Input);
    delay(50);
  }
  while (display.nextPage());

}

void setup()
{
  //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.firstPage();
  do{
    display.fillRect(0,0,296,23,GxEPD_BLACK);
    display.setCursor(70,15);
    display.print("Hello Nigga! :)");
    delay(50);
  }
  while(display.nextPage());

  display.hibernate();
}

void loop() {};
