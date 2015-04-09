#include "Arduino.h"
#include "LineFollowerV2.h"

/*
	Library for the Parallax Inc. Line Follower
*/

LineFollower::LineFollower(int outPin[8])
{
	// Initialize Pins, but into Low power mode
	for(int index = 0; index < 8; index++)
	{
		_outPin[index] = outPin[index];
		pinMode(_outPin[index], INPUT);
	}
	//_enablePin = enablePin;
	//pinMode(_enablePin, OUTPUT);
	//digitalWrite(_enablePin, LOW);
}

char LineFollower::measure()
{
	char temp = 0;
	for(int index = 0; index < 8; index++)
	{
		temp = (temp << 1) + !digitalRead(_outPin[index]);	// Flip because Hardware implements negative logic
	}
	return temp;	
}

bool* LineFollower::measureBA()
{
    bool* TEMP = (bool*)calloc(8,sizeof(bool));
	for(int index = 0; index < 8; index++)
	{
      TEMP[index]= !digitalRead(_outPin[index]);
    }
    return(TEMP);
}
