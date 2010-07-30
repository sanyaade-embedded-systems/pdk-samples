#ifndef __GRAPHICS__
#define __GRAPHICS__

// *** IMPORTANT NOTE *** IMPORTANT NOTE *** IMPORTANT NOTE *** IMPORTANT NOTE *** 
// there are two complete implementations of graphics in this app. Standard
// SDL, and OpenGL. which one is being used is defined by the USE_OGL define in 
// globals.h. See globals.h for more info. 

// the openGL versions of these functions can all be found in graphics_ogl.cpp
// the standard SDL version can all be found in graphics_sdl.cpp. Functions
// that are the same across both OGL and SDL (functions that don't directly 
// involve rendering) are in graphics_common.cpp.
// These files ifdef themselves out of play as needed based on the USE_OGL define.
// you don't need to remove either from the project. Just leave them both in. 

#include "SDL.h"
#include "globals.h"

#if USE_OGL
#include <GLES/gl.h>
#endif

// the SDL screen surface
extern SDL_Surface *g_screen;

// because we support both SDL and OGL version of the graphics,
// allowing you to see how to do things both SDL and OGL style, we need 
// to abstract out the image class. This is our container. To ensure
// we don't access aspects of it that we shouldn't (like looking
// at the m_surface variable when in OGL mode), we wrap the members
// in our OGL flag. This way, you get a compile error if you try to use
// it the wrong way.
// Yes, yes, we could have done this with an interface class, and 
// subclasses of OGLImage and SDLImage. But this is a compile switch,
// not multiple modes of functionality. 


class Image
{
public:
	Image();
	virtual ~Image();

	// load up the image from a file
	void load(const char *fileName);

#if USE_OGL
	// in OGL, we only have to hang on to the image name,
	// which is just an int.
	GLuint m_name;

	// because OGL requires powers of 2 for
	// the x and y size of textures, we will likely
	// have blank unused space in our texture. Because of this
	// we need to track the actual texture coordinates of the
	// *content* portion of the texture. So when we blit it, we
	// blit that part only.
	GLfloat m_textureWidth;
	GLfloat m_textureHeight;
#else
	// in SDL, we track an SDL_Surface
	SDL_Surface *m_surface;
#endif

	// both versions have these useful values
	int m_width;
	int m_height;
};

// forward declarations of graphics-related functionality. For comments on the functions,
// see the functions in either graphics_ogl.cpp, graphics_sdl.cpp, or graphics_common.cpp

// init the graphics engine
void graphics_init();

// load an image of any supported file type
Image *loadImage(const char *fileName);
void freeImage(Image *img);

// draw na image at the specified location
void drawImage(Image *img, int x, int y);

// fill a rect of the specified size and shape
// with the specified color. the color is in the
// form 0xrrggbb
void fillRect(int x, int y, int w, int h, int color);

// done with drawing, have OGL or SDL do their post drawing stuff
void frameDone();

#endif

