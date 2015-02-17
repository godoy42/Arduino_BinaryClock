// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int controlLed = 3;

int latchPin = 5;
int clockPin = 6;
int dataPin = 4;

int buttonPin = 7;

int mode = 0;
int secondsPressed = 0;
const int secondsToEnterConfig = 5;
boolean buttonPressed = false;
boolean blinkTime = false;
boolean currentBit = false;
 
byte leds = 0;
byte leds2 = 0;
byte ledBitCheck = 0;
int currentSecond = 0;
int currentMinute = 0;
int currentHour = 1;
const int loopDelay = 1000;
const int loopMax = 1000 / loopDelay;
int loopCurrent = 0;

// the setup routine runs once when you press reset:
void setup() {
  Serial.println("Starting.");
  Serial.println("Secondary loop (mode>0) each " + String(loopMax) + " loops. ");
  
  // initialize the digital pin as an output.
  pinMode(controlLed, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loopblink() {
  digitalWrite(controlLed, HIGH);
  delay(1000);
  digitalWrite(controlLed, LOW);
  delay(1000);
}

// the loop routine runs over and over again forever:
void loop() {
  leds = 0;
  leds2 = 0;
  updateShiftRegister();
  
  if(mode == 0 || mode == 8) {
    if(mode == 0) {
    }
    
    setCurrentTime();
    updateLeds();
    
  } else {
    leds = 0;
    for (int i = 1; i < mode; i++) {
      Serial.println("Turning the first leds ON. " + String(i) + "; " + String(mode) + "; " + String(currentSecond));
      ledBitCheck = 0;
      bitSet(ledBitCheck, i);
      
      if((ledBitCheck & currentSecond) > 0){
        bitSet(leds, i);
      }
      updateShiftRegister();
    }
    if(blinkTime)
      bitSet(leds, mode - 1);
    
    updateShiftRegister();
    
  }
  delay(1000);               // wait for a second
  
  if (digitalRead(buttonPin) == LOW) {
    //button is pressed
    if(!buttonPressed){
      secondsPressed = 0;
      loopCurrent = 0;
      Serial.println("button is pressed first time\n");
      
      if(mode > 0) {
        currentBit = !currentBit;
        digitalWrite(controlLed, LOW);
        
        if(currentBit) {
          Serial.println("Ligando o led");
          digitalWrite(controlLed, HIGH);
          bitSet(currentSecond, mode - 1);
        } else {
          bitClear(currentSecond, mode - 1);
        }
      }
    }
      
    buttonPressed = true;
  } else {
    if(buttonPressed) {
      Serial.println("Not pressed anymore\n");
    }
    
    buttonPressed = false;
  }
  
  if(mode > 0) {
    loopCurrent++;
    if(loopCurrent == loopMax) {
      loopCurrent = 0;
      blinkTime = !blinkTime;
      
      if(buttonPressed) {
        secondsPressed++;
        Serial.println(" " + String(secondsPressed) + " seconds");

        if(secondsPressed == 3) {
          changeMode();
        }
      } else {
        Serial.println("Still alive");
      }
    }
  } else {
      if(buttonPressed) {
        secondsPressed++;
        Serial.println(" " + String(secondsPressed) + " seconds");

        if(secondsPressed == secondsToEnterConfig) {
          changeMode();
        }
      }
  }
  
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, leds2);
  shiftOut(dataPin, clockPin, MSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}

void changeMode()
{
  Serial.println("And this is the end for the mode " + String(mode));
  mode++;
  if(mode > 0) {
    currentBit = (bitRead(currentSecond, mode - 1) == 1);
  }
  
  if(mode == 9) 
    mode = 0;
}

void setCurrentTime() {
  currentSecond ++;
  checkTime();
}
void checkTime() {
  if(currentSecond == 60){
    currentSecond = 0;
    currentMinute ++;
  }
  if(currentMinute == 60){
    currentMinute = 0;
    currentHour ++;
  }
  if(currentHour == 13){
    currentHour = 0;
  }
}

void updateLeds()
{
  int secondsPins[] = {0, 1, 2, 3, 4, 5};
  int minutesPins[] = {0, 1, 2, 3, 4, 5};
  int hoursPins[] = {6, 7, 6, 7};
  for (int i = 0; i < 6; i++)
  {
    ledBitCheck = 0;
    bitSet(ledBitCheck, i);
    
    //First, the seconds. First 6 digits of the first register
    if((ledBitCheck & currentSecond) > 0){
      bitSet(leds, secondsPins[i]);
    }
    //Now the minutes. First 6 digits of the second register.
    if((ledBitCheck & currentMinute) > 0 ){
      bitSet(leds2, minutesPins[i]);
    }
    //And now the hour. Last 2 digits of each registers.
    if(((ledBitCheck & currentHour) > 0)){
      if(i < 2) {
        bitSet(leds, hoursPins[i]);
      } else { 
        bitSet(leds2, hoursPins[i]); 
      }
    }
  }
  
  updateShiftRegister();
}
