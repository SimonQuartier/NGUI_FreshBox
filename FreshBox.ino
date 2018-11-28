
// Libraries
#include <dht.h>
#include <LiquidCrystal.h>
#include <ledmatrix.h>
#include <FastLED.h>

// -----------------------
// Hardware Initialisation

// LCD Init
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// DHT Init
dht DHT;
#define DHT11_PIN 7

const int LED_BRIGHTNESS = 20;
//Ledmatrix LEDMatrix;

// In library
//LED Matrix: 8x8 Init
CRGB leds[64];
int initColumn[16] = {0,1,  8,9,  16,17,  24,25,  32,33,  40,41,  48,49,  56,57};

// Dummy var
int dummy = 0;

// -----------------------

void setup() {
  // LCD Setup: number of columns and rows:
  lcd.begin(16, 2);
  //LEDMatrix.init(LED_BRIGHTNESS);


  // LED Screen Setup in library
  FastLED.addLeds<NEOPIXEL, 6>(leds, 64); 
  FastLED.setBrightness(20);
}



void loop() {
  // Read DHT data
  int chk = DHT.read11(DHT11_PIN);
  double temp = DHT.temperature;
  double hum = DHT.humidity;
  int temp_int = (int) temp;
  int hum_int = (int) hum;

  dummy = (dummy + 10) % 100;
  // Show on Matrix
  fillColumn(0, temp_int*2);
  fillColumn(1, hum_int);
  fillColumn(2, dummy);

  // Write on LCD
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(hum);
  lcd.print("%");

  // Execute Loop every 2 seconds
  delay(2000);
}

// -----------------------
// Matrix Code in library


// columnNr: 0-1-2 (width 2, space 1)
// amount: 0-100
void fillColumn(int columnNr, int amount){
  // reset column first
  clearColumn(columnNr);
	
  // fill according to input
  int ledAmount = (int) (16 * (amount / 100.0));
  // even out led amount for square bars
  ledAmount = ledAmount % 2 == 0 ? ledAmount : ledAmount + 1; 
  for(int h = 0; h < ledAmount; h++){
    colorLed(initColumn[h]+(columnNr*3), amount);
  }
}

void clearColumn(int columnNr){
  for(int h : initColumn){
    leds[h+(columnNr*3)] = CRGB(00,00,00); FastLED.show();
  }
}

// LED
void colorLed(int ledID, int amount){
  int perc = amount/25;

  if(perc < 1){
    leds[ledID] = CRGB(00,255,00); FastLED.show();
  }
  else if(perc < 2){
    leds[ledID] = CRGB(85,170,00); FastLED.show();
  }
  else if(perc < 3){
    leds[ledID] = CRGB(170,85,00); FastLED.show();
  }
  else{
    leds[ledID] = CRGB(255,00,00); FastLED.show();
  }
}
