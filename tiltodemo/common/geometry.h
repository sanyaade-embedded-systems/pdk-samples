#ifndef __GEOMETRY__
#define __GEOMETRY__

#include "SDL.h"


class Vector
{
public:
	Vector(){}
	void set(Vector &in);
	void setXY(double x, double y);
	SDL_bool equals(Vector &in);

	// set with polar coordinates. note -- Theta is in radians
	void setRTheta(double r, double theta);

	// get the length (r) of the vector
	double getLength();
	double getLengthSq(); // for optimized distance checks

	// returns the radiuns of the vector. If the vector's length is
	// 0, it returns 0
	double getAngle();
	void addVector(Vector &in);
	void subtractVector(Vector &in);
	
	// adjusts the length of the vector by the scalar
	void scalarMultiply(double scalar);
	void scalarDivide(double scalar);
	
	// rotates the point around the origin be the given number of radiuns
	void rotate(double theta);
	
	// makes the length of the vector 1.0 without changing the angle
	void normalize();
	
	// changes the length of the vector to the desired length
	// this is safer than calling normalize then multiplying, cause
	// it'll have less approximation errors in the angle
	void setLength(double fixNewLength);
	
	/******************************** CLASS DATA ******************************/
	double m_x;
	double m_y;
};

double applyPPSVel(double StartPos, double PPSVel, double milliseconds);
void applyPPSVel(Vector &startPos, Vector &ppsVel, double milliseconds, Vector &outResult);
SDL_bool ptInRect(int x, int y, SDL_Rect *rc);

/*
// geometric management functions.
SDL_bool ptInRect(double px, double py, double rectX, double rectY, double rectWidth, double rectHeight);
SDL_bool ptInTriangle(Vector &t1, Vector &t2, Vector &t3, Vector &p);
SDL_bool sameSide(Vector &p1, Vector &p2, Vector &p3, Vector &p4);
SDL_bool segmentsIntersect(Vector &p1, Vector &p2, Vector &p3, Vector &p4, Vector &outResult);
SDL_bool getIntersectionPoint(Vector &p1, Vector &p2, Vector &p3, Vector &p4, Vector &outResult);
SDL_bool segmentIntersectsRect(Vector &segmentStart, Vector &segmentEnd, Vector &topLeft, Vector &topRight, Vector &bottomLeft, Vector &bottomRight, Vector &outResult);
SDL_bool forceInBounds(Vector &bounds1, Vector &bounds2, Vector &mover1, Vector &mover2, Vector &inBounds, Vector &outResult);
SDL_bool forceInBoundsSlide(Vector &bounds1, Vector &bounds2, Vector &mover1, Vector &mover2, Vector &inBounds, Vector &outResult);
SDL_bool forceInRectSlide(Vector &topLeft, Vector &bottomRight, Vector &mover1, Vector &mover2, Vector &outResult);
void projectToLine(Vector &p1, Vector &p2, Vector &point, Vector &outResult);
double getUNumeratorA(Vector &p1, Vector &p2, Vector &p3, Vector &p4);
double getUNumeratorB(Vector &p1, Vector &p2, Vector &p3, Vector &p4);
double getUDenominator(Vector &p1, Vector &p2, Vector &p3, Vector &p4);
double getDistance(Vector &a, Vector &b);
double getDistanceSq(Vector &a, Vector &b);
double applyPPSVelToward(double startPos, double PPSVel, double endPos, double milliseconds);
void applyPPSVelToward(Vector &startPos, double PPSVel, Vector &endPos, double milliseconds, Vector &outResult);
SDL_bool nearestPoint(Vector &target, Vector &candidate1, Vector &candidate2, Vector &outResult);
double angleDiff(double angle1, double angle2);
double applyRPSAngle(double startAngle, double velRPS, double ms);
double applyRPSAngleToward(double startAngle, double endAngle, double velRPS, double ms);
int getFacingForVector(Vector &face, int numQuantas);
*/

#endif

