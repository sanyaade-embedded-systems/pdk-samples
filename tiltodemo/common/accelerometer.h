#ifndef __ACCELEROMETER__
#define __ACCELEROMETER__

#include "SDL.h"

#define ACCELEROMETER_X 0
#define ACCELEROMETER_Y 1
#define ACCELEROMETER_Z 2

// the function declarations for our accelerometer management
void accelerometer_init();
double getAccelerometerX();
double getAccelerometerY();
double getAccelerometerZ();
void updateAccelerometer(int accelerometer, int value);

#endif

