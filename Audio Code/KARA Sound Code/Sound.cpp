//// KARA ////

#include "Sound.h"
#include "Game.h"
#define DEG_TO_RAD (3.14159f / 180.0f)

FMOD_RESULT result;
void FMODError(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD error: " << result << " " << FMOD_ErrorString(result) << std::endl;
	}
}

//Based on FMOD Standard for Initializing the FMOD System
//Not Mine
System::System()
{
	count = 0;
	//create FMOD interface object
	result = FMOD::System_Create(&system);
	FMODError(result);

	//check version
	result = system->getVersion(&version);
	FMODError(result);

	if (version < FMOD_VERSION)
		std::cout << "Error: Using old version of FMOD " << version << ".  Program requires " << FMOD_VERSION << std::endl;

	//get number of sound cards
	result = system->getNumDrivers(&numDrivers);
	FMODError(result);

	if (numDrivers == 0)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		FMODError(result);
	}
	else
	{
		//get capabilities of the default (0) sound card
		result = system->getDriverCaps(0, &caps, 0, &speakerMode);
		FMODError(result);
		//set the speaker mode to match that in control panel
		result = system->setSpeakerMode(speakerMode);
		FMODError(result);
	}
	//if acceleration slider is set to off
	if (caps & FMOD_CAPS_HARDWARE_EMULATED)
	{
		result = system->setDSPBufferSize(1024, 10);
		FMODError(result);
	}


	////initialize FMOD////
	result = system->init(50, FMOD_INIT_NORMAL, 0);
	//first variable is # of channels, you need more # of channels than # of simultaneous sounds

	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		FMODError(result);

		result = system->init(50, FMOD_INIT_NORMAL, 0);
	}
	FMODError(result);

	//// ----Initialize Default Listener Position---- \\\\

	//change the listener pos, forward and up to our game's stuff
	listenerPos = { 0.0f, 0.0f, 0.0f };
	listenerVel = { 0.0f, 0.0f, 0.0f };
	listenerForward = { 0.0f, 0.0f, -1.0f };
	listenerUp = { 0.0f, 1.0f, 0.0f };
	result = system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
	FMODError(result);
}

System::~System()
{

}

//set position of listener - default is camera
void System::changeListenerPos(Vec pos)
{
	FMOD_VECTOR nPos;
	nPos.x = pos.x;
	nPos.y = pos.y;
	nPos.z = pos.z;
	result = System::getInstance()->system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
	FMODError(result);
}

Sound::Sound()
{
	//// Setup Default Values for Sound Object Position////
	vel = { 1.0f, 1.0f, 1.0f };
	pos = { 0.0f, 0.0f, -10.0f };
	forward = { 0.0f, 0.0f, -1.0f };
	up = { 0.0f, 1.0f, 0.0f };
}

Sound::~Sound()
{
	//if (sfx[])
	//	sfx[]->release();

}

//create instance of audio, best for SFX - from sound assignment!
void Sound::createSound(FMOD::Sound** _audio, std::string fileName)
{
	std::string pPath = "..//Assets//Audio//";
	pPath.append(fileName);
	const char* sFile = pPath.c_str();
	result = System::getInstance()->system->createSound(sFile, FMOD_3D, 0, _audio);
	FMODError(result);
}

//create instance of streaming audio, best for music and themes
void Sound::createStream(FMOD::Sound **_stream, std::string fileName)
{
	std::string pPath = "..//Assets//Audio//";
	pPath.append(fileName);
	const char* sFile = pPath.c_str();
	result = System::getInstance()->system->createStream(sFile, FMOD_LOOP_NORMAL, 0, _stream);
	FMODError(result);
}

//load all sounds at the beginning (probably a bad idea, but it's easier tbh
void Sound::loadSounds()
{
	createSound(&sfx[0], "First Jump.wav");
	createSound(&sfx[1], "Second Jump.wav");
	createSound(&sfx[2], "Arak First Jump.wav");
	createSound(&sfx[3], "Arak Second Jump.wav");
	createSound(&sfx[4], "Menu Music.wav");
	createSound(&sfx[5], "Menu Music 2.wav");
	createSound(&sfx[6], "Cooperative.wav");
	createSound(&sfx[7], "Versus.wav");
	createStream(&background[0], "Long Lost Memory.mp3");
	createStream(&background[1], "Spring Level 1.wav");
	createStream(&background[2], "Alt Spring Level 1.wav");
	setChannels();
	setVol(1.0f);
}

//plays instance of sound, places it into a channel
void Sound::playSound(FMOD::Sound* _audio, FMOD::Channel* _channel, bool mode, bool pause)
{
	result = System::getInstance()->system->playSound(FMOD_CHANNEL_FREE, _audio, true, &_channel);
	FMODError(result);
	
	//setting the channelGroups when the sound plays - this is how it works apparently
	if (mode) 
		result = _channel->setChannelGroup(sfxGroup);
	else
		result = _channel->setChannelGroup(backgroundGroup);
	FMODError(result);

	result = _channel->setPaused(pause);
	FMODError(result);
}

//Background music 
//1 is main theme, 2 is coop, 3 is versus 
void Sound::playBackground(int type)
{
	switch (type)
	{
	case 1:
		playSound(background[0], backgroundChannel[0], false, false);
		break;
	case 2:
		playSound(background[1], backgroundChannel[1], false, false);
		break;
	case 3:
		playSound(background[2], backgroundChannel[2], false, false);
		break;
	default:
		break;
	}	
}

//Cases 1 - 4 are jumping noises (1-2 KARA, 3-4 ARAK)
//Cases 5 - 6 are menu noises
void Sound::playSFX(int state, Vec pos)
{
	switch (state)
	{
	case 1:
		playSound(sfx[0], sfxChannel[0], true, false);
		setPosition(sfxChannel[0], pos.x, pos.y, pos.z);
		break;
	case 2:
		playSound(sfx[1], sfxChannel[1], true, false);
		setPosition(sfxChannel[1], pos.x, pos.y, pos.z);
		break;
	case 3:
		playSound(sfx[2], sfxChannel[2], true, false);
		setPosition(sfxChannel[2], pos.x, pos.y, pos.z);
		break;
	case 4:
		playSound(sfx[3], sfxChannel[3], true, false);
		setPosition(sfxChannel[3], pos.x, pos.y, pos.z);
		break;
	case 5:
		playSound(sfx[4], sfxChannel[4], true, false);
		setPosition(sfxChannel[4], 0, 0, 0);
		break;
	case 6:
		playSound(sfx[5], sfxChannel[5], true, false);
		setPosition(sfxChannel[5], 0, 0, 0);
		break;
	case 7:
		playSound(sfx[6], sfxChannel[6], true, false);
		setPosition(sfxChannel[6], 0, 0, 0);
		break;
	case 8:
		playSound(sfx[7], sfxChannel[7], true, false);
		setPosition(sfxChannel[7], 0, 0, 0);
		break;
	default:
		break;
	}
}

//sets channel groups to master channel group
void Sound::setChannels()
{
	result = System::getInstance()->system->createChannelGroup("Background", &backgroundGroup);
	FMODError(result);
	result = System::getInstance()->system->createChannelGroup("SFX", &sfxGroup);
	FMODError(result);
	result = System::getInstance()->system->getMasterChannelGroup(&masterGroup);
	FMODError(result);

	result = masterGroup->addGroup(backgroundGroup);
	FMODError(result);
	result = masterGroup->addGroup(sfxGroup);
	FMODError(result);
}

//set master volume
void Sound::setVol(float vol)
{
	masterGroup->setVolume(vol);
}

//set position of sound
void Sound::setPosition(FMOD::Channel *channel, float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
	result = channel->set3DAttributes(&pos, &vel);
}
void Sound::pauseBackground(int channel)
{
	switch (channel)
	{
	case 1:
		playSound(background[0], backgroundChannel[0], false, true);
		break;
	case 2:
		playSound(background[1], backgroundChannel[1], false, true);
		break;
	case 3:
		playSound(background[2], backgroundChannel[2], false, true);
		break;
	default:
		break;
	}
}