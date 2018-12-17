#include "mq137.h"

namespace AnalogSensor {

	GAS_MQ137::GAS_MQ137(uint8_t pin) {
		this->_pin = pin;
	}
	
	const float RL = 47; //The value of resistor RL is 47K
	const float Ro = 20; //Enter found Ro value
	const float m  = -0.263; //Enter calculated Slope 
	const float b  = 0.42; //Enter calculated intercept

	float GAS_MQ137::getVoltage() {
		float VRL = analogRead(this->_pin)*(5.0/1023.0); //Measure the voltage drop and convert to 0-5V
		return VRL;
	}
	
	float GAS_MQ137::read() {
		float VRL = this->getVoltage();
  		float Rs = ((5.0*RL)/VRL)-RL; //Use formula to get Rs value
  		float ratio = Rs/Ro;  // find ratio Rs/Ro
 
  		float ppm = pow(10, ((log10(ratio)-b)/m)); //use formula to calculate ppm
		return ppm;
	}
};