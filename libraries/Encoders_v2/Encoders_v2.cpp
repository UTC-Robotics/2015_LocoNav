#include "Arduino.h"
#include "Encoders_v2.h"

/******************************************************************************
*******************************************************************************
Encoder.cpp - Library for the using TWO US Digital E4P Single Ended Motor 
Encoders specifically for the Arduino Due

USERS REFERENCE: 
	Requirements: 
		- For use on Arduino Due

Author: Bryce Williams, 12 DEC 2013
*******************************************************************************
******************************************************************************/

// Global Variables
int g_encL_count;				// Left Encoder Master Count
int g_encL_tripCount;			// Left Encoder Trip Count (intended to be reset by user)
int g_encR_count;				// ... same for the Right Encoder
int g_encR_tripCount;

// Global Pin Definitions
int g_encL_chA_pin;			// Arduino Due Pin number for Left Encoder Channel A
int g_encL_chB_pin;			// Arduino Due Pin number for Left Encoder Channel B	
int g_encR_chA_pin;			// Arduino Due Pin number for Right Encoder Channel A
int g_encR_chB_pin;			// Arduino Due Pin number for Right Encoder Channel B
// Global Encoder Variables
int g_cpr;					  // Encoder Counts- Per- Revolution

/**
* Interrupt Service Routine to be called when an External Interrupt associated 
* with the Left Encoder occurs.
*
*/
void encoderL_ISR()
{
	if(digitalRead(g_encL_chA_pin) == digitalRead(g_encL_chB_pin))
	{
		g_encL_count++;
		g_encL_tripCount++;
	}
	else
	{
		g_encL_count--;
		g_encL_tripCount--;
	}
}

/**
* Interrupt Service Routine to be called when an External Interrupt associated 
* with the Right Encoder occurs.
*
*/
void encoderR_ISR()
{
	if(digitalRead(g_encR_chA_pin) == digitalRead(g_encR_chB_pin))
	{
		g_encR_count--;
		g_encR_tripCount--;
	}
	else
	{
		g_encR_count++;
		g_encR_tripCount++;
	}
}

/**
* initializes encoder pins, counts- per- revolution variable, and attaches the 
* correct ISR to external interrupt pins
*
*/
void encoders_init(int _g_cpr, int encL_ChA_pin, int encL_ChB_pin, 
							   int encR_ChA_pin, int encR_ChB_pin)
{
	// Initialize counter variables
	g_encL_count     = 0;
	g_encL_tripCount = 0;
	g_encR_count     = 0;
	g_encR_tripCount = 0;
	
	// Initialize variables
	g_cpr = _g_cpr;
	g_encL_chA_pin = encL_ChA_pin;
	g_encL_chB_pin = encL_ChB_pin;
	g_encR_chA_pin = encR_ChA_pin;
	g_encR_chB_pin = encR_ChB_pin;
	
	// Initialize pins
	pinMode(g_encL_chA_pin, INPUT);
	pinMode(g_encR_chA_pin, INPUT);
	pinMode(g_encL_chB_pin, INPUT);
	pinMode(g_encR_chB_pin, INPUT);
	digitalWrite(g_encL_chA_pin, HIGH); // Enables internal 20k pullup resistor
	digitalWrite(g_encR_chA_pin, HIGH); // Enables internal 20k pullup resistor
	digitalWrite(g_encL_chB_pin, HIGH); // Enables internal 20k pullup resistor
	digitalWrite(g_encR_chB_pin, HIGH); // Enables internal 20k pullup resistor

	// Attach interrupts
	attachInterrupt(g_encL_chA_pin, encoderL_ISR, RISING);
	attachInterrupt(g_encR_chA_pin, encoderR_ISR, RISING);
}

/**
* Returns value of "g_encL_count". The value is copied to a temporary variable,
* while interrupts are disabled, the temporary variable is then returned. 
* Interrupts are disabled during the copy to ensure an interrupt does not 
* occur while reading "g_encL_count".
*
*@return	Left Encoder Count variable value (32- bit signed integer)
*/
int getEncLmasterCount()
{
	int temp;
	// Avoid interrupts while reading volatile global variables
	noInterrupts();        // Disable interrupts
	temp = g_encL_count;
	interrupts();          // Restore interrupts
	return temp;
}

/**
* See getEncLCount() function above.
*
*@return	Right Encoder Count variable value (32- bit signed integer)
*/
int getEncRmasterCount()
{
	int temp;
	// Avoid interrupts while reading volatile global encoder count variable
	noInterrupts();        // Disable interrupts
	temp = g_encR_count;
	interrupts();          // Restore interrupts
	return temp;
}

/**
* Returns value of "g_encL_tripCount". The value is copied to a temporary variable,
* while interrupts are disabled, the temporary variable is then returned. 
* Interrupts are disabled during the copy to ensure an interrupt does not 
* occur while reading "g_encL_count".
*
*@return	Left Encoder Trip Count variable value (32- bit signed integer)
*/
int getEncLtripCount()
{
	int temp;
	// Avoid interrupts while reading volatile global variables
	noInterrupts();        // Disable interrupts
	temp = g_encL_tripCount;
	interrupts();          // Restore interrupts
	return temp;
}

/**
* See getEncLCount() function above.
*
*@return	Right Encoder Trip Count variable value (32- bit signed integer)
*/
int getEncRtripCount()
{
	int temp;
	// Avoid interrupts while reading volatile global encoder count variable
	noInterrupts();        // Disable interrupts
	temp = g_encR_tripCount;
	interrupts();          // Restore interrupts
	return temp;
}

/**
* Reset Left Encoder trip count to zero
*/
void resetEncLtripCount()
{
	g_encL_tripCount = 0;
}

/**
* Reset Right Encoder trip count to zero
*/
void resetEncRtripCount()
{
	g_encR_tripCount = 0;
}

/*
* Returns the specified Counts- Per- Revolution of the encoder
* specified by user in the encoders_init() function
*
*@return	Encoder Counts- Per- Revolution
*/
int getEncCPR()
{
	return g_cpr;
}

/* 
* Returns the pin number that the Left Encoder Channel A is connected to
*
*@return	The pin number Left Encoder Channel A is connected to
*/
int getEncL_ChApin()
{
	return g_encL_chA_pin;
}

/* 
* Returns the pin number that the Right Encoder Channel A is connected to
*
*@return	The pin number Right Encoder Channel A is connected to
*/
int getEncR_ChApin()
{
	return g_encR_chA_pin;
}

/* 
* Returns the pin number that the Left Encoder Channel B is connected to
*
*@return	The pin number Left Encoder Channel B is connected to
*/
int getEncL_ChBpin()
{
	return g_encL_chB_pin;
}

/* 
* Returns the pin number that the Right Encoder Channel B is connected to
*
*@return	The pin number Right Encoder Channel B is connected to
*/
int getEncR_ChBpin()
{
	return g_encR_chB_pin;
}
