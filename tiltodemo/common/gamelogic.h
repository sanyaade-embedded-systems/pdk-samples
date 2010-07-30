#ifndef __GAMELOGIC__

#include "SDL_Mixer.h"
#include "geometry.h"
#include "graphics.h"

class CollideWall;

// The main game logic class. As with all the classes
// in this sample app, the bulk of the comments are in the cpp code. 

class GameLogic
{
public:

	// an arbitrary maximum number of collide walls. 
	// we allocate each as needed, but we'd rather not 
	// implement a growing array system just to avoid having
	// a maximum. The same idea applies to the visible blocks
	// and pits.
	static const int MAX_COLLIDEWALLS = 256;
	static const int MAX_BLOCKS = 256;
	static const int MAX_PITS = 256;

	// acceleration due to gravity. This is in 
	// pixels per second per second. Same as any other 
	// physics model, but instead of meters, we're using
	// pixels as our primary unit of distance. Unfortunately,
	// you're not allowed to specify a double as an initialized
	// static const. Rather than use a #define, we will instead
	// have a member variable that is a double, which we'll init
	// to this value.
	static const int GRAVITY_ACC_PPSPS_INT = 300;
	static const int MINIMUM_REBOUND_VELOCITY_INT = 5;

	// member functions. See the cpp file for detailed comments
	GameLogic();
	virtual ~GameLogic();

	void run();
	void init();
	void eventloop();
	void tick();
	void draw();
	void addCollideRect(int x, int y, int width, int height);
	void addPit(int x, int y, int width, int height);
	void reset();
	const char *getPath(const char *file);


	// wants to be static. We would rather have this be a static
	// constant, but we can't do that with doubles. So we make it *look* like 
	// a static constant with the naming. Tell no one.
	double GRAVITY_ACC_PPSPS;
	double MINIMUM_REBOUND_VELOCITY;

	/*************************** DATA *****************************/
	// true if the game is running. false if it's time to return
	SDL_bool m_bGameRunning; 

	// the radius of the ball. Critical in collisioning and physics
	int m_ballRadius;

	// art assets
	Image *img_ball; // the ball
	Image *img_exit; // the exit
	Image *img_pit;  // the pit

	// ball physics values
	Vector m_ballPos; // current location of the ball's center
	Vector m_ballVel; // current velocity of the ball, in pixels per second

	// collision segments
	CollideWall *m_collideWalls[MAX_COLLIDEWALLS];
	int m_numCollideWalls;

	// visible rects
	SDL_Rect *m_blocks[MAX_BLOCKS];
	int m_numBlocks;

	// deadly pits of deadly deadliness
	// also they are deadly
	SDL_Rect *m_pits[MAX_PITS]; // the physical location of the pit
	SDL_Rect *m_pitDisplays[MAX_PITS]; // the visible portion of the pit
	int m_numPits;

	// the exit. If they hit this, they win
	SDL_Rect m_exitRect;

	// time management
	int m_lastTicks;

	// the sound effect we'll be using
	Mix_Chunk *m_wallHitSound;

	// the "You Lose" sound effect
	Mix_Chunk *m_gameLoseSound;

	// the "Victory" music
	Mix_Chunk *m_gameWinSound;
	
	// The string used to build the path to various resources, which will vary 
	// depending on the platform
	char m_workingStr[256];
};

#endif


