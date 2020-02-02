int latchPin = 4;  // pin 12 on the 74hc595
int dataPin = 2;   // pin 14 on the 74hc595
int clockPin = 5;  // pin 11 on the 74hc595
int enablePin = 3; // pin 13 on the 74hc595
uint32_t i = 0xFFFF;

void setup() {

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
  Serial.begin(9600);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, highByte(i));
  shiftOut(dataPin, clockPin, MSBFIRST, lowByte(i));
  digitalWrite(latchPin, HIGH);

}

void loop() {



}
