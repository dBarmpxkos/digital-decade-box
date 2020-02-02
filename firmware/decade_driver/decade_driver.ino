#include "hardware_definitions.h" 

void setup() {

  init_pins();

}

void loop() {
	char inputBuffer[100] = {'\0'};
	unsigned long resVal;
	monitor_serial(inputBuffer);
	resVal = strtoul(inputBuffer, NULL, 0);
	if (resVal) wideShiftOut(resVal);

}

