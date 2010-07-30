#include "SDL.h"
#include "collision.h"

// In general, this file is not as heavily commented as other files
// in this sample app. The purpose of the sample app is to demonstrate
// the proper use of SDL and SDL concepts, not to teach you how to do
// collision detection. 

CollideWall::CollideWall()
{
}

CollideWall::~CollideWall()
{
}

void CollideWall::initVertical(int x, int y, int height, int push)
{
	m_bIsVertical = SDL_TRUE;
	m_x = x;
	m_y = y;
	m_size = height;
	setPush(push);
}

void CollideWall::initHorizontal(int x, int y, int length, int push)
{
	m_bIsVertical = SDL_FALSE;
	m_x = x;
	m_y = y;
	m_size = length;
	setPush(push);
}

void CollideWall::setPush(int push)
{
	// we push just a quarter pixel. That way
	// the pushed item will display in the same location,
	// yet will still not be sitting right on the geometric
	// line of collision.
	if ( push > 0 )
	{
		m_push = 0.25;
	}
	else if ( push < 0 )
	{
		m_push = -0.25;
	}
	else
	{
		// really shouldn't do this. There should always
		// be a push. But ok, if they send in 0, they get
		// no push
		m_push = 0.0;
	}
}

SDL_bool CollideWall::checkCollision(Vector &start, Vector &end, Vector &result)
{
	// return true if there was a collision. result will have the 
	// point of collision put in to it. In the event that there
	// was no collision, result will be set to the value of end

	// any early return means there was no collision. So we'll set
	// this to end now, then change our minds later if need be
	result.set(end);

	// first off, before doing slope-intercept math, let's
	// do a simple test to see if it's even possible they intersected us
	// the first thing to check is that the points are on opposite
	// sides of us. For verticals, that means the x's of the start and
	// end need to be on opposite sides of our x. For horizontals, that
	// means the y's have to be on opposite sides of our y
	if ( m_bIsVertical )
	{
		// vertical. Are the x's on different sides? If not, there can't be a collision
		if ( !isMiddle(start.m_x, m_x, end.m_x) ) return SDL_FALSE;
	}
	else
	{
		// horizontal. Are the y's on different sides? If not, there can't be a collision
		if ( !isMiddle(start.m_y, m_y, end.m_y) ) return SDL_FALSE;
	}

	// if we're here, it means the movement vector will cross over the
	// line that this collision segment is on. We can do one more layer of
	// filtering before moving on to our costly line intersection function.
	// if the other dimension of movement does not overlap our own length, they
	// can't possible intersect. So if y values are both above our vertical, or 
	// both below it, they can't intersect. OR if both x's are to the left of our 
	// horizontal, or both to the right of it, they can't intersect.
	if ( m_bIsVertical )
	{
		// are both y values above us?
		if ( (start.m_y<m_y) && (end.m_y<m_y) ) return SDL_FALSE;

		// below us?
		double bottomY = start.m_y + m_size;
		if ( (start.m_y>bottomY) && (end.m_y>bottomY) ) return SDL_FALSE;
	}
	else
	{
		// are both x values left of us?
		if ( (start.m_x<m_x) && (end.m_x<m_x) ) return SDL_FALSE;

		// to the right of us?
		double rightX = start.m_x + m_size;
		if ( (start.m_x>rightX) && (end.m_x>rightX) ) return SDL_FALSE;
	}

	// if we're here, it means the movement line segment crosses over the line we 
	// are on, AND their total displacement parallel to us overlaps us. We don't
	// know for a fact that they intersect, but we haven't ruled it out the 
	// easy way yet. Time for the calculations to find line segment intersections.
	// we do this by simply ratioing the dx and dy of the movement vector to the
	// distance from the start to our collision segment. It's pretty
	// simple math, but we don't want to do it for every single collision segment in 
	// the world every game loop, hence the above filtering.

	double dx = end.m_x - start.m_x;
	double dy = end.m_y - start.m_y;
	if ( m_bIsVertical )
	{
		// we're a vertical collision segment. 
		double x = m_x - start.m_x; // distance from the start point to us.
		double y = start.m_y + (x*dy)/dx; // this will be the y value of the movement line where it intersects our line.

		// is that y value within our start and end y values?
		if ( !isMiddle(m_y, y, m_y+m_size) ) return SDL_FALSE;

		// if we're here, there was a collision. Set up the result vector
		result.m_x = m_x;
		result.m_y = y;
		return SDL_TRUE;
	}
	else
	{
		// we're a horizontal collision segment. 
		double y = m_y - start.m_y; // distance from the start point to us.
		double x = start.m_x + (y*dx)/dy; // this will be the x value of the movement line where it intersects our line.

		// is that x value within our start and end y values?
		if ( !isMiddle(m_x, x, m_x+m_size) ) return SDL_FALSE;

		// if we're here, there was a collision. Set up the result vector
		result.m_x = x;
		result.m_y = m_y;
		return SDL_TRUE;
	}
}

void CollideWall::push(Vector &pos)
{
	// apply the push to the appropriate 
	// component of the vector.
	if ( m_bIsVertical )
	{
		pos.m_x += m_push;
	}
	else
	{
		pos.m_y += m_push;
	}
}

SDL_bool CollideWall::isMiddle(double a, double middle, double b)
{
	// returns true if (a <= middle <= b) or if (b <= middle <= a)

	if ( a <= middle )
	{
		if ( b >= middle ) return SDL_TRUE;
		return SDL_FALSE;
	}

	// a must be > middle )
	if ( b <= middle ) return SDL_TRUE;
	return SDL_FALSE;
}

