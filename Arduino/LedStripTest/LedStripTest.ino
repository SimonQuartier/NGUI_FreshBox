// ports to be used
#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 3
 
#define FADESPEED 5     // make this higher to slow down
 
void setup() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
}

// Dummy var
int dummy = 0;
void loop() {
  lightTest();
//  dummy = (dummy + 10) % 110;
//  setEnvironmentLight(dummy);
}

// use this, value 0-100, 0 = extreme good, 100 = extreme bad
void setEnvironmentLight(int amount){
  // correct amount if needed  
  amount = min(100, amount);
  amount = max(0, amount);
  
  // first get color
  int maxi = 255;

  // calculate % between red & green
  int cRED = (maxi*amount/100);
  int cGREEN = (maxi*(100-amount)/100);

  // ----------------------------------
  // now set a light intensity
}

// only to test the strip
void lightTest(){
  int r, g, b;
 
  // fade from blue to violet
  for (r = 0; r < 256; r++) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from violet to red
  for (b = 255; b > 0; b--) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from red to yellow
  for (g = 0; g < 256; g++) { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
  // fade from yellow to green
  for (r = 255; r > 0; r--) { 
    analogWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from green to teal
  for (b = 0; b < 256; b++) { 
    analogWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  // fade from teal to blue
  for (g = 255; g > 0; g--) { 
    analogWrite(GREENPIN, g);
    delay(FADESPEED);
  }   
}
