//120B Final project
//Help from the following resources
//TA: RB
//Videos: https://www.youtube.com/watch?v=gj-H_agfd6U
//Other: Zybboks CHP 7 % 6
#include "Timer.h"
/*
  TODO:
    Change state and function names to match their purpose
    Add Buzzer functionality
*/
//States
enum DIGIT_STATE {FIRST_DIGIT, SECOND_DIGIT} dState = FIRST_DIGIT;
enum ROLLING_STATE {WAIT_THROW, CALCULATE_THROW_STR} rState = WAIT_THROW;
enum CALCULATE_ROLL_STATE  {WAIT_ROLL, ROLLING_DICE, CALCULATE_ROLL_VALUE, ADD_MODIFIER} sState = WAIT_ROLL;
int button_value = 0;

//Pins
char digitPins[2] = {13, 12};
int buzzerPin = 2;
unsigned int yValue = 0;

//Variables
char first_digit = 0;
char second_digit = 0;
char currentThrowStr = 0;

unsigned long rollElapsedTimeriod = 500;
const unsigned long rollPeriod = 500;

unsigned long calculateElapsedTime = 50;
const unsigned long calculatePeriod = 50;

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

//State to handle digit LED
void digitTick(){
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

//State to caculate roll value
void calculate_roll(){
    static int diceRoll = 0;
    static int cnt = 0;
    switch(sState){
      case WAIT_ROLL:
      if(yValue <= 599){
        Serial.println("Waiting for dice to be picked up");
        sState = WAIT_ROLL;
      }
      else if(yValue > 600){
        Serial.println("Dice has been picked up");
        sState = ROLLING_DICE;
      }
      break;
      
      case ROLLING_DICE:
      if(yValue > 600){
       Serial.println("Waiting for release");
       sState = ROLLING_DICE;
      }
      else if(yValue <= 599){
       Serial.println("Dice has been thrown");
       sState = CALCULATE_ROLL_VALUE;
      }
      break;

      case CALCULATE_ROLL_VALUE:
      if(cnt < 40){
          Serial.println("Slowing roll");
          cnt += 1;
          sState = CALCULATE_ROLL_VALUE;
        }
      else{
          cnt = 0;
          sState = ADD_MODIFIER;
        }
      break;

      case ADD_MODIFIER:
      sState = WAIT_ROLL;
      break;
    }
    
    switch(sState){
      case ROLLING_DICE:
      diceRoll = random(1,20);
      if(diceRoll < 10){
        first_digit = 0;
        second_digit = diceRoll % 10;  
      }
      else{
        first_digit = int(diceRoll/10);
        second_digit = diceRoll % 10;  
      }
      break;

     case CALCULATE_ROLL_VALUE:
      if(cnt % 5 == 0){
        diceRoll = random(1,20);
      }
      if(diceRoll < 10){
        first_digit = 0;
        second_digit = diceRoll % 10;  
      }
      else{
        first_digit = int(diceRoll/10);
        second_digit = diceRoll % 10;  
      }
     break;

     case ADD_MODIFIER:
     if(diceRoll + currentThrowStr >= 20){
      diceRoll = 20;
     }
     else {
        diceRoll += currentThrowStr;
        if(diceRoll < 10){
          first_digit = 0;
          second_digit = diceRoll % 10;  
        }
        else{
          first_digit = int(diceRoll/10);
          second_digit = diceRoll % 10;  
        }
     }
     break;
    }
}

//State to calculate strength of roll
void roll_dice(){
   switch(rState){
    case WAIT_THROW:
    if(yValue <= 599){
      Serial.println("Waiting for dice to be picked up");
      rState = WAIT_THROW;
    }
    else if(yValue > 600){
      Serial.println("Dice has been picked up");
      rState = CALCULATE_THROW_STR;
    }
    break;

   case CALCULATE_THROW_STR:
   if(yValue > 600){
    Serial.println("Waiting for release");
    rState = CALCULATE_THROW_STR;
   }
   else if(yValue <= 599){
     Serial.println("Dice has been thrown");
     //has_thrown = true;
     rState = WAIT_THROW;
   }
   break;
  }

  switch(rState){
    case CALCULATE_THROW_STR:
    if(currentThrowStr < 5){
      currentThrowStr += 1;
    }
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
  yValue = analogRead(A1);
  digitTick();

  if(calculateElapsedTime >= calculatePeriod){
    calculate_roll();
    calculateElapsedTime = 0;
  }
  if(rollElapsedTimeriod >= rollPeriod){
    roll_dice();
    rollElapsedTimeriod = 0;
  }
  rollElapsedTimeriod += timerPeriod;
  calculateElapsedTime += timerPeriod;
  while(!TimerFlag){}
  TimerFlag = 0;
}
