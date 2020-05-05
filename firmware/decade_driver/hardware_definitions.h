/* 74HC595 Pins:
 * latch: 4
 * data:  2
 * clock: 5
 * enable: 3
 */

#include <ArduinoJson.h>

#define ledPin 13
#define latchPin   	4
#define dataPin  	2
#define clockPin  	5
#define enablePin  	3



typedef struct {

    char* uid;
    uint32_t resistorValue;
    uint16_t shiftRegisterValue;

} resistorStructure;

/* --- base floor --- */
resistorStructure RES1;
resistorStructure RES2;
resistorStructure RES3;
resistorStructure RES4;
resistorStructure RES5;
resistorStructure RES6;
resistorStructure RES7;

/* --- upper floor --- */
resistorStructure RES8;
resistorStructure RES9;
resistorStructure RES10;
resistorStructure RES11;
resistorStructure RES12;
resistorStructure RES13;
resistorStructure RES14;
/* --- upper floor --- */


/* helper functions ------------------------------------------ */
static void yield_on_char(void){
    if ( serialEventRun ) serialEventRun();
    if ( Serial.available() ) return;
}

static void flush_serial_input(void){
    if ( serialEventRun ) serialEventRun();
    do { 
        Serial.read(); 
        delay(50); 
    } while ( Serial.available() > 0 );
}
 
static void blink_LED(void){
    digitalWrite(ledPin, !digitalRead(ledPin));
}

/* helper functions ------------------------------------------ */
bool init_pins(){
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	pinMode(enablePin, OUTPUT);
	digitalWrite(enablePin, LOW);
 	Serial.begin(9600);
}

void init_resistor(resistorStructure *resLoad, char* name, uint32_t value, uint16_t shiftValue){

    resLoad->uid = name;
    resLoad->resistorValue = value;
    resLoad->shiftRegisterValue = shiftValue;

}


/* 
## R1: 1.980k
## R2: 3.81k
## R3: 8.11k
## R4: 8.06k
## R5: 32.5k
## R6: 148.8k
## R7: 511k
## 
## Upper floor:
## R8: 2.22m
## R9: 3.85m
## R10: 4.81m
## R11: 9.79m
## R12: 11.52m
## R13: 10.63m
## R14: 9.99m
*/

void init_all_resistors(){

    init_resistor(&RES1, "R1", 1980, 2);
    init_resistor(&RES2, "R2", 3810, 4);
    init_resistor(&RES3, "R3", 8110, 8);
    init_resistor(&RES4, "R4", 8060, 16);
    init_resistor(&RES5, "R5", 32500, 32);
    init_resistor(&RES6, "R6", 148800, 64);
    init_resistor(&RES7, "R7", 511000, 128);
    init_resistor(&RES8, "R8", 2220000, 512);
    init_resistor(&RES9, "R9", 3850000, 1024);
    init_resistor(&RES10, "R10", 4810000, 2048);
    init_resistor(&RES11, "R11", 9790000, 4096);
    init_resistor(&RES12, "R12", 11520000, 8192);
    init_resistor(&RES13, "R13", 10630000, 16384);
    init_resistor(&RES14, "R14", 9990000, 32768);
}


bool wideShiftOut(const unsigned long resVal){

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, highByte(resVal));
    shiftOut(dataPin, clockPin, MSBFIRST, lowByte(resVal));
    digitalWrite(latchPin, HIGH);
    return true;

}

bool simpleShiftOut(const char resVal){
     
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, resVal);
    digitalWrite(latchPin, HIGH);
    return true;

}

bool sound_feedback_itsalive(void){
    wideShiftOut(1);
    delay(500);
    wideShiftOut(65534);
    delay(500);
}

/* json parser for settings */
bool settings(void) {
    StaticJsonDocument<200> JSONSettings;

    DeserializationError error = deserializeJson(JSONSettings, Serial);
    if (error) {
        Serial.print(F("[err]: deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    bool enable = JSONSettings["en"];
    Serial.print(F("[JSONSettings][en]: ")); Serial.println(enable);
    if (!enable)
        Serial.println("Disabled OUTPUT");
    
    else {
        uint32_t resVal = JSONSettings["resVal"];
 		Serial.print(F("[JSONSettings][resVal]: ")); Serial.println(resVal);
        uint16_t shiftRegisterIncoming = JSONSettings["shiftVal"];
        Serial.print(F("[JSONSettings][shiftVal]: ")); Serial.println(shiftRegisterIncoming);
        wideShiftOut(shiftRegisterIncoming);
        blink_LED();

    }
}


bool monitor_serial(void) {
    int8_t settingsType = -1;

    if (Serial.available() > 0) {
        char cmd = Serial.read();
        /* if first char is indicating that settings are coming */
        if (cmd == '>') {   
            Serial.println(F("[ser]: settings flag received"));
            while (!Serial.available()){ /* wait for host payload */
                delay(20);
            }
 			cmd = Serial.read();
            /* read next char; R: resVal settings */
            switch (cmd) {
                case 'R':
                    settingsType = 0;
                    settings();
                    Serial.println(F("[ser][run]: settings()"));
                    break;
                default:
                    Serial.println(F("[err]: error in settings type"));
                    break;
            }
        }
    }
    return settingsType;
}
