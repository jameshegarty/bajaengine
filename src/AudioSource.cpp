#include "../common.h"
#include "../engine.h"
#include "../Helperlib/HelperLibMath.hpp"
#include "../Helperlib/Log.hpp"
#include "../level.hpp"
#include "Audio.h"
#include "../timeing.hpp"
#include "../hardware.hpp"

#include <al.h>
#include <alc.h>

#include "../script.hpp"

#include "../os.h"

#include "../GlHeader.hpp"


void AudioSource::pause(){

	if(fakeLen>0){
		playing=false;
		return;
	}

	if(audio.alSourcePriority.size()>alSourceIndex){
		audio.alSourcePriority[alSourceIndex]=0;
	}

	if(fade>0){
		if(fadeinPlay>0){
			fadeoutPause=fadeinPlay;
			fadeinPlay=0;
			fadeoutStop=0;
		}else if(fadeoutStop>0){
			fadeoutPause=fadeoutStop;
			fadeinPlay=0;
			fadeoutStop=0;
		}else{
			if(fadeoutPause<=0){
				fadeoutPause=1;
			}
			fadeinPlay=0;
			fadeoutStop=0;
		}
	}else{
		playing=false;
		paused=true;
		stopped=false;
		
		rewindStream=false;

		//make it so what it starts back at the same pos when you resume from pause
		float op=getOffset();
		ov_time_seek(&oggStream,op);

		alSourcePause(source);
	}
}


AudioSource::AudioSource(){
	playing=false;
	paused=false;
	loop=true;
	directional=false;
	range=0;
	pitch=1;
	hasPlayed=false;
	stream=false;


	volume=1;
	fade=0;
	fadeinPlay=0;
	fadeoutPause=0;
	fadeoutStop=0;
	fadeVolumeTarget=-1;
	fadeVolumeCurrent=-1;

	stereo=false;

	rewindStream=false;
	offsetStream=-1;
	
	alSourceSet=false;
	alSourceIndex=0;

	loopEnd=false;

	for(int i=0; i<=BUFFER_COUNT; i++){
		timeHistory[i]=0;
	}

	oneBufferHack=false;
	fakeLen=0;
	fakePos=0;

	endPos=false;

	offsetSlipHack=false;
	offsetSlipBuffer=-1;

	lastGottenOffset=-1;
	playHack=false;

	lengthSeconds=0;
}


void AudioSource::perFrame(){

	if(fakeLen>0){
	
		if(playing){
			fakePos=fakePos+(float(engineTime.time)/float(engineTime.idealFps));

			if(fakePos>fakeLen){
				if(!loop){playing=false;}

				fakePos=0;
			}
		}

		return;
	}

	if(!alSourceSet){
		return;
	}

	if(playHack){
		alSourcePlay(source);
		playHack=false;
	}

	if(stream && playing){
		update();
	}else if(stream&& stopped){

		alSourceStop(source);
		alSourcei(source,AL_BUFFER,0);
	}else if(stream && paused){

		alSourceStop(source);
		alSourcei(source,AL_BUFFER,0);
	}

	if(!playing){
		return;
	}

	float atten=1;

	if(directional && range>0){
		alSource3f(source, AL_POSITION, pos.x,pos.y,pos.z);
		alSource3f(source, AL_VELOCITY, 0,0,0);
		alSourcef (source, AL_SOURCE_RELATIVE,  false     );
		
		alSourcef (source, AL_ROLLOFF_FACTOR,  1.0       );

		alSourcef (source, AL_REFERENCE_DISTANCE,  0);
		alSourcef (source, AL_MAX_DISTANCE,  range     );

		FloatVector3d d=level->camera->pos-pos;
		float tempatten=1-((d.magnitude())/range);

		if(tempatten<0){
			if(audio.alSourcePriority.size()>alSourceIndex){
				audio.alSourcePriority[alSourceIndex]=0;
			}
		}else{
			acquireAlSource();
			audio.alSourcePriority[alSourceIndex]=1;
		}

	}else if(range>0){
		FloatVector3d d=level->camera->pos-pos;

		atten=1-((d.magnitude())/range);

		if(atten<0){
			if(audio.alSourcePriority.size()>alSourceIndex){
				audio.alSourcePriority[alSourceIndex]=0;
			}

			atten=0;
		}else{
			acquireAlSource();
			audio.alSourcePriority[alSourceIndex]=1;
		}


		if(atten>1){

			atten=1;

			
		}
		alSource3f(source, AL_VELOCITY, 0,0,0);
		alSource3f(source, AL_POSITION, level->camera->pos.x,level->camera->pos.y,level->camera->pos.z);
		alSourcef (source, AL_SOURCE_RELATIVE,  false     );

		
	}else{
		//to make mono sounds full volume
		atten=1;

		alSource3f(source, AL_VELOCITY, 0,0,0);
		alSource3f(source, AL_POSITION, level->camera->pos.x,level->camera->pos.y,level->camera->pos.z);
		alSourcef (source, AL_SOURCE_RELATIVE,  false     );
	}

	alSourcef (source, AL_PITCH,    pitch    );

	if(!stream){
		alSourcei (source, AL_LOOPING,  loop     );
	}

	if(volume<0){volume=0;}
	if(volume>1){volume=1;}

	if(fadeoutPause>0){
		alSourcef(source,AL_GAIN, fadeoutPause*volume*atten);

		fadeoutPause-=(float(engineTime.time)/float(engineTime.idealFps))*(float(1)/float(fade));

		if(fadeoutPause<0){
			playing=false;
			paused=true;
			stopped=false;

			fadeoutPause=0;

			//make it so what it starts back at the same pos when you resume from pause
			float op=getOffset();
			ov_time_seek(&oggStream,op);

			alSourcePause(source);
		}

	}else if(fadeoutStop>0){
		alSourcef(source,AL_GAIN, fadeoutStop*volume*atten);

		fadeoutStop-=(float(engineTime.time)/float(engineTime.idealFps))*(float(1)/float(fade));

		if(fadeoutStop<0){
			playing=false;
			paused=false;
			stopped=true;

			fadeoutStop=0;
			rewindStream=true;
			alSourceStop(source);

			if(audio.alSourcePriority.size()>alSourceIndex){
				audio.alSourcePriority[alSourceIndex]=0;
			}
		}

	}else if(fadeinPlay>0){
		alSourcef(source,AL_GAIN, (1-fadeinPlay)*volume*atten);

		fadeinPlay-=(float(engineTime.time)/float(engineTime.idealFps))*(float(1)/float(fade));

		if(fadeinPlay<=0){
			fadeinPlay=0;
		}
	}else if(fadeVolumeTarget!=-1){

		alSourcef(source,AL_GAIN, fadeVolumeCurrent*atten);

		if(fadeVolumeCurrent<fadeVolumeTarget){
			fadeVolumeCurrent+=(float(engineTime.time)/float(engineTime.idealFps))*(float(1)/float(fade));

			if(fadeVolumeCurrent>=fadeVolumeTarget){
				fadeVolumeCurrent=-1;
				fadeVolumeTarget=-1;
			}

		}else if(fadeVolumeCurrent>fadeVolumeTarget){
			fadeVolumeCurrent-=(float(engineTime.time)/float(engineTime.idealFps))*(float(1)/float(fade));

			if(fadeVolumeCurrent<=fadeVolumeTarget){
				fadeVolumeCurrent=-1;
				fadeVolumeTarget=-1;
			}
		}

		if(fadeVolumeCurrent==fadeVolumeTarget){
			fadeVolumeTarget=-1;
			fadeVolumeCurrent=-1;
		}

	}else{
		alSourcef (source, AL_GAIN,    volume*atten    );
	}

}


void AudioSource::acquireAlSource(){
	
	if(!alSourceSet){

		ALenum error=alGetError();

		float lowestV=100000000000.0f;
		int lowestIndex=-1;

		for(int i=0; i<audio.alSourcePriority.size(); i++){
			if(audio.alSourcePriority[i]<lowestV){
				lowestV=audio.alSourcePriority[i];
				lowestIndex=i;
			}
		}

		if(lowestIndex==-1){
			console().write("ERROR: out of AL sources!");
			return;
		}

		if(lowestV==1){
			console().write("ERROR: out of AL sources!");
		}

		source=audio.alSources[lowestIndex];

		

		if(!stream){
			alSourcei (source,AL_BUFFER,   buffer   );
			alSourcei (source, AL_LOOPING,  loop     );
		}

		audio.alSourcePriority[lowestIndex]=1;
		alSourceIndex=lowestIndex;

		audio.sources[audio.alSourceUsedBy[lowestIndex]]->alSourceSet=false;
		
		audio.alSourceUsedBy[lowestIndex]=sourceIndex;

		alSourceSet=true;

		error=alGetError();

		if(error != AL_NO_ERROR){
			console().write("ALerror while acquireing source");
		}

		if(stream){
			alSourceStop(source);

			alSourcei(source,AL_BUFFER,0);
		}
	}
}


int AudioSource::doStream(ALuint buffer){

    char pcm[BUFFER_SIZE];
    int  size = 0;
    int  section;
    int  result;
	

	if(rewindStream){
		ov_raw_seek(&oggStream,0);
		rewindStream=false;

		timeHistory[2]=offsetStream;
	}

	if(offsetStream>=0){
		ov_time_seek(&oggStream,offsetStream);
		timeHistory[2]=offsetStream;
		offsetStream=-1;

	}


	float buffReadTime=ov_time_tell(&oggStream);

    while(size < BUFFER_SIZE)
    {
		#ifdef _WIN32
		int endian=0;
		#else
			#ifdef __LITTLE_ENDIAN__
				int endian=0;
			#else
				int endian=1;
			#endif
		#endif
        result = ov_read(&oggStream, pcm + size, BUFFER_SIZE - size, endian, 2, 1, &section);
    
		if(result > 0){
            size += result;
		}else{
			if(result < 0){
				return -1;
			}else if(result == 0){	//we're out of data
				if(loop){
					ov_raw_seek(&oggStream,0);	
				}else{
					break;
				}
			}
		}
    }
    
	buffReadTime=ov_time_tell(&oggStream);


	timeHistory[0]=timeHistory[1]; //prev
	timeHistory[1]=timeHistory[2]; //current, due to buffering
	timeHistory[2]=buffReadTime;   //next sample to be played

	if(size == 0){
        return 0;
	}
        
    alBufferData(buffer, format, pcm, size, vorbisInfo->rate);

    return 1;
}

void AudioSource::empty(){
	ALenum error=alGetError();
    int queued=0;
    
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
    
    while(queued--){
        ALuint buffer;
    
        alSourceUnqueueBuffers(source, 1, &buffer);

		error=alGetError();

		if(error != AL_NO_ERROR){
			console().write(String("audio error ("+filename.getRelative()+"): '"+String(alGetString(error))+"'"));
		}
    }

	queued=0;
    
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
}

bool AudioSource::update(){
	int l=42;
	
	alGetSourcei(source,AL_LOOPING,&l);

	alSourcei (source, AL_LOOPING,  false     );

    int processed=0;
    bool active = true;

	int queued=0;
    
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

	int state=0;


	alGetSourcei(source, AL_SOURCE_STATE, &state);


	if( ((playing && state==AL_STOPPED)  || (playing && !paused && state==AL_PAUSED)) && !loopEnd ){
		//we must have run out of buffers
		alSourcePlay(source);

		//make it so that it doesn't skip back to the start when we drop frames
		alSourcef(source,AL_SEC_OFFSET,lastOffset);

		offsetSlipHack=true;

	}else{

		alGetSourcef(source,AL_SEC_OFFSET,&lastOffset);
	}

	if(rewindStream && state==AL_INITIAL && !loopEnd){

		alSourcePlay(source);
	}

	if(paused || stopped){
		alSourceStop(source);
		alSourcei(source,AL_BUFFER,AL_NONE);
	}


	
	if(rewindStream && state==AL_PLAYING){
		//clear out the buffers + start over to prevent latency
		
		alSourceStop(source);

		alSourcei(source,AL_BUFFER,0);


		for(int i=0; i<=BUFFER_COUNT; i++){
			timeHistory[i]=0;
		}


		doStream(buffers[0]);
		doStream(buffers[1]);

		alSourceQueueBuffers(source, 2, buffers);

		alSourcePlay(source);
	}

	if(offsetStream>=0 && state==AL_PLAYING){
		//clear out the buffers + start over to prevent latency

		alSourceStop(source);

		alSourcei(source,AL_BUFFER,0);

		for(int i=0; i<=BUFFER_COUNT; i++){
			timeHistory[i]=0;
		}

		doStream(buffers[0]);
		doStream(buffers[1]);

		alSourceQueueBuffers(source, 2, buffers);

		alSourcePlay(source);
	}

    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);


	if(oneBufferHack && state==AL_INITIAL){
		alSourcePlay(source);
	}

	if(oneBufferHack && state==AL_STOPPED){
		oneBufferHack=false;
		playing=false;
		stopped=true;
		paused=false;
		endPos=true;

		alSourceStop(source);
		alSourcei(source,AL_BUFFER,AL_NONE);	
	}

	if(queued==1 && loopEnd && state==AL_STOPPED){

		playing=false;
		stopped=true;
		paused=false;
		endPos=true;

		alSourceStop(source);
		alSourcei(source,AL_BUFFER,AL_NONE);	
	}


    while(processed--){
        ALuint bufferR;
        
		alSourceUnqueueBuffers(source, 1, &bufferR);
	
        active = doStream(bufferR);

		if(active==1){
			alSourceQueueBuffers(source, 1, &bufferR);
		}else{
			loopEnd=true;
			audio.alSourcePriority[alSourceIndex]=0;
		}
    }

    return active;
}


void AudioSource::play(){
	
	

	if(fakeLen>0){
		playing=true;
		return;
	}

	if(playing){
		return;
	}

	if(endPos){
		return;
	}

	if(audio.alSourcePriority.size()>alSourceIndex){
		acquireAlSource();
		audio.alSourcePriority[alSourceIndex]=1;
	}

	playing=true;
	paused=false;
	stopped=false;

	loopEnd=false;

 	if(type==AUDIO_OGG){
		
		if(stream){

			lastGottenOffset=-1;

			for(int i=0; i<=BUFFER_COUNT; i++){
				timeHistory[i]=0;
			}

			int queued=0;
			alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

			if(!doStream(buffers[0])){
				return;
			}

			bool btwo=true;
			if(!doStream(buffers[1])){
				btwo=false;
				oneBufferHack=true;
			}

			if(btwo){
				alSourceQueueBuffers(source, 2, buffers);
			}else{
				alSourceQueueBuffers(source, 1, buffers);
				loopEnd=true;
			}

			playHack=true;
			alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
		}else{
			alSourcePlay(source);
		}

	}else{
		alSourcePlay(source);
	}

	if(fade>0){
		alListenerf(AL_GAIN, 0.0f);

		if(fadeoutPause>0){
			fadeinPlay=fadeoutPause;
			fadeoutPause=0;
			fadeoutStop=0;
		}else if(fadeoutStop>0){
			fadeinPlay=fadeoutStop;
			fadeoutPause=0;
			fadeoutStop=0;
		}else{
			if(fadeinPlay<=0){
				fadeinPlay=1;
			}

			fadeoutPause=0;
			fadeoutStop=0;
		}
	}else{
		alListenerf(AL_GAIN, 0.0f);
	}

	hasPlayed=true;


}

String AudioSource::getState(){
	if(playing){
		return "playing";
	}else if(stopped){
		return "stopped";
	}else if(paused){
		return "paused";
	}

	return "";
}


float AudioSource::getOffset(){

	if(fakeLen>0){
		return fakePos;
	}

	ALenum error=alGetError();

	acquireAlSource();

	float t=0;
	float secoffest=0;

	if(stream && type==AUDIO_OGG){

		if(!playing && !paused){return 0;}


		alGetSourcef(source,AL_SEC_OFFSET,&secoffest);

		/*really bad hack here - we need to keep the numbers increasing + constant;
		OAL goes funky w/ the numbers if the game drops frames and runs out of loaded buffers
		so when it drops buffers, we store a baseline (lastOffset) and don't let it go below that*/


		if(secoffest<lastOffset && offsetSlipHack){
			if(offsetSlipBuffer!=-1 && secoffest<offsetSlipBuffer){
				offsetSlipHack=false;
			}else{
				secoffest=lastOffset;
			}

			offsetSlipBuffer=secoffest;
		}else{
			lastOffset=secoffest;
			offsetSlipHack=false;
			offsetSlipBuffer=-1;
		}

		t=timeHistory[0]+secoffest;
		
		if(lastGottenOffset!=-1 && t<lastGottenOffset && playing){
			// Audio offset dropped
			t=lastGottenOffset;
		}

		lastGottenOffset=t;

		return t;
	}else{
		alGetSourcef(source,AL_SEC_OFFSET,&t);

		error=alGetError();

		if(error != AL_NO_ERROR){
			console().write(String("audio offset error ("+filename.getRelative()+"): '"+String(alGetString(error))+"'"));
		}

		return t;
	}
}

void AudioSource::setOffset(float t){

	if(fakeLen>0){
		fakePos=t;
	}

	if(stream){
		offsetStream=t;
		lastGottenOffset=t;
	}else{
		alSourcef(source,AL_SEC_OFFSET,t);
	}
	
}


void AudioSource::rewind(){
	if(fakeLen>0){
		fakePos=0;
	}

	if(getOffset()==0){
		playing=false;
	}

	endPos=false;

	rewindStream=true;
	lastGottenOffset=0;
	alSourceRewind(source);
}

void AudioSource::stop(){
	
	if(fakeLen>0){
		playing=false;
		fakePos=0;
		return;
	}

	if(stopped){
		return;
	}
	
	if(fade>0){
		if(fadeoutStop<=0){
			fadeoutStop=1;
		}
	}else{

		if(audio.alSourcePriority.size()>alSourceIndex){
			audio.alSourcePriority[alSourceIndex]=0;
		}

		playing=false;
		paused=false;
		stopped=true;

		rewindStream=true;
		alSourceStop(source);
	}
}

