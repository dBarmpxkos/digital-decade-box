#include <ArduinoJson.h>

#define ledPin      2
#define latchPin   	18
#define dataPin  	17
#define clockPin  	19
#define enablePin  	5
#define resetPin    4

typedef struct {
    char* uid;
    uint32_t resistorValue;
    uint16_t shiftRegisterValue;
} resistorStructure;

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


/* --- base floor --- */
resistorStructure RES1;
resistorStructure RES2;
resistorStructure RES3;
resistorStructure RES4;
resistorStructure RES5;
resistorStructure RES6;
resistorStructure RES7;
resistorStructure RES8;


static void
yield_on_char(void) {
    if ( serialEventRun ) serialEventRun();
    if ( Serial.available() ) return;
}

static void 
flush_serial_input(void){
    if ( serialEventRun ) serialEventRun();
    do { 
        Serial.read(); 
        delay(50); 
    } while ( Serial.available() > 0 );
}

static void 
blink_LED(void){
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);

}

void 
init_pins(){
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    digitalWrite(enablePin, LOW);
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
}

void 
init_resistor(resistorStructure *resLoad, 
              char* name, 
              uint32_t value, 
              uint16_t shiftValue) {
    resLoad->uid = name;
    resLoad->resistorValue = value;
    resLoad->shiftRegisterValue = shiftValue;
}


void 
init_all_resistors(){
    init_resistor(&RES1, "R1", 330, pow(2,0));
    init_resistor(&RES2, "R2", 330, pow(2,1));
    init_resistor(&RES3, "R3", 330, pow(2,2));
    init_resistor(&RES4, "R4", 330, pow(2,3));
    init_resistor(&RES5, "R5", 330, pow(2,4));
    init_resistor(&RES6, "R6", 330, pow(2,5));
    init_resistor(&RES7, "R7", 330, pow(2,6));
    init_resistor(&RES8, "R8", 330, pow(2,7));
}


bool 
wideShiftOut(const unsigned long resVal){

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, highByte(resVal));
    shiftOut(dataPin, clockPin, MSBFIRST, lowByte(resVal));
    digitalWrite(latchPin, HIGH);
    return true;

}

bool 
simpleShiftOut(const int resVal){

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, resVal);
    digitalWrite(latchPin, HIGH);
    return true;

}

void 
sound_feedback(void){
    for (int k=0;k<3;k++){
        for (int i=0;i<8;i++){
            simpleShiftOut(0xff-pow(2,i));
            delay(30);     
        }  
        delay(100);     
    }
    simpleShiftOut(0);
}

/* json parser for settings */
bool 
settings(void) {
    StaticJsonDocument<200> JSONSettings;

    DeserializationError error = deserializeJson(JSONSettings, Serial);
    if (error) {
        Serial.print(F("[err]: deserializeJson() failed: "));
        Serial.println(error.c_str());
        return false;
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
        simpleShiftOut(shiftRegisterIncoming);
        blink_LED();

    }
    return true;
}


bool
 monitor_serial(void) {
    int8_t settingsType = -1;

    if (Serial.available() > 0) {
        char cmd = Serial.read();
        /* if first char is indicating that settings are coming */
        if (cmd == '>') {   
            Serial.println(F("[ser]: settings flag received"));
            while (!Serial.available()) { /* wait for host payload */
                delay(20);
            }
            cmd = Serial.read();    /* read next char; R: resVal settings */
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