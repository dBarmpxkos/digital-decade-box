#include "hardware_definitions.h" 
byte which_resistor = 0;
byte resistor_state = 0;
int valueToShift = 0;

void 
setup() {	
	init_pins();
	sound_feedback();
	Serial.begin(9600);
}

void 
loop() {
    if (Serial.available() > 0) {
    	which_resistor = Serial.parseInt() - 1;	 /* zero-based indexing ,e.g. first resistor is 0 */

    	char cmd = Serial.read();
        if (cmd == ',') {  
        	resistor_state = Serial.parseInt();
        	Serial.printf("\r\n[>] Resistor %d | state: [%s]", which_resistor + 1, 
        				  						  resistor_state ? "ON" : "OFF");

 			if (resistor_state) {valueToShift |= (0b00000001 << which_resistor);}
 		    else             	{valueToShift &= 0xff - (0b00000001 << which_resistor);}

 		    /* either way */
 			simpleShiftOut(0xff - valueToShift);
			Serial.printf("\r\n[?] "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(valueToShift));
   	    }
	}
}
