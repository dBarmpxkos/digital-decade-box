#include "hardware_definitions.h" 
unsigned long resVal = 1, resHigh = 128;

void setup() {

  init_pins();
  sound_feedback_itsalive();

}

void loop() {
	// char inputBuffer[100] = {'\0'};
	// unsigned long resVal;
	// monitor_serial(inputBuffer);
	// resVal = strtoul(inputBuffer, NULL, 0);
	// if (resVal) // wideShiftOut(resVal);
	// simpleShiftOut(resVal);
}

