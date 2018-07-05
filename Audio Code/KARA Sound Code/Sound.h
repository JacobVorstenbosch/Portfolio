//// KARA ////

#pragma once
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#include "VecMath.h"

#include <string>
#include <iostream>

//uncertain if these are needed - will check
//#include <conio.h>
//#include <stdio.h>

//Global Error Checking - Used EVERYWHERE
void FMODError(FMOD_RESULT _result);

//System class, for initializing a lot of the background FMOD stuff
class System
{
public:
	static System* getInstance()
	{
		static System instance;
		return &instance;
	}
	FMOD::System *system;
	System();
	~System();

//	Listener Stuff: Perhaps move to it's own class
	void changeListenerPos(Vec);
	FMOD_VECTOR listenerPos, listenerVel, listenerUp, listenerForward;
	
private:
	float count;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS caps;
	unsigned int version;
	int numDrivers;
	char name[256];
};

//Sound class, for creating and playing sounds, and manipulating volume, position and channels
class Sound
{
public:

	Sound();
	~Sound();
	void createSound(FMOD::Sound **_audio, std::string fileName);
	void createStream(FMOD::Sound **stream, std::string fileName);
	void playSound(FMOD::Sound *_audio, FMOD::Channel* channel, bool mode, bool pause);

	void loadSounds();

	void playSFX(int state, Vec pos);
	void playBackground(int);

	void setChannels();
	void setPosition(FMOD::Channel* channel, float x, float y, float z);
	void setVol(float vol);
	void pauseBackground(int channel);

private:
	FMOD::Sound *sfx[8];
	FMOD::Sound *background[3]; 
	
	FMOD::Channel *sfxChannel[8];
	FMOD::Channel *backgroundChannel[3];

	FMOD::ChannelGroup *backgroundGroup, *sfxGroup, *masterGroup;

	FMOD_VECTOR forward, up, vel, pos;
	
	float bVol, sfxVol; //background volume, sfx volume
	bool mode; // true = sfxChannel, false - backgroundChannel
};
