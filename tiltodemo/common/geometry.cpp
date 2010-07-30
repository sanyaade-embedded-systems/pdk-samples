#include "geometry.h"
#include <math.h>

#define PI (3.14159)
#define TWOPI (6.28318)

// this function applies a velocity in PIXELS PER SECOND to a postion, with a time
// sent in in MILLISECONDS. It's IMPORTANT to note the discrepency in units. This
// functionality comes up often enough that it warranted a spot here. 
// The return value is the new location
double applyPPSVel(double startPos, double PPSVel, double milliseconds)
{
	double movement = PPSVel*milliseconds;
	movement = movement/(double)1000.0;
	return startPos + movement;
}

// same as the other applyPPSVel, except this one uses vectors and an outResult
void applyPPSVel(Vector &startPos, Vector &ppsVel, double milliseconds, Vector &outResult)
{
	outResult.m_x = applyPPSVel(startPos.m_x, ppsVel.m_x, milliseconds);
	outResult.m_y = applyPPSVel(startPos.m_y, ppsVel.m_y, milliseconds);
	return;
}

// fairly straightforward point in rect functionality
SDL_bool ptInRect(double px, double py, double rectX, double rectY, double rectWidth, double rectHeight)
{
	if ( px < rectX ) return SDL_FALSE;
	if ( px >= rectX+rectWidth ) return SDL_FALSE;
	if ( py < rectY ) return SDL_FALSE;
	if ( py >= rectY+rectHeight) return SDL_FALSE;
	return SDL_TRUE;
}

SDL_bool ptInRect(int x, int y, SDL_Rect *rc)
{
	return ptInRect(x, y, rc->x, rc->y, rc->w, rc->h);
}

/************************** VECTOR FUNCTIONS *****************************/
/************************** VECTOR FUNCTIONS *****************************/
/************************** VECTOR FUNCTIONS *****************************/

// set with cartesian coordinates	
void Vector::set(Vector &in)
{
	m_x = in.m_x;
	m_y = in.m_y;
}

void Vector::setXY(double x, double y)
{
	m_x = x;
	m_y = y;
}

SDL_bool Vector::equals(Vector &in)
{
	if ( m_x == in.m_x && m_y == in.m_y )
	{
		return SDL_TRUE;
	}
	return SDL_FALSE;
}

// set with polar coordinates. note -- Theta is in radians
void Vector::setRTheta(double r, double theta)
{
	// remember trig class, boys and girls?
	// x = r*cos(theta), y=r*sin(theta)
	double cosTheta = cos(theta);
	double sinTheta = sin(theta);
	
	m_x = r*cosTheta;
	m_y = r*sinTheta;
}

// get the length (r) of the vector
double Vector::getLengthSq()
{
	double xSquared = m_x*m_x;
	double ySquared = m_y*m_y;
	double hypotenuseSquared = xSquared + ySquared;
	return hypotenuseSquared;
}

double Vector::getLength()
{
	double hypotenuseSquared = getLengthSq();
	return sqrt(hypotenuseSquared);
}

double Vector::getAngle()
{
	if ( (m_x==0) && (m_y==0) )
	{
		return 0;
	}

	return atan2(m_y, m_x);
}

void Vector::addVector(Vector &in)
{
	m_x += in.m_x;
	m_y += in.m_y;
}

void Vector::subtractVector(Vector &in)
{
	m_x -= in.m_x;
	m_y -= in.m_y;
}

// multiplies the length of the vector by the scalar
void Vector::scalarMultiply(double scalar)
{
	m_x *= scalar;
	m_y *= scalar;
}

// divides the length of the vector by the scalar
void Vector::scalarDivide(double scalar)
{
	m_x /= scalar;
	m_y /= scalar;
}

// rotates the point around the origin be the given number of fourkays
void Vector::rotate(double angle)
{
	// Well documented formula: how to rotate a point around the origin:
	// newX = x*cos(theta) - y*sin(theta)
	// newY = x*sin(theta) + y*cos(theta)
	double newX = m_x*cos(angle) - m_y*sin(angle);
	double newY = m_x*sin(angle) + m_y*cos(angle);
	
	m_x = newX;
	m_y = newY;
}

// makes the length of the vector 1.0 without changing the angle
void Vector::normalize()
{
	double length = getLength();
	scalarDivide(length);		
}

// changes the length of the vector to the desired length
// this is safer than calling normalize then multiplying, cause
// it'll have less approximation errors in the angle
void Vector::setLength(double newLength)
{
	double origLength = getLength();
	if ( origLength == 0.0 )
	{
		setXY(0,0);
		return;
	}
	scalarMultiply(newLength);
	scalarDivide(origLength);
}

