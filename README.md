# Digital Dice
## CS 120B Final Project

### What is it?
<p>A device that simulates the rolling of a physical dice using a joystick to simulate holding and throwing of dice. Upon 'throwing' the dice, the 7-segment-4-digit LED display will shift through values randomly for a total of 5 seconds while playing a suspenseful medoloy via the connected passive buzzer. After 5 seconds, the number rolled will be displayed on the LED display, and depending on what number was rolled, the passive buzzer will play a meldoy. If the roll was less than 10, then the passive buzzer will play the 'losing' meldoy, and if the roll was greater than or equal to 10, the passive buzzer will play the 'victory' meldoy.</p>

### How does it work?
<p>The system works by using conccurrent state machine logic with some states using different periods to achieve program functionality. There are 3 main state machines: DIGIT_STATE, ROLLING_STATE, and CALCULATE_ROLL_STATE.
The ROLLING_STATE machine checks for changes in the joystick's y axis, and if the joystick's y axis exceeds 600 (indicating the joystick is being pushed up), the state machine will begin transitioning into another state, which calculates the strength of the dice roll. Once the dice have been thrown, the CALCULATE_ROLL_STATE machine will shift through random values for 5 seconds while writing to global variables that store the value that will be displayed on the LED display. 
  
After 5 seconds, the CALCULATE_ROLL_STATE machine will then transition into another state that calculates the value of the dice roll, adding the strength modifier to the value of the dice roll as well. The state then writes this value to the global variables that the DIGIT_STATE machine uses to output onto the LED display by shifting between two states that handle displaying the appropriate digit at the appropriate time. The last three states machines in the program handle song output and simply use boolen flags to indicate when a song should be played or not.</p>

### Video demonstration
[![Video demonstration](https://img.youtube.com/vi/_TLMPbpP9Zc/0.jpg)](https://www.youtube.com/watch?v=_TLMPbpP9Zc&feature=youtu.be)

### Circuit diagram
Coming soon...
