#ifndef MEGAEncoders_h
#define MEGAEncoders_h

/******************************************************************************
*******************************************************************************
Encoder.h - Library for the using TWO US Digital E4P Single Ended Motor 
Encoders specifically for the Arduino MEGA

USERS REFERENCE: 
	Requirements: 
		- For use on Arduino MEGA
		
Thrown by David McPherson, 31 MAR 2015		
*******************************************************************************
******************************************************************************/

// Function Prototypes
void encoders_init();
int getEncRCount();
int getEncLCount();
void resetEncRCount();
void resetEncLCount();
int ParseEncoder();
int LParseEncoder();

#endif;
