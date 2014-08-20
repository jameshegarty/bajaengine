#ifndef AUDIO_OBJECT_H
#define AUDIO_OBJECT_H

//like lights, size.x will hold range

#include "../Object.h"

#include <AL/al.h>

class AudioObject : public Object{
	friend class Level;
	
	
	private:	
		bool load(std::string file);
		std::string pastFile;
		ALuint buffer;
		ALuint source;
		bool playing;
		bool paused;
		ALfloat sourcePos[3];
		ALfloat sourceVel[3];
		bool tempLoop;
		
	public:	
		
		
		
		
		
		std::string file;  //changeing this will cause a reload
		bool loop;
		float volume; //0 to 1
		
		bool play();
		bool stop();
		bool pause();
		
		void draw3dZ();
		
		AudioObject();
		~AudioObject();
};

#endif
