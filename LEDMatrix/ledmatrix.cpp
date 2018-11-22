// ledmatrix.cpp

#include <ledmatrix.h>

// Public
void Ledmatrix::init(int brightness)
{
  FastLED.addLeds<NEOPIXEL, 6>(leds, NUMBER_OF_LEDS); 
  FastLED.setBrightness(brightness);
}

void Ledmatrix::fillColumn(int columnNr, int amount)
{
	for(int h = 0; h < ((int)(16 * (amount/100.0))); h++) 
	{
    	_colorLed(initColumn[h]+(columnNr*3), amount);
  	}
}

// Private
void Ledmatrix::_colorLed(int ledID, int amount) 
{
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