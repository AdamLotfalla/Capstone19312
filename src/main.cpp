#include <GxEPD2_3C.h>
#include <Adafruit_Sensor.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <DHT.h>


#define MAX_DISPLAY_BUFFER_SIZE 800
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#define EPD_CS SS
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
#define GxEPD2_DRIVER_CLASS GxEPD2_290_C90c // GDEM029C90  128x296, SSD1680, (FPC-7519 rev.b)
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));


U8G2_FOR_ADAFRUIT_GFX  u8g2Fonts;
DHT DHT_SENSOR(25,DHT22);
bool Temperature_danger = false;
bool Humidity_danger = false;
bool People_count_danger = false;
const int Temperature_upper_threashold = 30;
const int Temperature_lower_threashold = 15;
const int Humidity_upper_threashold = 60;
const int Humidity_lower_threashold = 20;

void setup()
{
  display.init(115200, true, 2, false);
  Serial.begin(115200);
  u8g2Fonts.begin(display);  // connect u8g2 procedures to Adafruit GFX
  DHT_SENSOR.begin();

}

void loop() {

  float temperature = DHT_SENSOR.readTemperature();
  float humidity = DHT_SENSOR.readHumidity();
  if(temperature > Temperature_upper_threashold){ Temperature_danger = true;}else if(temperature < Temperature_lower_threashold){ Temperature_danger = true;}else{Temperature_danger = false;}

  if(humidity > (float)Humidity_upper_threashold){ Humidity_danger = true;}else if(humidity < (float)Humidity_lower_threashold){ Humidity_danger = true;}else{Humidity_danger = false;}

  // Serial.printf("Humidity: %i\n Temperature: %i\n", (int)Humidity_danger, (int)Temperature_danger);

  //-----------------------
  display.setRotation(1);
  display.firstPage();
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

  do {
    u8g2Fonts.setForegroundColor(Temperature_danger? 0xF800: 0x0000);
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_fub17_tr);
    u8g2Fonts.setCursor(25,30);
    u8g2Fonts.printf("Temperature: %.2f", temperature);
    u8g2Fonts.setFont(u8g2_font_cu12_t_symbols);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2Fonts.println(" ℃");

    if(Temperature_danger){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,27, '\u0021');
    }
    //-----------------------
    u8g2Fonts.setForegroundColor(Humidity_danger? 0xF800: 0x0000);
    u8g2Fonts.setFont(u8g2_font_fub17_tr);
    u8g2Fonts.setCursor(25,55);
    u8g2Fonts.printf("Humidity: %.2f", humidity);
    u8g2Fonts.setFont(u8g2_font_cu12_t_symbols);
    u8g2Fonts.println(" %");

    if(Humidity_danger){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,27, '\u0021');
    }
    
  }while(display.nextPage());

  delay(17000);

}