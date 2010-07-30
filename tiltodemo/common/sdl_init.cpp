#include "SDL.h"
#include <GLES/gl.h>
#include "sdl_init.h" 
#include "globals.h"
#include "graphics.h" 
#include "PDL.h"

// sdl_init.cpp
// These functions all relate to the initalization of SDL. They are broken off and 
// placed together in this file so you can find them easier. You would probably have 
// them all in your main.cpp or equivalent file. There's really no need for them to 
// be all by themselves here other than to make them easy to find. This is example 
// code, after all.

// palceholder
bool parseNullsepList(const char *nullsepList, int listBufferSize, char **arrayToFill, int numElements);

// When running under windows, it is necessary to make additional OGL
// setup calls. We forward declare them here. Note that in the palm version
// this all gets #ifdef'd out.
// also, if we're not using OGL at all, this doesn't get compiled either. 
#if USE_OGL & WIN32
		// the forward declaration of the library loder. This is necessary to actually
		// load up the OGL lib. You can copy-paste this directly into your code. 
		// You'll never need to change it.
		extern "C" 
		{
			GL_API int GL_APIENTRY _dgles_load_library(void *, void *(*)(void *, const char *));
		};

		// this function is necessary for the call to _dgles_load_library. Internally,
		// it is a callback that the OGL lib calls to find out where various functions are.
		// You won't have to change this. Copy-paste is your friend.
		static void *proc_loader(void *h, const char *name)
		{
			(void) h; // make the compiler not complain about unused formal parameters
			return SDL_GL_GetProcAddress(name);
		}
#endif // USE_OGL & WIN32

// initSDL() - initalizes the SDL libraries.
int initSDL()
{
	// used to get the result value of SDL operations
	int result;

	/*
	char buffer[1024];
	sprintf(buffer, "test:bar:bas");
	buffer[4] = 0;
	buffer[8] = 0;
	char *names[3];
	parseNullsepList(buffer, 1024, names, 3);
	*/

	PDL_ScreenTimeoutEnable(PDL_FALSE);

	// init SDL. This function is all it takes to init both
	// the audio and video.
	result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK );
	if ( result < 0 )
	{
		return result;
	}

	// On the actual device, you do not need to do anything special
	// to use OpenGL. but on windows, you need to load the library
	// in order to use it. So this code block is ifdefed only for windows.
	// if we're not using OGL, this is unnecessary
#if USE_OGL & WIN32
	{
		result = SDL_GL_LoadLibrary("opengl32.dll");
		if (result != 0)
		{
			return result;
		}
	}
#endif

#if USE_OGL
	Uint32 videoFlags = SDL_OPENGL;

	// we need to set up OGL to be using the version of OGL we want. This
	// example uses OGL 1.0. 2.0 is a completely different animal. If you fail
	// to speficy what OGL you want to use, it will be pure luck which one you
	// get. Don't leave that up to chance. 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);	// Force this to be version 1
#else
	Uint32 videoFlags = SDL_SWSURFACE;
#endif

	// now we init the screen. The size of the screen on the palm
	// is 320x480. We have those values set in SCREEN_WIDTH and SCREEN_HEIGHT
	// in globals.h because magic numbers are icky.	
	// This function will return the screen's surface.

	g_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, videoFlags);
	if ( g_screen == NULL )
	{
		// couldn't make that screen
		return 1;
	}

	// time for some windows-specific initalization again. This time, it's
	// the call necessary to load up the OGL library. Again, this is windows
	// specific, and not needed on the palm side. You may wonder where _dgles_load_library
	// lives. It's in the OGL static lib associated with the dll. that's why we 
	// forward declared it at the top of this file, because none of our headers have
	// heard of it. 
	// And again, if we're not using OGL, this is unnecessary
#if USE_OGL & WIN32
	_dgles_load_library(NULL, proc_loader);
#endif

	// now we init graphics
	graphics_init();

	// This sets the "caption" for whatever the window is. On windows, it's the window
	// title. On the palm, this functionality does not exist and the function is ignored.
	SDL_WM_SetCaption("Tilt-o-Demo", NULL);

	// Like SDL, we will have the standard of always returning 0 for
	// success, and nonzero for failure. If we're here, it means success!
	return 0;
}

void quitSDL()
{
	// much simpler than initting. 
	SDL_Quit();
}


bool parseNullsepList(const char *nullsepList, int listBufferSize, char **arrayToFill, int numElements)
{
    // parse the string
    char *p = (char *)nullsepList;
    p[listBufferSize-1] = 0; // one way or another, this buffer ends in a 0
    int pos = 0; // used to track how far into the string we are
    for ( int i=0 ; i<numElements ; i++ )
    {
    	// get the current element
    	char *start = p;
    	    	
    	// advance until we hit a 0 (buffer is guaranteed to end in a 0)
    	while (*p!=0) 
		{
			p++;
			pos++;

			if ( pos >= listBufferSize ) return false;
		}

    	int len = (int)(p-start);
    	
    	// make the string
    	arrayToFill[i] = new char[len+1];
    	
		// snprintf(arrayToFill[i], len+1, "%s", p); // copy over the string
		sprintf(arrayToFill[i], "%s", start); // copy over the string
    	
    	// advance past the 0
    	p++;
		pos++;
    }
	return true;
}

