#ifndef __COLLISION__
#define __COLLISION__

#include "SDL.h"
#include "geometry.h"

// for collision detection, we will have a series of line segments. 
// Because this is just a sample app whose main function is showing
// you how to do things, and not a high-end game made to impress and astound,
// we'll just have purely vertical and horizontal collision walls because it makes
// things a lot easier. 
class CollideWall
{
public:
	CollideWall();
	~CollideWall();

	// for these initters, x and y are the start point. Then
	// length is either the vertical or horizontal size (depending on if
	// it is a vertical or horizontal segment). push should be either +1 or -1, 
	// and it represents the direction that the segment "pushes" things. So
	// a push of +1 on a vertical segment means the segment keeps things to the
	// right of it. It will "push" incoming objects to the right. 
	// this value is needed because we actually adjust the ball's position once
	// we know what collider it struck. We push it off the collider in this direction.
	// if the ball is resting exactly on the collider, it causes collisioning to 
	// misfire. 
	void initVertical(int x, int y, int height, int push);
	void initHorizontal(int x, int y, int length, int push);

	SDL_bool checkCollision(Vector &start, Vector &end, Vector &result);
	SDL_bool isMiddle(double a, double middle, double b); // returns trus if a<=middle<=b or b<=middle<=a
	void push(Vector &pos); // apply the push value to the postion

	// itnernals
	void setPush(int push); 

	// data. stored as doubles to minimize needless casting
	SDL_bool m_bIsVertical;
	double m_x;
	double m_y;
	double m_size; // width or length, depending on the orientation
	double m_push;
};




#endif

