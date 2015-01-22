// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int controlLed = 3;

int latchPin = 5;
int clockPin = 6;
int dataPin = 4;
 
byte leds = 0;
byte ledBitCheck = 0;
int currentSecond = 0;


// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(controlLed, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(controlLed, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(controlLed, LOW);    // turn the LED off by making the voltage LOW
  
  leds = 0;
  updateShiftRegister();
  currentSecond ++;
  
  if(currentSecond == 60)
    currentSecond = 0;
    
  for (int i = 0; i < 8; i++)
  {
    ledBitCheck = 0;
    bitSet(ledBitCheck, i);
    
    if((ledBitCheck & currentSecond) > 0){
      bitSet(leds, i);
    }
    updateShiftRegister();
  }
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}

