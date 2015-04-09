#ifndef LineFollowerV2_H
#define LineFollowerV2_H
#include "Arduino.h"

/*
	Library for the Parallax Inc. Line Follower
*/

class LineFollower
{
	public:
		// Constructors
		LineFollower(int outPin[8]);	// outPin[] array index number corresponds to actual pinout
														// i.e. outPin[0] is Out0 on Line Follower, outPin[1] is Out1, etc.
		// Public Methods
		char measure();			// Get current senor pins status
		bool* measureBA();      // Get the current sensor pins' status expressed as an array of booleans
		
		
	
	private:
		int _outPin[8];		// Sensor Pins
		char output;		// 8 bit physical representation of sensor pins input.
							// i.e. bit 0 corresponds to Out0 on LineFollower, etc.
							// 1 = Pin Triggered, 0 = Pin Not Triggered
};

#endif;
