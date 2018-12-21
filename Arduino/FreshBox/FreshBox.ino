/*
 * ****************************************************
 *  Project FreshBox                                  *
 *  Course: Next Generation User Interface: 2018-2019 *
 *  Authors:                                          *
 *  - Quartier Simon                                  *   
 *  - Schautteet Adrien                               *
 *  - Joachim Alvarez-Rodriguez                       *
 * ****************************************************  
*/

// Libraries
#include <dht.h>           // temp + hum sensor lib
#include <LiquidCrystal.h> // LCD lib
#include <FastLED.h>       // led matrix lib

// ------------------------------------------------------------------
// Configuration of the box :
// done via the mobile application, wifi shield was not available 
// Extra code for the connection is required 

#define CONTENT_MEAT 0
#define CONTENT_FISH 1
#define CONTENT_VEGE 2

uint8_t box_contents;

void set_box_contents(String cont) {
  if (cont == "Meat") {
    box_contents = CONTENT_MEAT;
  }
  else if (cont == "Fish") {
    box_contents = CONTENT_FISH;
  }
  else if (cont == "Vegetables") {
    box_contents = CONTENT_VEGE;
  }
}

// -------------------------------------------------------------------
// Hardware Initialisation: 
// - Pin definitions
// - Globals to store the values
// - Setup code

// Gas = methane/ammonia/hydrogen_sulfide dependent on the box content
uint16_t gas;

// -------------------------------
// Sensors: MQ4, MQ136, MQ137, DHT

// MQ4: methane: NH4
#define MQ4_AO_PIN 0
uint16_t methane = 0;
#define METHANE_NORMAL_VALUE 500

// MQ136: hydrogen sulfide: Sn02
#define MQ136_AO_PIN 2
uint16_t hydrogen_sulfide = 0; 

// MQ137: NH3
#define MQ137_AO_PIN 1
uint16_t ammonia = 0;

// DHT: Temperature + Humidity
#define DHT11_PIN 7
#define TYPE_HUMI 501
#define TYPE_TEMP 502
dht DHT;
int chk;
double temperature;
double humidity;

// ----------------------------
// UI: LCD, LED Matrix, Ambient

// LCD
const uint8_t rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String status_text1; // Message on first row of LCD
String status_text2; // Message on second row of LCD

// LED
#define NR_OF_LEDS 64 // 8x8 Led Matrix
#define LED_BRIGHTNESS 20 // Keep the brightness of the LED Matrix Leds Low (these are super bright)!
CRGB leds[NR_OF_LEDS]; // 8x8 Led Matrix
int initColumn[16] = {0,1,  8,9,  16,17,  24,25,  32,33,  40,41,  48,49,  56,57}; // Col Offsets

// AMBIENT LEDS
#define RED_LED_PIN 8
#define GREEN_LED_PIN 9
#define BLUE_LED_PIN 10

#define STATUS_GOOD 0
#define STATUS_WARNING 1
#define STATUS_BAD 2

uint8_t current_status = STATUS_GOOD;

// Photoresistor
#define PHOTORESISTOR1_PIN 14
#define PHOTORESISTOR2_PIN 15
#define LIGHT_THRESHOLD 250 // Threshold for a light environment
uint16_t photovalue1 = 0;  // light sensor value [0,1023]. 
uint16_t photovalue2 = 0;  // light sensor value [0,1023].

// old_light_env is used to keep track of a change of environment (light/dark) of the box
bool old_light_env = false;
bool light_env = false; 

// -------------------------------------------------------------------
// Setup

void setup() {
  setupLcd();
  setupLedMatrix();
  setupAmbientLeds();
}

// LCD Setup: number of columns and rows
void setupLcd() {
  lcd.begin(16, 2);
}

// LED Matrix Setup in library
void setupLedMatrix() {
  FastLED.addLeds<NEOPIXEL, 6>(leds, NR_OF_LEDS); 
  FastLED.setBrightness(LED_BRIGHTNESS);
}

// Set RGB LED pins to OUTPUT pins
void setupAmbientLeds() {
  pinMode(RED_LED_PIN,   OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN,  OUTPUT);
}

// -------------------------------------------------------------------
// Program

void loop() {
  // Read Sensor Data
  DHT.read11(DHT11_PIN);
  temperature = DHT.temperature;
  humidity = DHT.humidity;

  switch(box_contents) {
    case CONTENT_MEAT:
      methane = getMethane();
      gas = methane;
      break;
    case CONTENT_FISH:
      ammonia = getAmmonia();
      gas = ammonia;
      break;
    case CONTENT_VEGE:
      hydrogen_sulfide = getHydrogenSulfide();
      gas = hydrogen_sulfide;
      break;
    default: break;
      
  }

  // photoresistor data
  light_env = is_light_env();

  // Logic
  SetBoxStatus(temperature, humidity, gas);

  // UI
  if (light_env) {
    // Light env: show 
    showLCD(status_text1, status_text2);
    showLEDMatrix(temperature, humidity, gas);
    runAmbientLeds(current_status);
  } else {
    if (old_light_env) {
      // do nothing as the box stays in the same (off) state
    }
    else {
      // Turn the LCD and Led Matrix Off
      // Future Work: Turn off LCD/ Dim background (this is extremely difficult to find)
      lcd.clear();
      showLEDMatrix(0, 0, 0);
    }
  }
  // set old status to current status 
  old_light_env = light_env;

  // Delay of 2000 is good for demo practices, in real life this should be a lot longer (10000+)
  delay(2000);
}

// -------------------------------------------------------------------
// Program Logic 

// --------------------------------------------------
// Logic
// This function determines the state of the box 
// Temperature, Humidity and Gas values are compared
// Importance Order: Gas > Humidity > Temperature

void SetBoxStatus(int tmp, int hum, int gas) {
  // check if any values exceeds the threshold value
  if (hum >= 75 || gas >= 75) {
    current_status = STATUS_BAD;
    status_text2 = "Discard contents"; 

    if (tmp >= 75 && hum >= 75 && gas >= 75) {
      status_text1 = "Condition bad!";
    } else if (gas >= 75) {
        status_text1 = "High Gas Concentration";
    } else if (hum >= 75) {
        status_text1 = "High humidity";
    } else if (tmp >= 75) {
        status_text1 = "High temperature";
    }
 // Warning Values + recommendation
  } else if (tmp >= 50 || hum >= 50 || gas >= 50) { //
    current_status = STATUS_WARNING;
    status_text1 = "Warning";
    if (tmp >= 50 && hum >= 50 && gas >= 50) {
      status_text2 = "Discard contents";
    } else if (tmp >= 50) {
        status_text2 = "Cool contents";
    } else {
        status_text2 = "Consume now";
    }
  } else { // default when ok
    current_status = STATUS_GOOD;
    status_text1 = "OK";
    status_text2 = "Safe to consume";
  }
}

// -------
// Sensors
// Reading Raw Sensor data (Voltage):
// - Easier to measure fluctuations applied to this project
// - No libraries exist at the moment (math is out of the score of this course)
// - 

// NH4
uint16_t getMethane() {
  methane = analogRead(MQ4_AO_PIN);
  return map(methane, 400, 700, 0, 100);
}
// Sn02
uint16_t getHydrogenSulfide() {
  hydrogen_sulfide = analogRead(MQ136_AO_PIN);
  return map(hydrogen_sulfide, 400, 700, 0, 100);
}

// NH3
uint16_t getAmmonia() {
  ammonia = analogRead(MQ137_AO_PIN);
  return map(ammonia, 400, 700, 0, 100);
}

// -------------------------------------
// User Interface
// LCD, LED Matrix, Ambient Illuminating

// ---
// LCD

// Shows 2 text messages on LCD screen
void showLCD(String status_text1, String status_text2) {
  lcd.clear(); // remove old text
  lcd.setCursor(0,0); // start writing on the firt char of the first line
  lcd.print(status_text1); // write condition
  lcd.setCursor(0,1); // start writing on the firt char of the second line
  lcd.print(status_text2); // write status msg
}

// -----------
// LED Matrix


// Visualise Data on LED Matrix: 3 columns + 2 empty columns
void showLEDMatrix(int temperature, int humidity, int gas) {
  fillColumn(0, ((int) temperature)*2); // show temp on led matrix
  fillColumn(1, (int) humidity); // show humidity on led matrix
  fillColumn(2, gas); // show gas values on led matrix
}


// columnNr: 0-1-2 (width 2, space 1)
// amount: 0-100
void fillColumn(uint16_t columnNr, uint16_t amount){
  // correct amount if needed
  amount = min(100, amount);
  amount = max(0, amount);
  
  // fill according to input
  int ledAmount = (int) (16 * (amount / 100.0));

  // loop over all leds of column & check if needs to be lit or not
  for(uint8_t h = 0; h < 16; h++){
    if(h < ledAmount){
      colorLed(initColumn[h]+(columnNr*3), amount);
    }
    else{
      leds[initColumn[h]+(columnNr*3)] = CRGB(00,00,00);
      FastLED.show();
    }
  }
}

void colorLed(uint8_t ledID, int amount){
  uint8_t maxi = 255;
  // calculate % between red & green
  leds[ledID] = CRGB((maxi*amount/100),(maxi*(100-amount)/100),00);
  FastLED.show();
}

// ------------------
// Photoresistor Code

// Determines if the box is in a illuminated environment
// Only if both sensors sense light
bool is_light_env(){
  photovalue1 = analogRead(PHOTORESISTOR1_PIN);
  photovalue2 = analogRead(PHOTORESISTOR2_PIN);
  return ((photovalue1 > LIGHT_THRESHOLD) || (photovalue2 > LIGHT_THRESHOLD));
}

// ------------------
// Ambient Code 
// 3 statuses can be shown: Green | Orange | Red
// Red status has to blink to be extra clear for the user
// ! 220 Ohm Resistor is used instead of 270 Ohm Resistor
// Light Values are kept somewhat lower due to this

void runAmbientLeds(uint8_t status) {
  switch(status) {
    case STATUS_GOOD:
      setColor(0, 150, 0); // Green Color
      break;
    case STATUS_WARNING:
      setColor(100, 25, 0); // Orange Color
      break;
    case STATUS_BAD:
      setColor(150, 0, 0); // Red Color
      break;
  }
}

// Arduino does not support multiple threads such that blinking is somewhat a hack
void setColor(uint8_t redValue, uint8_t greenValue, uint8_t blueValue) {
  if ((blueValue == 0) && (greenValue == 0)) { // BLINK for Red
     analogWrite(RED_LED_PIN,   redValue);
     analogWrite(GREEN_LED_PIN, greenValue);
     analogWrite(BLUE_LED_PIN,  blueValue);
     delay(100); // Set small delay to provide a blinking experience
     // Turn led off
     analogWrite(RED_LED_PIN,   0);
     analogWrite(GREEN_LED_PIN, 0);
     analogWrite(BLUE_LED_PIN,  0);
  }
  else { // Ambient for Orange or Green
    analogWrite(RED_LED_PIN,   redValue);
    analogWrite(GREEN_LED_PIN, greenValue);
    analogWrite(BLUE_LED_PIN,  blueValue);
  }
}
