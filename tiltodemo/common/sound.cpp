#include "sound.h"
#include "SDL.h"

// the number of channels. For an explanation of what this
// is about, search this file for for Mix_AllocateChannels
#define NUM_CHANNELS 16

// streamed music is kept in a data structure called a Mix_Music.
// You can only have one stream playing at a time, and it's your 
// responsibility to load and free it. We will store our music in
// this static variable
Mix_Music *g_streamingMusic = NULL;

void initSound()
{
	// we'll use SDL_Mixer to do all our sound playback. 
	// It's a simple system that's easy to use. The most 
	// complicated part about using SLD_Mixer is initting it. So 
	// let's talk about that. 
	// this is the function definition:
	// Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize);
	// here's what it wants:
	// 
	// frequency: this is the sample rate you want the audio to
	// play at. On the Palm, things are optimized for 44100 samples per
	// second (CD quality). Though you can send in whatever sample rate you like.
	// 
	// format: For the Palm, you should use AUDIO_S16
	// 
	// channels: 1 for mono. 2 for stereo
	// 
	// chunksize: How big one audio buffer is, in bytes.
	// 
	// this example has the recommended settings for initting the mixer:
	int rate = 44100;
	Uint16 format = AUDIO_S16;
	int channels = 2;
	int bufferSize = 1024;
	if ( Mix_OpenAudio(rate, format, channels, bufferSize) )
	{
		// we had an error opening the audio
		printf("unable to open Audio! Reason: %s\n", Mix_GetError());
		exit(1);
	}

	// SDL mixer requires that you specify the maximum number of simultaneous
	// sounds you will have. This is done through the function Mix_AllocateChannels. 
	// We'll need an answer for that. A channel has very little overhead,
	// so we'll arbitrarily allocate 16 channels (even though we're only using
	// 1 actual sound channel). Among other things, we need a channel per unique
	// simultaneous sound playback. So if we play the same sound 4 times, and have
	// 4 copies of the sound playing from different start times, we need four channels.
	// Note - streamed music does not consume a channel. Only sound playback does.
	// So why allocate more than we need? Because it's such a small overhead, and
	// later we don't need to come and revisit this every time we add a new sound. 
	Mix_AllocateChannels(NUM_CHANNELS);
}

void startStreamingMusic(const char *filename)
{
	// in this function, we will fire up a music stream and have
	// it play.
	// first thing we do is load up the music
	g_streamingMusic = Mix_LoadMUS(filename);

	// if it couldn't get ahold of the music, it
	// returns null
	if ( g_streamingMusic == NULL )
	{
		printf("Failed to open music file! Reason: %s\n", Mix_GetError());
		exit(1);
	}

	// now start it playing as a loop. To play looped, send -1 as the number
	// of loops to play. If you didn't want it to loop, you'd simply send 
	// however many times through you want it to REplay. This is not the number
	// of times it will play, it's the number of times it will REplay. 
	// so if you want it to play once you send in 0. 
	Mix_PlayMusic(g_streamingMusic, -1);

	// that's all there is to it
}

Mix_Chunk *loadSound(const char *filename)
{
	// This is how you load a wav file. Simple enough, eh?
	Mix_Chunk *ret = Mix_LoadWAV(filename);

	// if it failed to load, it will return NULL
	if ( ret == NULL )
	{
		printf("Could not load wav file %s... Reason: %s", filename, Mix_GetError());
		exit(1);
	}

	// if we're here, we have a valid, loaded sound
	return ret;
}

void unloadSound(Mix_Chunk *sound)
{
	// deallocate the sound and remove it from the system.
	Mix_FreeChunk(sound);
}

int playSound(Mix_Chunk *sound)
{
	// we will kick off playback of this sound,
	// and return the channel it was playing on
	// the first parameter is the channel you want to 
	// play the sound on. If you just want it to play
	// and don't care what channel it's on, send -1, and
	// SDL_Mixer will find an unused channel to play it in.
	// -1 is the most common way to send things along, however
	// more advanced sound manipulation may cause you to care 
	// about which channel things play on. ths final
	// value in the function is the number of times you want
	// it to replay. As with looping music, sending -1 will cause
	// it to loop forever. And as with looping music, this is the
	// number of times you want it to REplay. So sending a value
	// of 0 will make it play once.
	int channel = Mix_PlayChannel(-1, sound, 0);
	return channel;
}

void quitSound()
{
	// halt any streaming music playback in progress
	Mix_HaltMusic();
	if ( g_streamingMusic != NULL )
	{
		// free up any memore in use by the music track
		Mix_FreeMusic(g_streamingMusic);

		// set it to null for safety (ensuring that 
		// if we acidentally refer to this variable after
		// deletion, at least it will be null)
		g_streamingMusic = NULL; 
	}

	// close out the audio
	Mix_CloseAudio();
}


