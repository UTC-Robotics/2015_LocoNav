#ifndef Encoders_v2_h
#define Encoders_v2_h

/******************************************************************************
*******************************************************************************
Encoder.h - Library for the using TWO US Digital E4P Single Ended Motor 
Encoders specifically for the Arduino Due

USERS REFERENCE: 
	Requirements: 
		- For use on Arduino Due
		
Created by Bryce Williams, 12 DEC 2013		
*******************************************************************************
******************************************************************************/

// Function Prototypes
void encoders_init(int _g_cpr, int encL_ChA_pin, int encL_ChB_pin, 
							   int encR_ChA_pin, int encR_ChB_pin);
int getEncLmasterCount();	// Returns total count on left encoder
int getEncRmasterCount();	// Returns total count on right encoder
int getEncLtripCount();		// Returns trip count on left encoder	
int getEncRtripCount();		// Returns trip count on right encoder
void resetEncLtripCount();	// Set Left Encoder Trip Count to zero
void resetEncRtripCount();	// Set Right Encoder Trip Count to zero
int getEncCPR();			// Returns Counts Per Revolution of encoders
int getEncL_ChApin();		// Returns pin number of Left Encoder ChA pin 
int getEncR_ChApin();		// Returns pin number of Right Encoder ChA pin
int getEncL_ChBpin();		// Returns pin number of Left Encoder ChB pin
int getEncR_ChBpin();		// Returns pin number of Right Encoder ChB pin

#endif;