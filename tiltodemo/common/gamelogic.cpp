#include "SDL.h"
#include "graphics.h"
#include "globals.h"
#include "gamelogic.h"
#include "accelerometer.h"
#include "geometry.h"
#include "collision.h"
#include "sound.h"

// This is the main game logic class. Everything related to the actual
// game is in here. The main loop, event loop, drawing, etc. is all
// here as well. 

GameLogic::GameLogic()
{
	// because ANSI (for some reason) won't let you init a double
	// as a static const member of a class, we have to init it
	// in the constructor. So we made a class member, GRAVITY_ACC_PPSPS
	// which is named in imposing all-caps so as to look like 
	// a constant, but really it's a normal member. We set it to 
	// GRAVITY_ACC_PPSPS_INT which *is* a constant. This way, it's
	// a changeable constant that acts as you would expect. 
	GRAVITY_ACC_PPSPS = GRAVITY_ACC_PPSPS_INT;
	MINIMUM_REBOUND_VELOCITY = MINIMUM_REBOUND_VELOCITY_INT;

	// init the number of walls and blocks to 0. The rest of our
	// initialization is done in init(). But we clear these values in case
	// a GameLogic is ever instanced, then deleted before init() is called. 
	// We care because these values are used in the destructor.
	m_numCollideWalls = 0;
	m_numBlocks = 0;
	m_numPits = 0;
	img_ball = NULL;
}

GameLogic::~GameLogic()
{
	// Unload all images
	freeImage(img_ball);
	freeImage(img_exit);
	freeImage(img_pit);

	// clear out those collide walls
	for ( int i=0 ; i<m_numCollideWalls ; i++ )
	{
		delete m_collideWalls[i];
	}

	// clear out the blocks
	for ( int i=0 ; i<m_numBlocks; i++ )
	{
		delete m_blocks[i];
	}

	// clear out the pits
	for ( int i=0 ; i<m_numPits ; i++ )
	{
		delete m_pits[i];
		delete m_pitDisplays[i];
	}
}

// called from main, this is the main run function of our game. 
void GameLogic::run()
{
	// get the stuff we need
	init();

	m_bGameRunning = SDL_TRUE;
	while ( m_bGameRunning ) 
	{
		eventloop();
		tick();
		draw();

		// You should always have a little delay (recommend 10ms)
		// at the end of your event loop. This keeps SDL from
		// hogging all the CPU time. 
		SDL_Delay(10); 
	}
}

void GameLogic::eventloop()
{
	// we'll be using this for event polling
    SDL_Event ev;

	// this is how you poll for events. You can't just
	// ignore this function and go your own way. SDL does
	// critical tasks during SDL_PollEvent. You have to call
	// it, and call it frequently. If you have some very large
	// task to perform, like loading hundreds of images, you'll
	// have to break it up in to bite sized pieces. Don't 
	// starve SDL. 
	while (SDL_PollEvent(&ev)) 
	{
		switch (ev.type) 
		{
			case SDL_QUIT:
			{
				m_bGameRunning = SDL_FALSE;
			}
			break; 

			case SDL_JOYAXISMOTION:
			{
				int accelerometer = ev.jaxis.axis;
				int val = ev.jaxis.value;
				updateAccelerometer(accelerometer, val);
			}
			break;
		}
	}
}

void GameLogic::tick()
{
	// note the number of ms that have passed since our
	// last call to tick. 
	int ticks = SDL_GetTicks();
	int ms = ticks - m_lastTicks;
	m_lastTicks = ticks;

	// this is a very handy way to keep things under control
	// when you're debugging. This way, when you break in a function and
	// look around for a while, the following game loop doesn't have a
	// huge ticks value since the last loop. 
	if ( ms > 100 ) ms = 100;

	// apply the current accelerations to the velocity
	// to do that, we first must figure out the accelerations.
	// We are getting values from -1.0 to 1.0 from our 
	// accelerometer management code (see accelerometer.cpp). 
	// so we simply multiply that ratio value by our gravity constant.
	double accX = getAccelerometerX()*GRAVITY_ACC_PPSPS;
	double accY = getAccelerometerY()*GRAVITY_ACC_PPSPS;

	// the function applyPPSVel applies the sent in 
	// pixels-per-second velocity to the start value, 
	// ratioed to the number of milliseconds sent in. 
	// the process is exactly the same when applying an acceleration
	// to a velocity. 
	m_ballVel.m_x = applyPPSVel(m_ballVel.m_x, accX, ms);
	m_ballVel.m_y = applyPPSVel(m_ballVel.m_y, accY, ms);

	// now the ball's velocity is updated. Apply it to the ball's location
	// this tiem we use a different version of applyPPSVel. This one takes
	// vectors (instead of doubles). Rather than updating the ball's 
	// position, it merely puts the resultant position into "wantPos". 
	// We still need to do collision detection, so we need the start and 
	// end positions of the bal to be distinct entities.
	Vector wantPos;
	applyPPSVel(m_ballPos, m_ballVel, ms, wantPos);

	// now we have the ball's start and end location
	// check to see if it hit any colliders. The new 
	// position will ultimately be in postCollision
	Vector postCollision;
	CollideWall *collidedWith = NULL;
	for ( int i=0 ; i<m_numCollideWalls ; i++ )
	{
		SDL_bool bCollided = m_collideWalls[i]->checkCollision(m_ballPos, wantPos, postCollision);
		if ( bCollided )
		{
			// we collided with something. postCollision will be set to the point
			// where we should stop. Change our new destination to be that, and
			// continue with the loop. We may have hit multiple collision walls
			// in a single tick. We want to stop at whichever one we hit first. Doing
			// this guarantees that whichever one we hit first ends up being the
			// last one that registers a collision. All the colliders we hit that are 
			// before the winner in the array will end up putting postCollision further than 
			// where we hit the winner. All of the colliders after the winner will not 
			// register a collision at all, because the winner will have put postCollision
			// to a point before the movement intersected them. 
			wantPos.set(postCollision);
			collidedWith = m_collideWalls[i];
		}
	}

	// we're done checking for collisions. wantPos will now be the
	// correct location for the ball.
	m_ballPos.set(wantPos);

	// did we collide with anything?
	if ( collidedWith != NULL )
	{
		// we collided. We need to apply that collider's 
		// push value to our location. This shoves us off the wall
		// a bit, to keep us from being right on it.
		collidedWith->push(m_ballPos);

		// colliding changes our velocity.
		// we bounce.
		if ( collidedWith->m_bIsVertical )
		{
			// it's a vertical wall. Our x
			// velocity needs to reverse. Our
			// y velocity is unaffected
			// CG: Reduce the x velocity by 25%, and reduce it to 0 if it's less
			// than MINIMUM_REBOUND_VELOCITY
			m_ballVel.m_x = -0.25 * m_ballVel.m_x;
			if ((m_ballVel.m_x * m_ballVel.m_x) < (MINIMUM_REBOUND_VELOCITY*MINIMUM_REBOUND_VELOCITY)) {
				m_ballVel.m_x = 0;
			}
		}
		else
		{
			// it's a horizontal wall. Our y
			// velocity needs to reverse. Our
			// x velocity is unaffected
			// CG: Reduce the y velocity by 25%, and reduce it to 0 if it's less
			// than MINIMUM_REBOUND_VELOCITY
			m_ballVel.m_y = -0.25 * m_ballVel.m_y;
			if ((m_ballVel.m_y * m_ballVel.m_y) < (MINIMUM_REBOUND_VELOCITY*MINIMUM_REBOUND_VELOCITY)) {
				m_ballVel.m_y = 0;
			}
		}

		// we'll say an arbitrary 25%
		// of the ball's velocity is lost in the collision
		// m_ballVel.setLength(0.75*m_ballVel.getLength());
		// CG: Trying a different physics method, within the individual direction changes.

		// and finally, hitting a wall means we play the
		// collide sound
		playSound(m_wallHitSound);
	}

	// now we're done with collision detection. The ball's position
	// is correctly set. Now it's time to check for pits. 
	for ( int i=0 ; i<m_numPits ; i++ )
	{
		if ( ptInRect((int)m_ballPos.m_x, (int)m_ballPos.m_y, m_pits[i]) )
		{
			// fell in to a pit.
			// CG: Added a you lose sound
			playSound(m_gameLoseSound);
			reset();
			return;
		}
	}

	// and finally, see if they hit the exit
	if ( ptInRect((int)m_ballPos.m_x, (int)m_ballPos.m_y, &m_exitRect) )
	{
		// they win the game! Their reward is a brief pause followed by
		// the game resetting. Ah what a warm feeling that will give them
		// CG: Added a victory sound
		playSound(m_gameWinSound);
		SDL_Delay(1000); 
		reset();
		return;
	}
}

void GameLogic::draw()
{
	// clear the screen
	fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	// now draw the blocks
	for ( int i=0 ; i<m_numBlocks ; i++ )
	{
		fillRect(m_blocks[i]->x, m_blocks[i]->y, m_blocks[i]->w, m_blocks[i]->h, 0x0000ff);
	}

	// and now the pits
	for ( int i=0 ; i<m_numPits ; i++ )
	{
		// fillRect(m_pitDisplays[i]->x, m_pitDisplays[i]->y, m_pitDisplays[i]->w, m_pitDisplays[i]->h, 0xffffff);
		// Uncomment the above line and comment the next to switch from a pit image to a solid white block
		drawImage(img_pit, m_pitDisplays[i]->x, m_pitDisplays[i]->y);

	}

	// finally, the exit
	// fillRect(m_exitRect.x, m_exitRect.y, m_exitRect.w, m_exitRect.h, 0xff0000);
	// Uncomment the above line and comment the next to switch from an exit image to a solid red block
	drawImage(img_exit, m_exitRect.x, m_exitRect.y);

	// draw the ball. Remember the ball pos is 
	// for the *centeR* of the ball. We need to tell it 
	// the location of the top left. So we subtract the
	// radius from it.
	int ballDrawX = (int)m_ballPos.m_x - m_ballRadius;
	int ballDrawY = (int)m_ballPos.m_y - m_ballRadius;
	drawImage(img_ball, ballDrawX, ballDrawY);

	frameDone();
}

	//getPath returns a path appropriate to the platform.
	//MacOS builds create a different folder structure than
	//Windows or Palm for all resources
const char *GameLogic::getPath(const char *file)
{
#if __MACOSX__
	sprintf(m_workingStr, "./tiltodemo.app/Contents/Resources/%s", file);
	return m_workingStr;
#else
	return file;
#endif
}

void GameLogic::init()
{
	// fire up the accelerometers
	accelerometer_init();

	// load images
	img_ball = loadImage(getPath("ball.png"));
	img_pit = loadImage(getPath("pit.jpg"));
	img_exit = loadImage(getPath("exit.gif"));

	// note the ball radius. We extract it from the image of the ball.
	// data-driven design. More than just a coding philosophy. A way of life. 
	m_ballRadius = img_ball->m_width/2;

	// load our sound effects
	m_wallHitSound = loadSound(getPath("woodblock.wav"));
	m_gameLoseSound = loadSound(getPath("wawa.wav"));
	m_gameWinSound = loadSound(getPath("tada.wav"));

	// set up our collision segments. 
	// start with the 4 segments that comprise the outer wall.
	// see addColliderRect for an explanation of how we initalize collideWalls
	
	// you may wonder why we don't simply call addCollideRect to add this
	// outer rect. the reason is because these walls point *in* to the rect, while
	// the calls to addCollideRect cause walls that point *out*.

	// also remember, we are insetting everything by the radius of the ball.
	// so here are the x, y, w, and h values for the actual collideWall
	int boundsX1 = m_ballRadius;
	int boundsY1 = m_ballRadius;
	int boundsX2 = SCREEN_WIDTH - m_ballRadius;
	int boundsY2 = SCREEN_HEIGHT - m_ballRadius;
	int boundsW = SCREEN_WIDTH - m_ballRadius*2;
	int boundsH = SCREEN_HEIGHT - m_ballRadius*2;

	// top wall. It runs across the top of the screen and points down
	m_collideWalls[m_numCollideWalls]= new CollideWall();
	m_collideWalls[m_numCollideWalls]->initHorizontal(boundsX1, boundsY1, boundsW, 1); 
	m_numCollideWalls++;

	// bottom wall. Runs across the bottom of the screen and points up
	m_collideWalls[m_numCollideWalls]= new CollideWall();
	m_collideWalls[m_numCollideWalls]->initHorizontal(boundsX1, boundsY2, boundsW, -1); 
	m_numCollideWalls++;

	// left wall. Runs across the left side oft he screen and points right
	m_collideWalls[m_numCollideWalls]= new CollideWall();
	m_collideWalls[m_numCollideWalls]->initVertical(boundsX1, boundsY1, boundsH, 1); 
	m_numCollideWalls++;

	// right wall. Runs across the right side oft he screen and points left
	m_collideWalls[m_numCollideWalls]= new CollideWall();
	m_collideWalls[m_numCollideWalls]->initVertical(boundsX2, boundsY1, boundsH, -1); 
	m_numCollideWalls++;

	// now, add the collide rects. These were carefully crafted by great
	// artisans to provide maximum enjoyability in gameplay. Or, perhaps they
	// were arbitrarily strewn around because this is a silly sample app and 
	// playability doesn't matter. 
	addCollideRect(0, 50, 270, 8);
	addCollideRect(0, 100, 135, 8);
	addCollideRect(185, 100, 135, 8);
	addCollideRect(0, 330, 135, 8);
	addCollideRect(185, 330, 135, 8);
	addCollideRect(50, 380, 270, 8);
	addCollideRect(50, 430, 220, 8);
	addCollideRect(265, 430, 8, 50);

	// now add some pits 
	addPit(85, 58, 42, 42);

	// set up the exit
	m_exitRect.x = 278;
	m_exitRect.y = 438;
	m_exitRect.w = 42;
	m_exitRect.h = 42;

	// initalize m_lastTicks. Just start it off at the 
	// current getTicks value.
	m_lastTicks = SDL_GetTicks();

	// set up for a game
	reset();

}

void GameLogic::addCollideRect(int x, int y, int width, int height)
{
	// first off, we add the rect to our list of blocks. We instance a new SDL_rect
	// for the purpose, then store it.
	SDL_Rect *newRect = new SDL_Rect();
	newRect->x = (Sint16)x;
	newRect->y = (Sint16)y;
	newRect->w = (Sint16)width;
	newRect->h = (Sint16)height;
	m_blocks[m_numBlocks] = newRect;
	m_numBlocks++;

	// we don't want to do complex calculations to figure out of any part
	// of a circle hit a collider wall. So instead of working out how to 
	// do that, we outset the rect by the radius of the ball. That way, all we
	// have to do is check collisioning on the center of the ball. So we start
	// by outsettingthe rect.
	x -= m_ballRadius;
	y -= m_ballRadius;
	width += m_ballRadius*2;
	height += m_ballRadius*2;

	// now make 4 collide walls out of this rect. Each collideWall 
	// needs to know what direction it's facing. That's the "push"
	// value (see collide.cpp for more info). The push value is -1 or 1.
	// Its sign is in the direction that the collideWall faces. So if a
	// collide wall is facing up, the push will be -1 (because y negative is up)

	// the left wall. It runs vertical, and points left 
	m_collideWalls[m_numCollideWalls]= new CollideWall();
	m_collideWalls[m_numCollideWalls]->initVertical(x, y, height, -1); 
	m_numCollideWalls++;

	// the right wall. It runs vertical, and points right 
	m_collideWalls[m_numCollideWalls]= new CollideWall();
	m_collideWalls[m_numCollideWalls]->initVertical(x+width-1, y, height, 1); 
	m_numCollideWalls++;

	// the top wall. It runs horizontal, and points up
	m_collideWalls[m_numCollideWalls]= new CollideWall();
	m_collideWalls[m_numCollideWalls]->initHorizontal(x, y, width, -1); 
	m_numCollideWalls++;

	// the bottom wall. It runs horizontal, and points down
	m_collideWalls[m_numCollideWalls]= new CollideWall();
	m_collideWalls[m_numCollideWalls]->initHorizontal(x, y+height-1, width, 1); 
	m_numCollideWalls++;
}

void GameLogic::addPit(int x, int y, int width, int height)
{
	// Remember that all checking is done from the CENTER of the ball.
	// it would look a little weird if you could hang almost half the
	// ball over the pit. So we'll outset the pit by half the ball's
	// radius. This allows them to hang about 1/4th of the ball over
	// the edge of the pit before falling in.
	// Because of this, we have two different rects to track. We have to
	// note the actuall collision area of the pit, and the visible portion 
	// of the pit. 

	// first, note the visible portion. This is simply the sent-in values
	SDL_Rect *newRect = new SDL_Rect();
	newRect->x = (Sint16)x;
	newRect->y = (Sint16)y;
	newRect->w = (Sint16)width;
	newRect->h = (Sint16)height;
	m_pitDisplays[m_numPits] = newRect;

	// now make note of the collision area of this pit
	newRect = new SDL_Rect();
	newRect->x = (Sint16)(x - m_ballRadius/2);
	newRect->y = (Sint16)(y - m_ballRadius/2);
	newRect->w = (Sint16)(width + m_ballRadius/2);
	newRect->h = (Sint16)(height + m_ballRadius/2);
	m_pits[m_numPits] = newRect;

	// update the number of pits
	m_numPits++;
}

void GameLogic::reset()
{
	// start the ball in the upper corner
	// with no velocity
	m_ballPos.setXY(20, 20);
	m_ballVel.setXY(0, 0);
}

