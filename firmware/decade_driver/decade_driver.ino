#include "hardware_definitions.h" 

void setup() {	

	init_pins();
	Serial.println(">");
}


void loop() {

	monitor_serial();
	
	// for (int i=0; i<65535; i++){
	// 	wideShiftOut(65535 - i);
	// 	delay(300);
	// }
}
