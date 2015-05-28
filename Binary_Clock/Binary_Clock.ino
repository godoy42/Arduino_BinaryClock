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
boolean controlLedBlink = false;
boolean currentBit = false;
 
byte leds = 0;
byte leds2 = 0;
byte ledBitCheck = 0;
int currentSecond = 0;
int currentMinute = 1;
int currentHour = 1;
const int loopDelay = 100;
const int loopMax = 1000 / loopDelay;
int loopCurrent = 0;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(57600);
  Serial.println("Starting.");
  
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
  loopCurrent++;
  digitalWrite(controlLed, LOW);
  
  if(mode > 0) {
    controlLedBlink = !controlLedBlink;
    if(controlLedBlink)
      digitalWrite(controlLed, HIGH);
  }
  if(loopCurrent == loopMax) {
    loopCurrent = 0;
    blinkTime = !blinkTime;
    
    if(mode == 0)
      currentSecond ++;
    
    checkTime();
    
    if(buttonPressed){
      Serial.println("button is pressed for " + String(secondsPressed) + " seconds. ");
      secondsPressed++;
    
      if(secondsPressed == secondsToEnterConfig)
        changeMode();
    }
  }
  updateLeds();

  delay(loopDelay);
  
  if (digitalRead(buttonPin) == LOW) {
    //button is pressed
    if(!buttonPressed){
      secondsPressed = 0;
      loopCurrent = 0;
      Serial.println("button is pressed first time");
    }
      
    buttonPressed = true;
  } else {
    if(buttonPressed && (secondsPressed < secondsToEnterConfig)) 
      if(mode == 1)
        currentMinute++;
      else
        currentHour++;
    
    checkTime();
    
    if(buttonPressed)
      Serial.println("button was released.");
    
    buttonPressed = false;
  }
}

void changeMode()
{
  Serial.println("And this is the end for the mode " + String(mode));
  mode++;
  if(mode == 3) 
    mode = 0;
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
  if(currentHour > 12){
    currentHour = 1;
  }
}

void updateLeds()
{
  leds = 0;
  leds2 = 0;
  int secondsPins[] = {0, 1, 2, 3, 4, 5};
  int minutesPins[] = {0, 1, 2, 3, 4, 5};
  int hoursPins[] = {6, 7, 6, 7};
  for (int i = 0; i < 6; i++)
  {
    ledBitCheck = 0;
    bitSet(ledBitCheck, i);
    
    //First, the seconds. First 6 digits of the first register
    if((ledBitCheck & currentSecond) > 0) {
      bitSet(leds, secondsPins[i]);
    }
    
    //Now the minutes. First 6 digits of the second register.
    if((ledBitCheck & currentMinute) > 0 )
      if((mode == 0) || ((mode == 1) && blinkTime)) 
        bitSet(leds2, minutesPins[i]);
        
    //And now the hour. Last 2 digits of each registers.
    if(((ledBitCheck & currentHour) > 0)){
      if((mode == 0) || ((mode == 2) && blinkTime)){ 
        if(i < 2)
            bitSet(leds, hoursPins[i]);
        else
            bitSet(leds2, hoursPins[i]);
      }
    }
  }
  
  updateShiftRegister();
}

void updateShiftRegister()
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, leds2);
  shiftOut(dataPin, clockPin, MSBFIRST, leds);
  digitalWrite(latchPin, HIGH);
}


