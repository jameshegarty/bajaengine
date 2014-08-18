#ifndef AUDIO_H
#define AUDIO_H

#include <al.h>

#include "os.h"
#include "HelperLibString.hpp"
#include "Containers.hpp"

extern "C"{
#include <vorbis/vorbisfile.h>
}

/*
VOLUMES go from 0-1 NOT 0-100!!!!!!!!!!!!!
*/

//#define BUFFER_SIZE (4096 * 8)	//32k
//#define BUFFER_SIZE (4096 * 16)	//64k
#define BUFFER_SIZE (4096 * 32)	//128k
#define BUFFER_COUNT 2

#ifdef FEATURE_OGG_STREAM
	#define OGG_STREAM 1
#endif

class Audio;

enum AudioType{
	AUDIO_WAV,
	AUDIO_OGG
};

class AudioSource{
	friend class Audio;

	protected:
		float fakeLen;	//length if the fake file, if there is no audio hardware. 0 if there is audio hardware + this is not a fake source
		float fakePos;
		bool hasPlayed;

		

		float timeHistory[BUFFER_COUNT+1]; //index BUFFER_COUNT contains the most recent reading, index 1 contains the current, time 0 is the segment that was just discarded

		ALuint buffer;
		ALuint buffers[2];
		ALuint source;
		bool alSourceSet;	//false if we need to assign it a new alSource
		int alSourceIndex;	//index in array of all alSources
		int sourceIndex;	//index in audio.sources
		void acquireAlSource();
		ALfloat sourcePos[3];
		ALfloat sourceVelocity[3];
		ALenum format;
		ALsizei freq; 

		FILE*           oggFile;
		OggVorbis_File  oggStream;
		 vorbis_info*    vorbisInfo;
        vorbis_comment* vorbisComment;

		int doStream(ALuint buffer);
		bool update();
		void empty();
		bool rewindStream;
		float offsetStream;	//-1 if not active
		bool loopEnd;
		bool endPos;
		
		float fadeoutStop;
		float fadeoutPause;
		float fadeinPlay;
		
		float lastGottenOffset;
		bool oneBufferHack;
		bool playHack;
		bool offsetSlipHack;
		float offsetSlipBuffer;
		float lastOffset;
	
	public:	
		float lengthSeconds;
		float fadeVolumeTarget;	//-1 if no volume fade occuring
		float fadeVolumeCurrent;
		Path filename;

		AudioType type;

		void play();
		void stop();
		void pause();
		void rewind();

		void setOffset(float);
		float getOffset();

		bool stereo;	//is the current loaded file stereo?

		bool loop;
		bool playing;
		bool paused;
		bool stopped;
		bool stream;	//stream from disk, don't load into memory

		FloatVector3d pos;
		FloatVector3d velocity;
		float fade;	//in seconds, 0 for not fade
		float volume;	//0-1
		float pitch;
		bool directional;
		float range;		//0=norange
		void perFrame();
		
		String getState();

		AudioSource();
		~AudioSource();
};

class Audio{
	friend class AudioSource;

	private:
		bool active;  //is audio running ok? set to false if there was a major error

	protected:
		ALfloat listenerPos[3];
		ALfloat listenerVel[3];
		int loadFakeWav(Path file);
		int loadFakeOgg(Path file);
		int loadWav(Path file);
		int loadOgg(Path file);
		int loadOggStream(Path file);

	public:
		Array<AudioSource*> sources;
		Array<ALuint> alSources;
		Array<float> alSourcePriority;	//0==unused, bigger values are more important
		Array<int> alSourceUsedBy;	//the engine source (AudioSource class) that's using the alSource

		int load(Path file);	//returns id, 0 if failed

		float volume;  //overall volume 0 to 1
		float dopplerFactor;
		
		bool init();
		bool deInit();
		
		ALuint getSource();
		ALuint getBuffer();
		
		void perFrame();
		
		bool scriptInit();


		Audio();
		~Audio();
};

void alutLoadWAVFile(ALbyte *file,ALenum *format,ALvoid **data,ALsizei *size,ALsizei *freq, ALboolean *loop);
float wavLen(ALbyte *file);

extern Audio audio;

#endif
