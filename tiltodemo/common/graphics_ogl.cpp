// OGL_STUFF - Here's where we put all the OGL related stuff. All in one 
// convienent file for your perusal. Note that the ENTIRE file is wrapped in an
// ifdef of USE_OGL. USE_OGL is defined in globals.h. See globals.h for an explanation 
// of how that works
#include "globals.h"

#if USE_OGL

#include "graphics.h"
#include "SDL.h"
#include "SDL_image.h"

// the variable declaration of the screen global
// we need this even in the OGL build, because it
// is used in SDL initalization
SDL_Surface *g_screen = NULL;

// forward declarations
int flipY(int y);
int power_of_two(int input);

GLfloat g_coordinates[] = 
{  
   0.0, SCREEN_WIDTH,
   SCREEN_HEIGHT, SCREEN_WIDTH,
   0.0, 0.0,
   SCREEN_HEIGHT, 0.0 
};



// init ogl. This is farily standard. Palm's implementation of
// ogl is not special in any way. If you want information on the
// use of ogl, there are countless examples to be found online
void graphics_init()
{
	// set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glOrthof((GLfloat)0, (GLfloat)SCREEN_WIDTH, (GLfloat)0, (GLfloat)SCREEN_HEIGHT, (GLfloat)-1, (GLfloat)1);
	glMatrixMode(GL_MODELVIEW);

	// set up the basic rendering states
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_BLEND);

	glTexCoordPointer(2, GL_FLOAT, 0, g_coordinates); 
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

void drawImage(Image *img, int x, int y)
{
	GLfloat fX = (GLfloat)x;

	// see flipY for an explanation of this
	GLfloat fY = (GLfloat)(flipY(img->m_height+y));
	
	GLfloat	coordinates[] = { 0, img->m_textureHeight, img->m_textureWidth, img->m_textureHeight, 0, 0, img->m_textureWidth, 0 };
	GLfloat		vertices[] = 
	{ 
		fX, fY, 0.0,
		img->m_width+fX, fY, 0.0,
		fX, img->m_height+fY, 0.0,
		img->m_width+fX, img->m_height+fY, 0.0 
	};	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, img->m_name);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void fillRect(int x, int y, int w, int h, int color)
{
	// see flipY to see why we're doing this
	y = flipY(y);

	// set the color, OGL style. This requires us to
	// know the individual components
	int r = (color&0x00ff0000)>>16;
	int g = (color&0x0000ff00)>>8;
	int b = (color&0x000000ff);
	GLfloat colorRed = ((float)r)/(float)255.0;
	GLfloat colorGreen = ((float)g)/(float)255.0;
	GLfloat colorBlue = ((float)b)/(float)255.0;
	glColor4f(colorRed, colorGreen, colorBlue, 255.0);

	// drawing raw primatives, so we need to set
	// up for that.
	glBindTexture(GL_TEXTURE_2D, 0);

	// make the array of points to draw
	GLfloat fX = (GLfloat)x;
	GLfloat fY = (GLfloat)y;
	GLfloat fX2 = (GLfloat)(x+w);
	GLfloat fY2 = (GLfloat)(y-h); // we subtract because down is positive in OGL land
	
	const GLfloat vertices[] = 
	{
		fX2, fY, 0.0,
		fX, fY, 0.0,
		fX2, fY2, 0.0,
		fX, fY2, 0.0
	};
	
	// draw the filled rect
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void frameDone()
{
	// tell OGL that we're done giving it operations
	// and it's time to execute them
	glFlush();
	glFinish();

	// tell SDL it's time to flip surfaces
	SDL_GL_SwapBuffers();
}


/********************* IMAGE CLASS ********************/
/********************* IMAGE CLASS ********************/
/********************* IMAGE CLASS ********************/
Image::Image()
{
	m_name = 0;
}

Image::~Image()
{
	// free the image (if any)
	if ( m_name != 0 )
	{
		glDeleteTextures(1, &m_name);
		m_name = 0;
	}
}

// load an image into an OGL surface. The sent-in
// parameter is the file name. 
void Image::load(const char *fileName)
{
	// we start by loading an SDL surface. 
	// this is the exact same thing we do in 
	// the non OGL version of this function. To see
	// detailed comments on loading an SDL image, see
	// Image::load in the file graphics_sdl.cpp
	SDL_Surface *surface = IMG_Load(fileName);
	if ( !surface )
	{
		printf("Could not load image %s. Reason: %s\n", fileName, SDL_GetError());
		exit(1);
	}

	// set up our internals
	m_width = surface->w;
	m_height = surface->h;

	// and now, it's time to make an OGL image out of this SDL image
	// OGL requires powers of 2 for the width and height
	int pow2W = power_of_two(surface->w);
	int pow2H = power_of_two(surface->h);

	// we wont necessarily be using the whole texture.
	// the texture will be the power of two sizes. 
	// So we note the texture coordinates needed to 
	// end up using *only* the actual image portion of the texture.
	m_textureWidth = (GLfloat)m_width / (GLfloat)pow2W;	
	m_textureHeight = (GLfloat)m_height / (GLfloat)pow2H;	

	// now, we make and SDL image that is RGB
	// this is complicated by the rgba masks being
	// different in big endian / little endian modes. So
	// we compile-switch for that.
	// Why are we making this intermediate image?
	// because we need to create an OGL image with all data
	// in hand at the time of creation. and that means we
	// need an SDL image that is already set up to
	// be the power-of-two compliant sizes. So all we're doing
	// here is making another SDL image, one that encodes as
	// RGB, which has power of two sizes, and we'll copy the original
	// image into this one. 
	SDL_Surface *image = SDL_CreateRGBSurface(
			SDL_SWSURFACE,
			pow2W, pow2H,
			32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
			0x000000FF, 
			0x0000FF00, 
			0x00FF0000, 
			0xFF000000
#else
			0xFF000000,
			0x00FF0000, 
			0x0000FF00, 
			0x000000FF
#endif
		       );

	if ( image == NULL ) 
	{
		// fail
		printf("Could create intermediate texture while loading image %s\n", fileName);
		exit(1);
	}

	// we'll need to blit with our own alpha blending attributes.
	// we don't want to mess up whatever was there before, so we'll
	// save them and put them back the way we found them when we're done.
	Uint32 saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
	Uint8 saved_alpha = surface->format->alpha;
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) 
	{
		SDL_SetAlpha(surface, 0, 0);
	}

	// copy the original image into our new power of 2
	// RGB image.
	SDL_Rect area;
	area.x = 0;
	area.y = 0;
	area.w = (Uint16)m_width;
	area.h = (Uint16)m_height;
	SDL_BlitSurface(surface, &area, image, &area);

	// put these guys back the way we found them 
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) 
	{
		SDL_SetAlpha(surface, saved_flags, saved_alpha);
	}

	// Create an OpenGL texture for the image 
	glGenTextures(1, &m_name);
	glBindTexture(GL_TEXTURE_2D, m_name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,
		     0,
		     GL_RGBA,
		     pow2W, pow2H,
		     0,
		     GL_RGBA,
		     GL_UNSIGNED_BYTE,
		     image->pixels);

	// the SDL image we made is no longer needed
	SDL_FreeSurface(image); 

	// we no longer need the SDL surface. We duplicated all the data from
	// it and have our OGL surface all set up.
	SDL_FreeSurface(surface);
}

// find the next power of 2 that is 
// equal to or above the in value
int power_of_two(int input)
{
	int value = 1;
	while ( value < input ) 
	{
		value <<= 1;
	}
	return value;
}

int flipY(int y)
{
	// While most graphics systems have y increasing 
	// as you go down, OGL decided to have it increase as you go 
	// up. This app can be made to work with SDL or OGL graphics 
	// with the flip of a compile switch, so we need a common 
	// coordinate system. So we elected to use SDL's coordinate 
	// system in the game logic, and have ths graphics functions 
	// responsible for converting to OGL coordinates internally.
	return SCREEN_HEIGHT - y;
}

#endif // USE_OGL


