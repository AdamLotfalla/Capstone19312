#include <GxEPD2_3C.h>
#include <Adafruit_Sensor.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <DHT.h>


#define MAX_DISPLAY_BUFFER_SIZE 800
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#define EPD_CS SS
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
#define GxEPD2_DRIVER_CLASS GxEPD2_290_C90c //GDEM029C90  128x296, SSD1680, (FPC-7519 rev.b)
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

#define PIR_in_pin 27
#define PIR_out_pin 26



U8G2_FOR_ADAFRUIT_GFX  u8g2Fonts;
DHT DHT_SENSOR(25,DHT22);
short int Temperature_danger = 0;
short int Humidity_danger = 0;
bool People_count_danger = false;
const int Temperature_upper_threashold = 21;
const int Temperature_lower_threashold = 16;
const int Humidity_upper_threashold = 60;
const int Humidity_lower_threashold = 40;
bool DHT_fault = false;
volatile int People_count = 0;
const int People_count_upper_threashold = 5;

void count_up(){
  People_count++;
}

void count_down(){
  People_count--;
}

void setup()
{
  display.init(115200, true, 2, false);
  Serial.begin(115200);
  u8g2Fonts.begin(display);  //connect u8g2 procedures to Adafruit GFX
  DHT_SENSOR.begin();
  pinMode(PIR_in_pin, INPUT_PULLDOWN);
  pinMode(PIR_out_pin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt( PIR_in_pin ), count_up ,RISING);
  attachInterrupt(digitalPinToInterrupt( PIR_out_pin ), count_down ,RISING);

  display.setRotation(1);

}

void loop() {

  setCpuFrequencyMhz(80);
  float temperature = DHT_SENSOR.readTemperature();
  float humidity = DHT_SENSOR.readHumidity();
  

  if(temperature > Temperature_upper_threashold){ Temperature_danger = 1;}else if(temperature < Temperature_lower_threashold){ Temperature_danger = -1;}else{Temperature_danger = 0;}
  if(humidity > Humidity_upper_threashold){ Humidity_danger = 1;}else if(humidity < Humidity_lower_threashold){ Humidity_danger = -1;}else{Humidity_danger = 0;}
  if(People_count >= People_count_upper_threashold){ People_count_danger = true;}else{ People_count_danger = false;}
  if(isnan(temperature) || isnan(humidity)){DHT_fault = true;}else{DHT_fault = false;}

  //-----------------------

  display.firstPage();
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);

  do {
    u8g2Fonts.setForegroundColor(Temperature_danger || DHT_fault? 0xF800: 0x0000);
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setFont(u8g2_font_fub17_tr);
    u8g2Fonts.setCursor(25,30);
    u8g2Fonts.printf("Temperature: %.2f", temperature);
    u8g2Fonts.setFont(u8g2_font_cu12_t_symbols);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2Fonts.print(" ℃");

    if(DHT_fault){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,27, '\u0047');
    }
    else if(Temperature_danger == 1){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,27, '\u007B'); 
    }
    else if(Temperature_danger == -1){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,27, '\u007B');
    }
    //-----------------------
    u8g2Fonts.setForegroundColor(Humidity_danger || DHT_fault? 0xF800: 0x0000);
    u8g2Fonts.setFont(u8g2_font_fub17_tr);
    u8g2Fonts.setCursor(25,55);
    u8g2Fonts.printf("Humidity: %.2f", humidity);
    u8g2Fonts.setFont(u8g2_font_cu12_t_symbols);
    u8g2Fonts.print(" %");

    if(DHT_fault){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,52, '\u0047');
    }
    else if(Humidity_danger == 1){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,52, '\u007B');
    }
    else if(Humidity_danger == -1){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,52, '\u007D');
    }
    //------------------------
    u8g2Fonts.setForegroundColor(People_count_danger? 0xF800: 0x0000);
    u8g2Fonts.setFont(u8g2_font_fub17_tr);
    u8g2Fonts.setCursor(25,80);
    u8g2Fonts.printf("People count: %i", People_count);
    u8g2Fonts.setFont(u8g2_font_cu12_t_symbols);
    u8g2Fonts.printf("/ %i", People_count_upper_threashold);

    if(People_count_danger){
      u8g2Fonts.setFont(u8g2_font_twelvedings_t_all);
      u8g2Fonts.drawGlyph(8,77, '\u0021');
    }

    
  }while(display.nextPage());
  display.hibernate();

  auto time = millis();
  // esp_sleep_enable_timer_wakeup((30000 - ((int)time % 30000))*1000);
  // esp_sleep_enable_ext0_wakeup((gpio_num_t)17,HIGH);
  // esp_sleep_enable_ext0_wakeup((gpio_num_t)7,HIGH);
  // esp_light_sleep_start();
  
while(time % 30000 > 50){time = millis();}


}

