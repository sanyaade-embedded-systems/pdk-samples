#ifndef __SOUND__
#define __SOUND__

#include "SDL_mixer.h"
// companion header to sound.cpp. Functions are heavily 
// commented in their source. See sound.cpp

void initSound();
void quitSound();

// streaming music stuff
void startStreamingMusic(const char *filename);

// sound stuff
Mix_Chunk *loadSound(const char *filename);
void unloadSound(Mix_Chunk *sound);
int playSound(Mix_Chunk *sound);

#endif

