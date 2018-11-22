#ifndef ledmatrix_h
#define ledmatrix_h

#include <FastLED.h>

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif


#define NUMBER_OF_LEDS 64



class Ledmatrix
{
public:
	void init(int brightness);
	void fillColumn(int columnNr, int amount);

private:
	CRGB leds[NUMBER_OF_LEDS];
	int initColumn[16] = {0,1,  8,9,  16,17,  24,25,  32,33,  40,41,  48,49,  56,57};
	void _colorLed(int ledID, int amount);
};
#endif