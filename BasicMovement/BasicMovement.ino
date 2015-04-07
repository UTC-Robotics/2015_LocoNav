#include <LineFollowerV2.h> // Line Follower Provider's Library
#include <DualVNH5019MotorShield.h> // Motor shield library
#include <Arduino.h> // Generic Arduino library

//int FrontLFPins[8]={36,38,40,42,44,46,48,50}; // Initialize Pins for the Front line follower
int MiddleLFPins[8]={37,39,41,43,45,47,49,51}; // Initialize Pins for the Middle line follower
LineFollower MiddleLF(MiddleLFPins, 52); // Declare line follower's handler object
DualVNH5019MotorShield shield; // Declare the motor shield's handler object

void setup() { // Setup all the inputs and outputs
  MiddleLF.enable(); // Enables the line follower we're using
  shield.init(); // Turn on the motor shield
  shield.setM1Speed(0); // Ensure Motor1 isn't moving when program starts ("Right" Motor)
  shield.setM2Speed(0); // Ensure Motor2 isn't moving when program starts ("Left" Motor)
  Serial.begin(9600);  // Setup the debugging Serial communication channel
//  Serial2.begin(115200); // In case we want a second debugging channel
}


void loop() { // Main Function = JOURNEY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int linepriority; // Declares a variable to check which line matters at a crossroad
linepriority==0; // 0==Left, 1==Right
bool* MLFPointer; // Declares a pointer to check the Middle line sensor
free(MLFPointer); // Frees the pointer from any value it may currently hold
MLFPointer = MiddleLF.measureBA(); // Sets the pointer to the boolean array recieved from the Middle line sensor


switch(MLFPointer[7]){ // Switch Statement for the Rightmost sensor
case 0: // No Left Line
   switch(MLFPointer[0]){ // Switch Statement for the Leftmost sensor
    case 0: // NoLines
      shield.setM1Speed(-100); //R
      shield.setM2Speed(-100); //L
    break;
    case 1: // Yes Right Line
      shield.setM1Speed(-100);
      shield.setM2Speed(100);
    break;
   }
break;
case 1: // Yes Left Line
      shield.setM1Speed(100); // Activate these lines to always turn right at intersections
      shield.setM2Speed(-100);
/*
   switch(MLFPointer[0]){
    case 0: //YesLeftLine + NoRightLine  ============
      shield.setBrakes(0,0);
      shield.setM1Speed(100); //R
      shield.setM2Speed(-100); //L
    break;
    case 1: //YesLeftLine + YesRightLine ============
    switch(linepriority){ // Checks which line gets priority at crossroads
      case 0: // Gives Priority to the Left Line
        shield.setBrakes(0,0);
        shield.setM1Speed(-100); //R
        shield.setM2Speed(100);  //L
      break;
      case 1: // Gives Priority to the Right Line
        shield.setBrakes(0,0);
        shield.setM1Speed(100);  //R
        shield.setM2Speed(-100); //L
      break;
      default:
        shield.setBrakes(0,0);
    }
//    free(MLFPointer);
//    MLFPointer = MiddleLF.measureBA();
    break;
    default:
    shield.setBrakes(0,0);
   }
   */
break;

}
    //shield.setBrakes(80,80);
//delay(10);
}



