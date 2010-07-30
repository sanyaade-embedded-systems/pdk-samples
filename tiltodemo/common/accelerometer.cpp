#include "accelerometer.h"

// we keep these values, and poll for them once per game loop. 
// We do it this way because our implementation wants nubmers in the range 
// of -1.0 to 1.0, so some math is involved. Rather than do that over and over
// again, we just do it once per tick (when the main app calls updateAccelerometer()). 
double s_accX;
double s_accY;
double s_accZ;

SDL_Joystick *gJoystick;

void accelerometer_init()
{
	s_accX = 0.0;
	s_accY = 0.0;
	s_accZ = 0.0;

	// open the joystick for use. The joystick
	// in this case is the accelerometer. If there
	// is not joystick 0, something bad has happened.
	if( SDL_NumJoysticks() > 0 )
	{
		// ntoe the joystick
		gJoystick = SDL_JoystickOpen(0);
	}
	else
	{
		// no joystick. Bad. But not technically fatal.
		// and pretty self-evident when running the app (the ball won't move).
		// So we'll alert with some debug text, but not panic-quit.
		printf("Could not find joystick!\n");
	}
}

double getAccelerometerX()
{
	return s_accX;
}

double getAccelerometerY()
{
	return s_accY;
}

double getAccelerometerZ()
{
	return s_accZ;
}

// This function is set up to take the interger valued from the accelerometer
// events that came in through the main event loop. We will turn these events into 
// values from -1.0 to 1.0. The first value is ACCELEROMETER_X, ACCELEROMETER_Y,
// or ACCELEROMETER_Z
void updateAccelerometer(int accelerometer, int value)
{
	// conform it to the scale of -1.0 to 1.0 for 1g in either direction
	double dVal = (double)value / 32768.0;

	// now set the appropriate internal value for the 
	// accelerometer that changed.
	switch ( accelerometer )
	{
		case ACCELEROMETER_X: s_accX = dVal; break;
		case ACCELEROMETER_Y: s_accY = dVal; break;
		case ACCELEROMETER_Z: s_accZ = dVal; break;
	}
}


