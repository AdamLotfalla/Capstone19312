#include <GxEPD2_3C.h>
#include <Adafruit_Sensor.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <queue>
#include <Adafruit_TSL2561_U.h> //https://github.com/jacobstim/Adafruit_TSL2561
#include <Wire.h>
#include "pitches.h"
// #include <freertos/FreeRTOS.h>
// #include <task.h>

//---------------------------------------_Wi-Fi definitions_---------------------------------------//

#define WIFI_SSID "Redmi Note 10S"
#define WIFI_PASSWORD "0987654321"
//Yurichi, 12348765
//MSTF El-Khaliji, Flashfrien@511
//Oh Biscuits, F9heeler@55
//Redmi Note 10S, 0987654321

//---------------------------------------_TSL2561 interrupt adresses_---------------------------------------//

#define TSL2561_ADDR 0x39 // Default I2C address for TSL2561
#define COMMAND_BIT 0x80  // Command bit to specify register

// TSL2561 Registers
#define THRESHLOWLOW 0x02
#define THRESHLOWHIGH 0x03
#define THRESHHIGHLOW 0x04
#define THRESHHIGHHIGH 0x05
#define INTERRUPT_CONTROL 0x06
#define COMMAND_CLEAR_INTERRUPT 0xC0

//---------------------------------------_EPD definitions_---------------------------------------//

#define MAX_DISPLAY_BUFFER_SIZE 800
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
#define EPD_CS SS
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
#define GxEPD2_DRIVER_CLASS GxEPD2_290_C90c //GDEM029C90  128x296, SSD1680, (FPC-7519 rev.b)
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=5*/ EPD_CS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

//---------------------------------------_Pin definitions_---------------------------------------//

#define PIR_in_pin 27
#define PIR_out_pin 26
#define led 33
#define buzzer 32
#define TSL2561_INT_PIN 13

//---------------------------------------_Variable declarations_---------------------------------------//

U8G2_FOR_ADAFRUIT_GFX  displayFont;
DHT DHT_SENSOR(25,DHT22);
Adafruit_TSL2561_Unified TSL = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
sensors_event_t event;
String URL;

short int Temperature_danger = 0;
short int Humidity_danger = 0;
bool People_count_danger = false;
bool Light_danger = false;

const int Temperature_upper_threshold = 24;
const int Temperature_lower_threshold = 16;
const int Humidity_upper_threshold = 60;
const int Humidity_lower_threshold = 40;
// const int People_count_upper_threshold = 50; //uncomment if you want an upper limit for people count
const int Light_upper_threshold = 100;

bool DHT_fault = false;
bool TSL2561_fault = false;

int People_count = 0;
std::queue <bool> gateLog; //0 = inside, 1 = outside

float Light;
float Light_Buffer;
float prevTemp;
float prevHumidity;
float prevLight;
int prevPeople_count;

//---------------------------------------_Interrupt handlers_---------------------------------------//

void innerPIRtrigger (){
  gateLog.push(0);
}

void outerPIRtrigger(){
  gateLog.push(1);
}


//---------------------------------------_Functions_---------------------------------------//

void displaySensorDetails(void)
{
  sensor_t sensor;
  TSL.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" lux");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" lux");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" lux");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void configureSensor(void)
{
  TSL.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

  TSL.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */

  /* Configure interrupt thresholds */
  // First: convert lux value to raw sensor value, using "sunlight" approximation.
  // Other approximations, see Adafruit_TSL2561_U.h
  uint16_t threshold = TSL.calculateRawCH0(Light_upper_threshold, TSL2561_APPROXCHRATIO_LED);
  TSL.setInterruptThreshold(0,threshold);
  // Serial.println(threshold);

  /* Enable level interrupt, trigger interrupt after 5 integration times
     -> Because integration time is 13ms by default, this means the threshold
        needs to be exceeded for more than 65ms before the interrupt is triggered.
        Maximum value is 15; put to 1 for immediate triggering; put to 0 to have
        the interrupt triggered after every integration time (regardless of whether
        the thresholds were exceeded or not)
  */
  TSL.setInterruptControl(TSL2561_INTERRUPTCTL_LEVEL, 1);
  TSL.clearLevelInterrupt();
}

void DangerTone() {
  // Define a danger tone melody with sharp high and low alternating notes
  int melody[] = {NOTE_B7, NOTE_F7, NOTE_B7, NOTE_F7, NOTE_B7, NOTE_F7};
  int noteDurations[] = {8, 8, 8, 8, 8, 8}; // Short duration for each note
  
  for (int repeat = 0; repeat < 1; repeat++) { // Repeat the sequence 3 times for urgency
    for (int thisNote = 0; thisNote < 6; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(buzzer, melody[thisNote], noteDuration);
      delay(noteDuration * 1.3); // Add a slight pause between notes
      noTone(buzzer); // Stop the tone before the next note
    }
    // delay(300); // Add a short pause between sequences
  }
}

void WarningTone(){
    // Warning melody notes and durations
    int warningMelody[] = {
      NOTE_A5, NOTE_B5, NOTE_A5, NOTE_G5
    };
    int noteDurations[] = {
      8, 8, 8, 4
    };

    // Play the warning melody
    for (int i = 0; i < 4; i++) {
      int noteDuration = 1000 / noteDurations[i]; // Calculate note duration
      tone(buzzer, warningMelody[i], noteDuration); // Play the note
      delay(noteDuration * 1.3); // Add a short pause after each note
      noTone(buzzer); // Turn off the buzzer
    }      
}

//------------------------------------_FreeRTOS tasks_-----------------------------------//

void SensorPoll_Task (void* parameter){
  vTaskDelay(2000);
  configureSensor();
  Serial.println("Task started");
  for(;;){

    TSL.getEvent(&event);
    Light = event.light;

    Serial.println(Light);

    if(Light > Light_upper_threshold){
      tone(buzzer, 1000, 750);
    }
    vTaskDelay(900);
  }
}

//------------------------------------_Framework funcitons_------------------------------------//
void setup(){
  display.init(115200, true, 2, false);
  Serial.begin(115200);
  displayFont.begin(display);  //connect u8g2 procedures to Adafruit GFX
  DHT_SENSOR.begin();

  pinMode(PIR_in_pin, INPUT_PULLDOWN);
  pinMode(PIR_out_pin, INPUT_PULLDOWN);
  pinMode(TSL2561_INT_PIN, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(10, OUTPUT);

  digitalWrite(led, LOW);
  digitalWrite(buzzer, HIGH);

  displaySensorDetails();
  // configureSensor();
  
  if(!TSL.begin())
  {
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  display.setRotation(1);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.println();

  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(led, HIGH);
  }
  else{
    digitalWrite(led, LOW);
  }

  setCpuFrequencyMhz(80);

  attachInterrupt(digitalPinToInterrupt( PIR_in_pin ), innerPIRtrigger ,RISING);
  attachInterrupt(digitalPinToInterrupt( PIR_out_pin ), outerPIRtrigger ,RISING);


  xTaskCreate(
    SensorPoll_Task, //function name
    "Task 1", //task name
    2048, //stack size
    NULL, //task parameters
    5, //task priority
    NULL
  );

}

void loop() {


  float temperature = DHT_SENSOR.readTemperature();
  float humidity = DHT_SENSOR.readHumidity();
  Light_Buffer = Light;
  // TSL.getEvent(&event);
  // Light = event.light;

  // Serial.println(Light);

  while(gateLog.size() >= 2){
    bool first = gateLog.front();
    gateLog.pop();
    bool second = gateLog.front();
    gateLog.pop();

    if(first == 0 && second == 1){People_count--;}
    else if( first == 1 && second == 0){People_count++;}
  }

  URL = "https://script.google.com/macros/s/AKfycbxK6v36wEHI8-qAQuePUMdTTxE2sqFKfAbY9ps76FSZiG8xHWi6HVeGCcv41KrRYW7F/exec?sts=write&temp=" + String(temperature) + "&humd=" + String(humidity) + "&npeople=" + String(People_count) + "&Light=" + String(Light_Buffer);

  HTTPClient http;
  http.begin(URL.c_str());
  // http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // commented out to remove extra delay 
  int httpCode = http.GET();
  String payload;
  if (httpCode > 0) {
    payload = http.getString();
    // Serial.println("Payload : " + payload);    
  }

  http.end();

  if(temperature > Temperature_upper_threshold){ Temperature_danger = 1;}else if(temperature < Temperature_lower_threshold){ Temperature_danger = -1;}else{Temperature_danger = 0;}
  if(humidity > Humidity_upper_threshold){ Humidity_danger = 1;}else if(humidity < Humidity_lower_threshold){ Humidity_danger = -1;}else{Humidity_danger = 0;}
  // if(People_count >= People_count_upper_threshold){ People_count_danger = true;}else{ People_count_danger = false;} //uncomment if you want an upper limit for people count
  if(isnan(temperature) || isnan(humidity)){DHT_fault = true;}else{DHT_fault = false;}
  if(Light_Buffer > Light_upper_threshold){Light_danger = true;}else{Light_danger = false;}
  if(Light_Buffer == 0){TSL2561_fault = true;}else{TSL2561_fault = false;}

  if(Temperature_danger || Humidity_danger || People_count_danger || Light_danger || TSL2561_fault || DHT_fault){
    DangerTone();
  }

  //--------------------_Display_--------------------
  if(temperature != prevTemp || humidity != prevHumidity || People_count != prevPeople_count || Light_Buffer != prevLight){
    display.firstPage();
    displayFont.setBackgroundColor(GxEPD_WHITE);

    do {
      displayFont.setForegroundColor(Temperature_danger || DHT_fault? 0xF800: 0x0000);
      display.fillScreen(GxEPD_WHITE);
      displayFont.setFont(u8g2_font_fub17_tr);
      displayFont.setCursor(25,30);
      displayFont.printf("Temperature: %.2f", temperature);
      displayFont.setFont(u8g2_font_cu12_t_symbols);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
      displayFont.print(" ℃");

      if(DHT_fault){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,27, '\u0047');
      }
      else if(Temperature_danger == 1){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,27, '\u007B'); 
      }
      else if(Temperature_danger == -1){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,27, '\u007D');
      }
      //-----------------------
      displayFont.setForegroundColor(Humidity_danger || DHT_fault? 0xF800: 0x0000);
      displayFont.setFont(u8g2_font_fub17_tr);
      displayFont.setCursor(25,55);
      displayFont.printf("Humidity: %.2f", humidity);
      displayFont.setFont(u8g2_font_cu12_t_symbols);
      displayFont.print(" %");

      if(DHT_fault){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,52, '\u0047');
      }
      else if(Humidity_danger == 1){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,52, '\u007B');
      }
      else if(Humidity_danger == -1){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,52, '\u007D');
      }
      //------------------------
      displayFont.setForegroundColor(People_count_danger? 0xF800: 0x0000);
      displayFont.setFont(u8g2_font_fub17_tr);
      displayFont.setCursor(25,80);
      displayFont.printf("People count: %i", People_count);
      displayFont.setFont(u8g2_font_cu12_t_symbols);
      // displayFont.printf("/ %i", People_count_upper_threshold); //uncomment if you want an upper limit for people count

      if(People_count_danger){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,77, '\u0021');
      }
      //------------------------
      displayFont.setForegroundColor(Light_danger || TSL2561_fault? 0xF800: 0x0000);
      displayFont.setFont(u8g2_font_fub17_tr);
      displayFont.setCursor(25,105);
      displayFont.printf("Light intensity: %.0f", Light_Buffer);
      displayFont.setFont(u8g2_font_cu12_t_symbols);
      displayFont.printf(" lux");

      if(TSL2561_fault){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,105, '\u0047');
      }
      else if(Light_danger){
        displayFont.setFont(u8g2_font_twelvedings_t_all);
        displayFont.drawGlyph(8,105, '\u0021');
      }


    }while(display.nextPage());

    display.hibernate();
  }
  //--------------------_End of display_--------------------

  prevTemp = temperature;
  prevHumidity = humidity;
  prevPeople_count = People_count;
  prevLight = Light_Buffer;

  auto time = millis();
  while(time % 30000 >= 50){time = millis();}

}
