// SDL Graphics - Here's where we put all the SDL related graphics stuff. All in one 
// convienent file for your perusal. Note that the ENTIRE file is wrapped in an
// ifdef of USE_OGL. USE_OGL is defined in globals.h. See globals.h for an explanation 
// of how that works
#include "globals.h"

#if !USE_OGL

#include "SDL.h"
#include "SDL_image.h"
#include "graphics.h"

// the variable declaration of the screen global
SDL_Surface *g_screen = NULL;

// SDL does not require any particular graphics init. 
// Or, more accurately, it did that in SDL_Init because we sent
// in the flag SDL_INIT_VIDEO (see sdl_init.cpp).
void graphics_init()
{
}

// filling a rect is very straightforward in sdl. 
void fillRect(int x, int y, int w, int h, int color)
{
	// define a rect that we'll be filling
	SDL_Rect r;
	r.x = (Sint16)x;
	r.y = (Sint16)y;
	r.w = (Sint16)w;
	r.h = (Sint16)h;

	// then fill it.
	SDL_FillRect(g_screen, &r, color);
}

void drawImage(Image *img, int x, int y)
{
	// Despite the fact that you send a source RECT 
	// and a destination RECT, SDL_BlitSurface doesn't
	// actually stretch or shrink pixels. It will blit
	// the source rect portion of the source to the location
	// specified in the dest rect. but the dest rect width and
	// height are ignored.
	SDL_Rect destRect = { (Sint16)x, (Sint16)y, 0, 0 };
	SDL_BlitSurface(img->m_surface, NULL, g_screen, &destRect);
}

// SDL doesn't actually blit until you call SDL_UpdateRect
void frameDone()
{
	// update the screen
	SDL_UpdateRect(g_screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);    
}

Image *loadImage(const char *fileName)
{
	// simply instance an Image, and tell it to load.
	Image *ret = new Image();
	ret->load(fileName);
	return ret;
}

void freeImage(Image *img)
{
	delete img;
}

/********************* IMAGE CLASS ********************/
/********************* IMAGE CLASS ********************/
/********************* IMAGE CLASS ********************/
Image::Image()
{
	m_surface = NULL;
}

Image::~Image()
{
	// free the surface
	if ( m_surface != NULL )
	{
		SDL_FreeSurface(m_surface);
		m_surface = NULL;
	}
}

// load an image into an SDL surface. The sent-in
// parameter is the file name. 
void Image::load(const char *fileName)
{
	// this is really insanely easy. But broken into 
	// a seperate function so it will be easy for you to find.
	// you ready for this? Ok watch closely...
	m_surface = IMG_Load(fileName);

	// your image is now loaded. Whew! that was rough, eh?
	// well, ok, there might have been a problem. And here's a 
	// place for you to drop a breakpoint if there was. Most of 
	// the time, if there's a problem, it will be because your
	// file name is incorrect, or the working path isn't what
	// you expected or something like that. But check the error
	// string to find out what went wrong.
	if ( !m_surface )
	{
		printf("Could not load image %s. Reason: %s\n", fileName, SDL_GetError());
		exit(1);
	}

	// now we have the image, set up our internals
	m_width = m_surface->w;
	m_height = m_surface->h;
}

#endif // !USE_OGL

