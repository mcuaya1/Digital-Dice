//120B Final project
//Help from the following resources
//TA: RB
#include "Timer.h"

//States
enum DIGIT_STATE {FIRST_DIGIT, SECOND_DIGIT} dState = FIRST_DIGIT;
enum THROW_STATE {WAIT_THROW, CALCULATE_THROW_STR, SET_ROLL_VALUE} tState = WAIT_THROW;
enum SHIFT_SATE  {WAIT_SHIFT, SHIFT_NUMBERS} sState = WAIT_SHIFT;
int button_value = 0;

//Pins
int digitPins[2] = {13, 12};
int buzzerPin = 2;
int x_value = 0;
int y_value = 0;

//Variables
int first_digit = 0;
int second_digit = 0;
bool has_thrown = false;

unsigned long throw_elapsedTime = 500;
const unsigned long throwPeriod = 500;

unsigned long shift_elapsedTime = 50;
const unsigned long shiftPeriod = 50;

const unsigned long timerPeriod = 10;

//RB's Helper function
// gSegPins
// An array of pins of the arduino that are connected
// to segments a, b, c, d, e... g in that order.
//                 A, B, C, D, E, F, G
char gSegPins[7] = {3, 4, 5, 6, 7, 8, 9};


//RB'S helper function
// displayNumTo7Seg
// displays one number (between 0 and 9) "targetNum" on the digit conneceted to "digitPin"
// E.g. If I wanted to display the number 6 on the third digit of my display.
// and the third digit was connected to pin A0, then I'd write: displayNumTo7Seg(6, A0);
void displayNumTo7Seg(unsigned int targetNum, int digitPin) {


    // A map of integers to the respective values needed to display
    // a value on one 7 seg digit.
    unsigned char encodeInt[10] = {
        // 0     1     2     3     4     5     6     7     8     9
        0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
    };




    // Make sure the target digit is off while updating the segments iteratively
    digitalWrite(digitPin, LOW);


    // Update the segments
    for (int k = 0; k < 7; ++k) {
        digitalWrite(gSegPins[k], encodeInt[targetNum] & (1 << k));
    }


    // Turn on the digit again
    digitalWrite(digitPin, HIGH);


}

void digit_tick(){
  switch(dState){
    case FIRST_DIGIT:
    dState = SECOND_DIGIT;
    break;

   case SECOND_DIGIT:
   dState = FIRST_DIGIT;
   
   break;
  }

  switch(dState){
    case FIRST_DIGIT:
    digitalWrite(digitPins[0],HIGH);
    digitalWrite(digitPins[1],LOW);
    displayNumTo7Seg(first_digit, digitPins[0]);
    break;
    
    case SECOND_DIGIT:
    digitalWrite(digitPins[0],LOW);
    digitalWrite(digitPins[1],HIGH);
    displayNumTo7Seg(second_digit, digitPins[1]);
    break;
  }
  
}


void calculate_throw(){
    static int diceRoll = 0;
    switch(sState){
      case WAIT_SHIFT:
      if(y_value <= 599 && has_thrown == false){
        Serial.println("Waiting for dice to be picked up");
        sState = WAIT_SHIFT;
      }
      else if(y_value > 600 && has_thrown == false){
        Serial.println("Dice has been picked up");
        sState = SHIFT_NUMBERS;
      }
      break;
      
      case SHIFT_NUMBERS:
      if(y_value > 600 && has_thrown == false){
       Serial.println("Waiting for release");
       sState = SHIFT_NUMBERS;
      }
      else if(y_value <= 599 && has_thrown == false){
       Serial.println("Dice has been thrown");
       sState = WAIT_SHIFT;
      }
      break;
    }
    
    switch(sState){
      case SHIFT_NUMBERS:
      diceRoll = random(1,20);
      Serial.println(diceRoll);
      if(diceRoll < 10){
        first_digit = 0;
        second_digit = diceRoll % 10;  
      }
      else{
        first_digit = int(diceRoll/10);
        second_digit = diceRoll % 10;  
      }
      break;
    }
}

void rolling_dice(){
  static int current_throw_str = 0;
   switch(tState){
    case WAIT_THROW:
    if(y_value <= 599 && has_thrown == false){
      Serial.println("Waiting for dice to be picked up");
      tState = WAIT_THROW;
    }
    else if(y_value > 600 && has_thrown == false){
      Serial.println("Dice has been picked up");
      tState = CALCULATE_THROW_STR;
    }
    break;

   case CALCULATE_THROW_STR:
   if(y_value > 600 && has_thrown == false){
    Serial.println("Waiting for release");
    tState = CALCULATE_THROW_STR;
   }
   else if(y_value <= 599 && has_thrown == false){
     Serial.println("Dice has been thrown");
     has_thrown = true;
     tState = SET_ROLL_VALUE;
   }
   break;

   case SET_ROLL_VALUE:
   Serial.println("Setting dice value");
   tState = WAIT_THROW;
   break;
  }

  switch(tState){
    case CALCULATE_THROW_STR:
    current_throw_str += 5;
    break;
    
    case SET_ROLL_VALUE:
    has_thrown = false;
    break;
  }
}


void setup() {
  randomSeed(analogRead(A3));
  TimerSet(timerPeriod);
  TimerOn();
  pinMode(buzzerPin, OUTPUT);
  for(int i = 0; i < 7; i++){
    pinMode(gSegPins[i], OUTPUT);
  }

  for(int i = 0; i < 2; i++){
      pinMode(digitPins[i], OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  y_value = analogRead(A1);
  digit_tick();

  if(shift_elapsedTime >= shiftPeriod){
    calculate_throw();
    shift_elapsedTime = 0;
  }
  if(throw_elapsedTime >= throwPeriod){
    rolling_dice();
    throw_elapsedTime = 0;
  }
  throw_elapsedTime += timerPeriod;
  shift_elapsedTime += timerPeriod;
  while(!TimerFlag){}
  TimerFlag = 0;
}
