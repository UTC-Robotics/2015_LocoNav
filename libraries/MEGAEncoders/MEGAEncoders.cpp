#include "Arduino.h"
#include "MEGAEncoders.h"
#include <digitalWriteFast.h>

/******************************************************************************
*******************************************************************************
Encoder.cpp - Library for the using TWO US Digital E4P Single Ended Motor 
Encoders specifically for the Arduino MEGA

USERS REFERENCE: 
	Requirements: 
		- For use on Arduino MEGA on pins 18-20

Author: David McPherson (borrowing liberally from jrraines'  http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1267553811/0), 12 DEC 2013
*******************************************************************************
******************************************************************************/

// Left encoder
#define c_LeftEncoderInterruptA 2
#define c_LeftEncoderInterruptB 3
#define c_LeftEncoderPinA 21
#define c_LeftEncoderPinB 20
#define LeftEncoderIsReversed
// Right encoder
#define c_RightEncoderInterruptA 4
#define c_RightEncoderInterruptB 5
#define c_RightEncoderPinA 19
#define c_RightEncoderPinB 18
#define RightEncoderIsReversed

volatile bool _LeftEncoderASet;
volatile bool _LeftEncoderBSet;
volatile bool _LeftEncoderAPrev;
volatile bool _LeftEncoderBPrev;
volatile long _LeftEncoderTicks;

volatile bool _RightEncoderASet;
volatile bool _RightEncoderBSet;
volatile bool _RightEncoderAPrev;
volatile bool _RightEncoderBPrev;
volatile long _RightEncoderTicks;


/**
* Interrupt Service Routine to be called when an External Interrupt associated 
* with the Left or Right Encoders occur.
*
*/

// Interrupt service routines for the Right motor's quadrature encoder
void HandleRightMotorInterruptA(){
  _RightEncoderBSet = digitalReadFast(c_RightEncoderPinB);
  _RightEncoderASet = digitalReadFast(c_RightEncoderPinA);
  
  _RightEncoderTicks+=ParseEncoder();
  
  _RightEncoderAPrev = _RightEncoderASet;
  _RightEncoderBPrev = _RightEncoderBSet;
}

// Interrupt service routines for the right motor's quadrature encoder
void HandleRightMotorInterruptB(){
  // Test transition;
  _RightEncoderBSet = digitalReadFast(c_RightEncoderPinB);
  _RightEncoderASet = digitalReadFast(c_RightEncoderPinA);
  
  _RightEncoderTicks+=ParseEncoder();
  
  _RightEncoderAPrev = _RightEncoderASet;
  _RightEncoderBPrev = _RightEncoderBSet;
}

int ParseEncoder(){
  if(_RightEncoderAPrev && _RightEncoderBPrev){
    if(!_RightEncoderASet && _RightEncoderBSet) return 1;
    if(_RightEncoderASet && !_RightEncoderBSet) return -1;
  }else if(!_RightEncoderAPrev && _RightEncoderBPrev){
    if(!_RightEncoderASet && !_RightEncoderBSet) return 1;
    if(_RightEncoderASet && _RightEncoderBSet) return -1;
  }else if(!_RightEncoderAPrev && !_RightEncoderBPrev){
    if(_RightEncoderASet && !_RightEncoderBSet) return 1;
    if(!_RightEncoderASet && _RightEncoderBSet) return -1;
  }else if(_RightEncoderAPrev && !_RightEncoderBPrev){
    if(_RightEncoderASet && _RightEncoderBSet) return 1;
    if(!_RightEncoderASet && !_RightEncoderBSet) return -1;
  }
}


// Interrupt service routines for the left motor's quadrature encoder
void HandleLeftMotorInterruptA(){
  _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB);
  _LeftEncoderASet = digitalReadFast(c_LeftEncoderPinA);
  
  _LeftEncoderTicks+=LParseEncoder();
  
  _LeftEncoderAPrev = _LeftEncoderASet;
  _LeftEncoderBPrev = _LeftEncoderBSet;
}

// Interrupt service routines for the right motor's quadrature encoder
void HandleLeftMotorInterruptB(){
  // Test transition;
  _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB);
  _LeftEncoderASet = digitalReadFast(c_LeftEncoderPinA);
  
  _LeftEncoderTicks+=LParseEncoder();
  
  _LeftEncoderAPrev = _LeftEncoderASet;
  _LeftEncoderBPrev = _LeftEncoderBSet;
}

int LParseEncoder(){
  if(_LeftEncoderAPrev && _LeftEncoderBPrev){
    if(!_LeftEncoderASet && _LeftEncoderBSet) return 1;
    if(_LeftEncoderASet && !_LeftEncoderBSet) return -1;
  }else if(!_LeftEncoderAPrev && _LeftEncoderBPrev){
    if(!_LeftEncoderASet && !_LeftEncoderBSet) return 1;
    if(_LeftEncoderASet && _LeftEncoderBSet) return -1;
  }else if(!_LeftEncoderAPrev && !_LeftEncoderBPrev){
    if(_LeftEncoderASet && !_LeftEncoderBSet) return 1;
    if(!_LeftEncoderASet && _LeftEncoderBSet) return -1;
  }else if(_LeftEncoderAPrev && !_LeftEncoderBPrev){
    if(_LeftEncoderASet && _LeftEncoderBSet) return 1;
    if(!_LeftEncoderASet && !_LeftEncoderBSet) return -1;
  }
}


/**
* initializes encoder pins, counts- per- revolution variable, and attaches the 
* correct ISR to external interrupt pins
*
*/
void encoders_init()
{	
    _LeftEncoderTicks = 0;

    _RightEncoderTicks = 0;
	
	  // Left encoder
  pinMode(c_LeftEncoderPinA, INPUT);      // sets pin A as input
  digitalWrite(c_LeftEncoderPinA, LOW);  // turn on pullup resistors
  pinMode(c_LeftEncoderPinB, INPUT);      // sets pin B as input
  digitalWrite(c_LeftEncoderPinB, LOW);  // turn on pullup resistors
  attachInterrupt(c_LeftEncoderInterruptA, HandleLeftMotorInterruptA, CHANGE);
  attachInterrupt(c_LeftEncoderInterruptB, HandleLeftMotorInterruptB, CHANGE);
  // Right encRightoder
  pinMode(c_RightEncoderPinA, INPUT);      // sets pin A as input
  digitalWrite(c_RightEncoderPinA, LOW);  // turn on pullup resistors
  pinMode(c_RightEncoderPinB, INPUT);      // sets pin B as input
  digitalWrite(c_RightEncoderPinB, LOW);  // turn on pullup resistors
  attachInterrupt(c_RightEncoderInterruptA, HandleRightMotorInterruptA, CHANGE);
  attachInterrupt(c_RightEncoderInterruptB, HandleRightMotorInterruptB, CHANGE);
}

/**
* Returns value of "g_encL_tripCount". The value is copied to a temporary variable,
* while interrupts are disabled, the temporary variable is then returned. 
* Interrupts are disabled during the copy to ensure an interrupt does not 
* occur while reading "g_encL_count".
*
*@return	Left Encoder Trip Count variable value (32- bit signed integer)
*/
int getEncLCount()
{
	int temp;
	// Avoid interrupts while reading volatile global variables
	noInterrupts();        // Disable interrupts
	temp = _LeftEncoderTicks;
	interrupts();          // Restore interrupts
	return temp;
}

/**
* See getEncLCount() function above.
*
*@return	Right Encoder Trip Count variable value (32- bit signed integer)
*/
int getEncRCount()
{
	int temp;
	// Avoid interrupts while reading volatile global encoder count variable
	noInterrupts();        // Disable interrupts
	temp = _RightEncoderTicks;
	interrupts();          // Restore interrupts
	return temp;
}

/**
* Reset Left Encoder trip count to zero
*/
void resetEncLCount()
{
	_LeftEncoderTicks = 0;
}

/**
* Reset Right Encoder trip count to zero
*/
void resetEncRCount()
{
	_RightEncoderTicks = 0;
}
