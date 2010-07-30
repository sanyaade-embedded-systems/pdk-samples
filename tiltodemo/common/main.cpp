/* 
	TILT-O-DEMO   A demo of the Palm implementation of SDL.

	This application is here as sample code for all of you who, like many,
	learn best by example. Most of what you'd want to do in game development
	is encompassed in this app.

	This app's codebase is needlessly partitioned into discrete chunks based 
	on the functionality be demonstrated. Your actual app would probably not
	be so partitioned, but hopefully it will make it easier for you to track 
	down examples of exactly what you're doing, rather than having to find what
	you want in a soup of other code.

	It's heavily commented, so take a moment to read the comments above any portion
	of the code that is difficult to understand or is causing you problems in your
	own apps.
*/
#include "SDL.h"
#include "sdl_init.h"
#include "sound.h"
#include "gamelogic.h"
#include "PDL.h"

// main: the entry point. Birth of all applications. From here begins the 
// great adventure. 
int main(int argc, char *argv[])
{
	// eliminate unused formal parameter warnings.
	argc;
	argv;

	// init SDL. See sdl_init.cpp for this function
	int result = initSDL(); 
	if ( result != 0 )
	{
		// we had a problem initting SDL. You can put a breakpoint here
		// or a printf if you prefer to find out what went wrong. After
		// executing this line, "error" will contain a string describing 
		// the error encountered.
		const char *error = SDL_GetError();

		// to eliminate unused variable warnings. But we made it an executable
		// line so you could put a breakpoint there to see what's wrong if something
		// is amiss.
		error = error; 
	}

	// now init the sounds. This function is in sound.cpp
	initSound();

	// create the game and turn it loose. We allocate it as 
	// a pointer, rather than a stack object, because we want to 
	// tear it down BEFORE terminating SDL. (Otherwise, the systems
	// have already been torn down by the time GameLogic's destructor
	// is called, and chaos ensues.)
	GameLogic *game = new GameLogic();
	game->run();
	delete game;

	quitSound(); // in sound.cpp
	quitSDL(); // in sdl_init.cpp

	return 0;
}

