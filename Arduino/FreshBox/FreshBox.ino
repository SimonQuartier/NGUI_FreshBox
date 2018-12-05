// Libraries
#include <dht.h>
#include <LiquidCrystal.h>
#include <FastLED.h>

// -----------------------
// Hardware Initialisation

// LCD
const uint8_t rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// DHT
#define DHT11_PIN 7
dht DHT;

int chk;
double temperature;
double humidity;

// LED
const uint8_t LED_BRIGHTNESS = 20;
CRGB leds[64];
int initColumn[16] = {0,1,  8,9,  16,17,  24,25,  32,33,  40,41,  48,49,  56,57};

// Photoresistor
#define PHOTORESISTOR1_PIN 14
#define PHOTORESISTOR2_PIN 15

uint16_t photovalue1 = 0;  // light sensor value [0,1023]. 
uint16_t photovalue2 = 0;  // light sensor value [0,1023]. 

bool old_surround = false;
bool surround = false; 

const uint8_t LIGHT_THRESHOLD = 250; // Threshold for a light surrounding

// Dummy var
uint8_t dummy = 0;

// -----------------------

void setup() {
  // LCD Setup: number of columns and rows:
  lcd.begin(16, 2);

  // LED Screen Setup in library
  FastLED.addLeds<NEOPIXEL, 6>(leds, 64); 
  FastLED.setBrightness(20);
}



void loop() {
  
  // Read Photo Values
  photovalue1 = analogRead(PHOTORESISTOR1_PIN);
  photovalue2 = analogRead(PHOTORESISTOR2_PIN);
  surround = lightSurrounding(photovalue1, photovalue2);

  // Read DHT data
  chk = DHT.read11(DHT11_PIN);
  temperature = DHT.temperature;
  humidity = DHT.humidity;

  if (surround) {
    // LED Matrix
    dummy = (dummy + 10) % 110;
    // Show on Matrix
    fillColumn(0, ((int) temperature)*2);
    fillColumn(1, (int) humidity);
    fillColumn(2, dummy);

    // Write on LCD
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print((char)223);
    lcd.print("C");
    lcd.print(" S");
    lcd.print(surround);
  
    lcd.setCursor(0,1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");
  }
  else {
    if (old_surround) {
      // do nothing as the box stays in the same (off) state
    }
    else {
      lcd.clear();
      fillColumn(0, 0);
      fillColumn(1, 0);
      fillColumn(2, 0);

      // TODO dim LCD
    }
  }
  old_surround = surround;

  // Execute Loop every 2 seconds
  delay(2000);
}

// ------------------
// Photoresistor Code

bool lightSurrounding(uint16_t value1, uint16_t value2) {
  return ((value1 > LIGHT_THRESHOLD) || (value2 > LIGHT_THRESHOLD));
}


// ---------------
// LED Matrix Code

// columnNr: 0-1-2 (width 2, space 1)
// amount: 0-100
void fillColumn(uint16_t columnNr, uint16_t amount){
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
  uint8_t maxi = 255;
  // calculate % between red & green
  leds[ledID] = CRGB((maxi*amount/100),(maxi*(100-amount)/100),00); FastLED.show();
}
