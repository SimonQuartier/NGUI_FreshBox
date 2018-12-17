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
const int TYPE_HUMI = 501;
const int TYPE_TEMP = 502;

String status_text1;
String status_text2;

// LED MATRIX
const uint8_t LED_BRIGHTNESS = 30;
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

// AMBIENT LEDS
int redPin= 8;
int greenPin = 9;
int bluePin = 10;

const int STATUS_GOOD = 100;
const int STATUS_WARNING = 200;
const int STATUS_BAD = 300;

int current_status = STATUS_GOOD;

// MQ Sensors
const int mq4out = 0;
const int mq136out = 2;
const int mq137out = 1;

int mq4value;
int mq136value;
int mq137value;

int METHANE_NORMAL_VALUE = 500;

// Dummy var
uint8_t dummy = 0;

// -----------------------

void setup() {
  Serial.begin(115200);
  setupLcd();
  setupLedMatrix();
  setupAmbientLeds();
}

void loop() {
  runLcdAndLedMatrix();
  runAmbientLeds(current_status);
}

void setupLcd() {
  // LCD Setup: number of columns and rows:
  lcd.begin(16, 2);
}

void setupLedMatrix() {
  // LED Screen Setup in library
  FastLED.addLeds<NEOPIXEL, 6>(leds, 64); 
  FastLED.setBrightness(LED_BRIGHTNESS);
}

void setupAmbientLeds() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void runAmbientLeds(int status) {
	switch(status) {
	case STATUS_GOOD:
	  setColor(0, 50, 0); // Green Color
	  break;
	case STATUS_WARNING:
	  setColor(100, 25, 0); // Orange Color
	  break;
	case STATUS_BAD:
	  setColor(255, 0, 0); // Red Color
	  break;
	}
}

void runLcdAndLedMatrix() {
	int chk = DHT.read11(DHT11_PIN);
	double temperature = DHT.temperature;
	double humidity = DHT.humidity;
	bool surround = isSurround();

  int methane = getMethane();

  colorAmbientLeds(temperature, humidity, methane);
  setStatusText(temperature, humidity, methane);

	if (surround) {
		// LED Matrix
		// dummy = (dummy + 10) % 110;
		// Show on Matrix
		fillColumn(0, ((int) temperature)*2);
		fillColumn(1, (int) humidity);
		fillColumn(2, methane);

		// Write on LCD
		lcd.clear();
		lcd.setCursor(0,0); 
    lcd.print(status_text1);
    lcd.setCursor(0,1);
    lcd.print(status_text2);
//		lcd.print("Temp: ");
//		lcd.print(temperature);
//		lcd.print((char)223);
//		lcd.print("C");

//		lcd.setCursor(0,1);
//		lcd.print("Humidity: ");
//		lcd.print(humidity);
//		lcd.print("%");
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
	delay(2000);
}

bool isSurround(){
  photovalue1 = analogRead(PHOTORESISTOR1_PIN);
  photovalue2 = analogRead(PHOTORESISTOR2_PIN);
  return lightSurrounding(photovalue1, photovalue2);
}

// MQ-4 Sensor
int getMethane(){
  mq4value = analogRead(mq4out);
  Serial.print("Methane value: ");
  Serial.println(mq4value);//prints the methane value
  return map(mq4value, 400, 700, 0, 100);
}

void runMq136(){
  mq4value = analogRead(mq136out);
  Serial.print("Hydrogen Sulfide value: ");
  Serial.println(mq136value);//prints the methane value
}

void runMq137(){
  mq4value = analogRead(mq137out);
  Serial.print("Ammonia value: ");
  Serial.println(mq137value);//prints the methane value
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
      leds[initColumn[h]+(columnNr*3)] = CRGB(00,00,00);
      FastLED.show();
    }
  }
}

// LED Matrix
void colorLed(int ledID, int amount){
  uint8_t maxi = 255;
  // calculate % between red & green
  leds[ledID] = CRGB((maxi*amount/100),(maxi*(100-amount)/100),00);
  FastLED.show();
}

// Ambient leds
void colorAmbientLeds(int tmp, int hum, int gas) {
  if (tmp >= 75 || hum >= 75 || gas >= 75) {
    current_status = STATUS_BAD;
  } else if (tmp >= 50 || hum >= 50 || gas >= 50) {
    current_status = STATUS_WARNING;
  } else {
    current_status = STATUS_GOOD;
  }
}

void setStatusText(int tmp, int hum, int gas) {
  if (tmp >= 75 && hum >= 75 && gas >= 75) {
    status_text1 = "Condition bad!";
    status_text2 = "Discard contents";
  } else if (tmp >= 75) {
    status_text1 = "High temperature";
    status_text2 = "Discard contents";
  } else if (hum >= 75) {
    status_text1 = "High humidity";
    status_text2 = "Discard contents";
  } else if (gas >= 75) {
    status_text1 = "High methane";
    status_text2 = "Discard contents";
  } else if (tmp >= 50 && hum >= 50 && gas >= 50) {
    status_text1 = "Warning";
    status_text2 = "Discard contents";
  } else if (tmp >= 50) {
    status_text1 = "Warning";
    status_text2 = "Cool contents";
  } else if (hum >= 50) {
    status_text1 = "Warning";
    status_text2 = "Consume now";
  } else if (hum >= 50) {
    status_text1 = "Warning";
    status_text2 = "Consume now";
  } else {
    status_text1 = "OK";
    status_text2 = "Safe to consume";
  }
}

// AMBIENT LEDS
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(bluePin, blueValue);
  if ((blueValue == 0) && (greenValue == 0)) {
     analogWrite(greenPin, greenValue);
     analogWrite(redPin, redValue);
     analogWrite(greenPin, greenValue);
     analogWrite(bluePin, blueValue);
     delay(100);
     analogWrite(redPin, 0);
     analogWrite(greenPin, 0);
     analogWrite(bluePin, 0);
  }
  else {
    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
  }
}
