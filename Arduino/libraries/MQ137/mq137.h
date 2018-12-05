/*
	All code of this library was written by Aswinth Raj.
	Code retrieved at Feb 09, 2018 on https://circuitdigest.com/microcontroller-projects/arduino-mq137-ammonia-sensor 
*/

#ifndef mq137_h
#define mq137_h

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

namespace AnalogSensor {
class GAS_MQ137
{
	public:
		GAS_MQ137(uint8_t pin);
		float read();
		float getVoltage();
	private:
		uint8_t _pin;
};	
}
#endif