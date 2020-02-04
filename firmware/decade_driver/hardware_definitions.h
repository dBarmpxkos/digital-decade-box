int latchPin  = 4;  /* pin 12 on the 74hc595 */
int dataPin	  = 2;  /* pin 14 on the 74hc595 */
int clockPin  = 5;  /* pin 11 on the 74hc595 */
int enablePin = 3; 	/* pin 13 on the 74hc595 */

bool init_pins(){
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	pinMode(enablePin, OUTPUT);
	digitalWrite(enablePin, LOW);
 	Serial.begin(9600);
}

bool monitor_serial(char *inStream) {
	unsigned int i = 0;

    if (Serial.available() > 0) {
        char cmd = Serial.read();
        /* if first char is indicating that settings are coming */
        if (cmd == '>') {   
            Serial.println(F("[ser]: settings flag received"));
            while (!Serial.available()){ /* wait for host payload */
                delay(20);
            }
            while (Serial.available() > 0){
           		inStream[i++] = Serial.read();
            }
   			return true;
        }
    }

    return false;
}

bool extract_ohms(char *inStream, unsigned int &value, const char range){
	char resBuffer[4] = {'\0'};
	switch (range){
		case 0:	/* extract ohm */
			for (char i=0; i<3; i++){
				resBuffer[i] = inStream[i];
			}
			sscanf(resBuffer, "%d", &value);
		break;

		case 1: /* extract kohm */
			for (char i=0; i<3; i++){
				resBuffer[i] = inStream[i+3];
			}
			sscanf(resBuffer, "%d", &value);
		break;

		case 2: /* extract Mohm */
			for (char i=0; i<3; i++){
				resBuffer[i] = inStream[i+6];
			}
			sscanf(resBuffer, "%d", &value);
		break;

		case 3: /* extract Gohm */
			for (char i=0; i<3; i++){
				resBuffer[i] = inStream[i+9];
			}
			sscanf(resBuffer, "%d", &value);
		break;

		default:
		break;
	}
	return true;
}

bool wideShiftOut(const unsigned long resVal){

	 digitalWrite(latchPin, LOW);
	 for (char i = 0; i < 4; i++) {
		 shiftOut(dataPin, clockPin, MSBFIRST, highByte(resVal << i*4));
		 Serial.print("HIGH: "); Serial.println(highByte(resVal << i*4), BIN);
		 shiftOut(dataPin, clockPin, MSBFIRST, lowByte(resVal << i*4));
		 Serial.print("LOW: "); Serial.println(lowByte(resVal << i*4), BIN);
		 Serial.println();
	}
	 digitalWrite(latchPin, HIGH);

}

bool simpleShiftOut(const char resVal){
	 
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, resVal);
	digitalWrite(latchPin, HIGH);

}

bool sound_feedback_itsalive(void){
	simpleShiftOut(255);
	delay(3000);
	simpleShiftOut(0);
}