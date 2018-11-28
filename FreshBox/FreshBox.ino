
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

  dummy = (dummy + 10) % 110;
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
  // correct amount if needed
  amount = min(100, amount);
  amount = max(0, amount);
  
  // fill according to input
  int ledAmount = (int) (16 * (amount / 100.0));

  // loop over all leds of column & check if needs to be lit or not
  for(int h = 0; h < 16; h++){
	  if(h < ledAmount){
		  colorLed(initColumn[h]+(columnNr*3), amount);
	  }
	  else{
		  leds[initColumn[h]+(columnNr*3)] = CRGB(00,00,00); FastLED.show();
	  }
  }
}

// LED
void colorLed(int ledID, int amount){
	int max = 255;

  // calculate % between red & green
	leds[ledID] = CRGB((max*amount/100),(max*(100-amount)/100),00); FastLED.show();  
}
