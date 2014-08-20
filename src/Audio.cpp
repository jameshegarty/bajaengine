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

Audio audio;

/*seagull=audio.load(gdfgdfg.wav)

seagull.volume=0
seagull.directional=true

seagull:play()*/


/*script:

audio.volume		//global volume; default=1
audio.load(string)	//returns handle (table)

handle:play();				//starts playing from start if initial, or from last pos if paused
handle:pause();				//stops playing, saves pos
handle:stop();				//stops playing, goes back to start
handle:rewind();			//goes back to start
handle:volume(n)			//default=1 (out of 1)
handle:volume()				//returns volume
handle:range(num)			//default=0 (no range=play everywhere)
handle:directional(bool)	//defaut=false
handle:loop(bool)			//default=true | if file is currently playing + loop is set to false, it'll stop when the file comes to the end
handle:pos(x,y,z)
handle.id		//handle id, internal


POSITIONAL SOUNDS MUST BE MONO!  OR ELSE THEY WON'T WORK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! LOL

http://www.devmaster.net/forums/index.php?showtopic=373&pid=2989&mode=threaded&show=&st=&
http://www.devmaster.net/articles/openal-tutorials/lesson8.php
http://www.gamedev.net/reference/articles/article2031.asp
*/

void alutUnloadWAV(ALenum format,ALvoid *data,ALsizei size,ALsizei freq)
{
	if (data)
		free(data);
}


bool alutInit(ALint *argc,ALbyte **argv) 
{
	ALenum errori=alGetError();
	alGetError();
	alGetError();
	alGetError();
	alGetError();

	ALCcontext *Context;
	ALCdevice *Device;
	
	errori=alGetError();
	if(errori != AL_NO_ERROR){
		return false;
	}

	errori=alGetError();
	if(errori != AL_NO_ERROR){
		return false;

	}


	Device=alcOpenDevice(NULL);

	Context=alcCreateContext(Device,NULL);
	
	//Set active context
	alcMakeContextCurrent(Context);

	char* t=(char*)alcGetString(Device,ALC_DEVICE_SPECIFIER);
	

	#ifdef _WIN32
	if(t==NULL){
		logs().audio.write("HARDWARESTRING:''");
		return false;
	}

	String lol;

	while(t[1]!=NULL){
		lol+=t;
		t+=lol.size()+1;
	}

	logs().audio.write("HARDWARESTRING:'"+lol+"'");

	if(lol=="" || lol==" "){
		//no AL hardware
		return false;
	}
	
	#else
		//os x doesn't give us a hardware string? so ignore
	#endif

	int majorVersion=0;
	int minorVersion=0;

	alcGetIntegerv(Device,ALC_MAJOR_VERSION,sizeof(majorVersion),&majorVersion);
	alcGetIntegerv(Device,ALC_MINOR_VERSION,sizeof(minorVersion),&minorVersion);

	String version=String(majorVersion)+"."+String(minorVersion);

	logs().audio.write("HARDWAREVERSION:'"+version+"'");

	if(version!="1.1"){
		logs().audio.write("Error, openal not version 1.1");

		//try to fallback on the software implementation

		alcDestroyContext(Context);
		alcCloseDevice(Device);

		logs().audio.write("searching for software dll");

		ALCdevice *DeviceSoftware;

		DeviceSoftware=alcOpenDevice("Generic Software");

		if(DeviceSoftware==NULL){
			logs().audio.write("device not found!");
			return false;
		}

		ALCcontext *ContextSoftware=alcCreateContext(DeviceSoftware,NULL);

		if(ContextSoftware==NULL){
			logs().audio.write("bad context!");
			return false;
		}

		//Set active context
		alcMakeContextCurrent(ContextSoftware);

		int majorVersionS=0;
		int minorVersionS=0;

		alcGetIntegerv(DeviceSoftware,ALC_MAJOR_VERSION,sizeof(majorVersionS),&majorVersionS);
		alcGetIntegerv(DeviceSoftware,ALC_MINOR_VERSION,sizeof(minorVersionS),&minorVersionS);

		String versionS=String(majorVersionS)+"."+String(minorVersionS);

		logs().audio.write("HARDWAREVERSION_SOFTWARE:'"+versionS+"'");

		
	

		#ifdef _WIN32
		char* t=(char*)alcGetString(DeviceSoftware,ALC_DEVICE_SPECIFIER);
		
		if(t==NULL){
			logs().audio.write("HARDWARESTRING_SOFTWARE:''");
			return false;
		}

		String lol;

		while(t[1]!=NULL){
			lol+=t;
			t+=lol.size()+1;
		}

		logs().audio.write("HARDWARESTRING_SOFTWARE:'"+lol+"'");

		if(lol=="" || lol==" "){
			//no AL hardware
			return false;
		}
		
		#else
			//os x doesn't give us a hardware string? so ignore
		#endif

		if(versionS!="1.1"){
			logs().audio.write("error, software version still too old");
			return false;
		}

		//w0000 $$$$$$$$$$$$
		return true;
	}

	errori=alGetError();
	if(errori != AL_NO_ERROR){
		return false;
	}

	t=(char*)alcGetString(Device,ALC_DEVICE_SPECIFIER);

	return true;
}


void checkAlError() {
	int error = alGetError();

	if(error != AL_NO_ERROR){
		//throw string("OpenAL error was raised.");
		console().write("ALERROR");
	}
}


Audio::~Audio(){
	if(active){deInit();}
}

Audio::Audio(){
	listenerPos[0]=0;
	listenerPos[1]=0;
	listenerPos[2]=0;
	volume=1;
	
	dopplerFactor=100;
}




AudioSource::~AudioSource(){
	//oggRelease();
}




float lol=1;


int luaLoadAudio(lua_State *L);
int luaListActive(lua_State *L);

///////////////////////////
bool Audio::scriptInit(){
	LuaType* luaa=script().add("audio");
	
	script().add("volume",&volume,luaa);		//audio.volume	//global volume; default=1
	script().add("load",&luaLoadAudio,luaa);					//audio.load(string) returns handle table
	script().add("listActive",luaListActive,luaa);
	return true;
}

bool Audio::init(){
	logs().audio.write("[Audio] init");

	if(!alutInit(NULL, 0)){
		hardware.audio=false;
		return false;
	}


	ALenum error=alGetError();

	if(error != AL_NO_ERROR){
		logs().audio.write("[AudioObject] init error '"+String(alGetString(error))+"' ");
		console().write(String("audio init error '"+String(alGetString(error))+"' "));
		return false;
	}

	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);	//NONE for none, AL_INVERSE_DISTANCE for other + //AL_INVERSE_DISTANCE_CLAMPED
	alDopplerFactor ( dopplerFactor );
	
	error=alGetError();
	
	active=true;

	//handle:play();
	//handle:volume			//default=1 (out of 1)
	//handle:range			//default=0 (no range=play everywhere)
	//handle:directional	//defaut=false
	//handle:loop			//default=true
	//handle:id		//handle id, internal

	error=alGetError();

	if(error != AL_NO_ERROR){
		return false;
	}

	return true;
}	

int Audio::loadWav(Path file){
	engineTime.skipNextFrame();
	AudioSource* source=new AudioSource;

	source->type=AUDIO_WAV;
	source->filename=file;

	ALenum format;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean l;  //discarded
	
	// Load wav data into a buffer.

	alGetError();
	alGetError();
	alGetError();
	alGetError();
	alGetError();
	alGetError();

	alGenBuffers(1, &source->buffer);

	ALenum error=alGetError();

	if(error != AL_NO_ERROR){
		logs().audio.write("[AudioObject] Error loading file:"+file.getRelative()+", "+String(alGetString(error)));
		console().write("audio error: could not load file '"+file.getRelative()+"', "+String(alGetString(error)));
		return -1;
	}

	
	error=alGetError();
	

		alutLoadWAVFile((ALbyte*)file.getAbsolute().c_str(), &format, &data, &size, &freq, &l);

	
	if(format==AL_FORMAT_STEREO8 || format==AL_FORMAT_STEREO16){
		source->stereo=true;
	}else{
		source->stereo=false;
	}


	alBufferData(source->buffer, format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);

	// Bind buffer with a source.

	error=alGetError();


	if(error != AL_NO_ERROR){
		logs().audio.write("[AudioObject] Error loading file:"+file.getRelative()+", "+String(alGetString(error)));
		console().write("audio error: could not loading file '"+file.getRelative()+"', "+String(alGetString(error)));
		return -1;
	}

	alGenSources(1, &source->source);

	
	
	if(error==AL_OUT_OF_MEMORY || error==AL_INVALID_VALUE){
		//there's not more resources for sources
		source->alSourceSet=false;
	}else if(error!=AL_NO_ERROR){
		return -1;
	}else{
		source->alSourceSet=true;
		alSources.pushBack(source->source);
		source->sourceIndex=sources.size()-1;
		alSourcePriority.pushBack(0);

		while(alSourceUsedBy.size()<=sources.size()){	//note, we haven't actually added this source to the sources list yet, thus the weird index
			alSourceUsedBy.pushBack();
		}
		alSourceUsedBy[alSources.size()-1]=sources.size();

		source->alSourceIndex=alSources.size()-1;
	}
	
	source->sourcePos[0]=source->pos.x;
	source->sourcePos[1]=source->pos.y;
	source->sourcePos[2]=source->pos.z;

	source->sourceVelocity[0]=source->velocity.x;
	source->sourceVelocity[1]=source->velocity.y;
	source->sourceVelocity[2]=source->velocity.z;
	
	
	if(source->alSourceSet){
		alSourcei (source->source, AL_BUFFER,   source->buffer   );
		alSourcef (source->source, AL_PITCH,    1.0f     );
		alSourcef (source->source, AL_GAIN,     1.0f     );

		if(!source->stream){
			alSourcei (source->source, AL_LOOPING,  source->loop     );
		}
	}
	
	
	// Do another error check and return.

	error=alGetError();

	if(error == AL_NO_ERROR){
		sources.pushBack(source);
		source->sourceIndex=sources.size()-1;
		return sources.size()-1;
	}else{

		logs().audio.write("[AudioObject] Error 2 loading file:"+file.getRelative());
		console().write(String("audio wav error: error '")+String(alGetString(error))+"' loading file '"+file.getRelative()+"'");
		return -1;
	}

}	

String errorString(int code)
{
    switch(code)
    {
        case OV_EREAD:
            return String("Read from media.");
        case OV_ENOTVORBIS:
            return String("Not Vorbis data.");
        case OV_EVERSION:
            return String("Vorbis version mismatch.");
        case OV_EBADHEADER:
            return String("Invalid Vorbis header.");
        case OV_EFAULT:
            return String("Internal logic fault (bug or heap/stack corruption.");
        default:
            return String("Unknown Ogg error.");
    }
}

#ifdef FEATURE_OGG_STREAM
int Audio::loadOggStream(Path path){

	int result;
    
	AudioSource* source=new AudioSource;

	source->type=AUDIO_OGG;
	source->filename=path;
	source->stream=true;

	ALenum error=alGetError();

	if(!(source->oggFile = fopen(path.getAbsolute().c_str(), "rb"))){
		console().write("Could not open Ogg file.");
		return -1;
	}

    if((result = ov_open(source->oggFile, &source->oggStream, NULL, 0)) < 0)
    {
        fclose(source->oggFile);
        
		console().write("Could not open Ogg stream. " + errorString(result));
		return -1;
    }

    source->vorbisInfo = ov_info(&source->oggStream, -1);
    source->vorbisComment = ov_comment(&source->oggStream, -1);

	source->freq = source->vorbisInfo->rate;

	if(source->vorbisInfo->channels == 1){
        source->format = AL_FORMAT_MONO16;
		source->stereo=false;
	}else{
        source->format = AL_FORMAT_STEREO16;
		source->stereo=true;
	}

	source->lengthSeconds=ov_time_total(&source->oggStream,-1);

	error=alGetError();
        
    alGenBuffers(2, source->buffers);
    alGenSources(1, &source->source);

	error=alGetError();
	
	if(error==AL_OUT_OF_MEMORY || error==AL_INVALID_VALUE){
		//there's not more resources for sources

		source->alSourceSet=false;
	}else if(error!=AL_NO_ERROR){
		return -1;
	}else{
		source->alSourceSet=true;
		alSources.pushBack(source->source);
		source->sourceIndex=sources.size()-1;
		alSourcePriority.pushBack(0);

		while(alSourceUsedBy.size()<=sources.size()){	//note, we haven't actually added this source to the sources list yet, thus the weird index
			alSourceUsedBy.pushBack();
		}
		alSourceUsedBy[alSources.size()-1]=sources.size();

		source->alSourceIndex=alSources.size()-1;
	}

	error=alGetError();

	if(error == AL_NO_ERROR){
		sources.pushBack(source);
		source->sourceIndex=sources.size()-1;
		return sources.size()-1;
	}else{
		logs().audio.write("[AudioObject] Error 2 loading file:"+source->filename.getRelative());
		console().write(String("audio wav error: error '")+String(alGetString(error))+"' loading file '"+source->filename.getRelative()+"'");
		return -1;
	}
}
#endif

#ifdef FEATURE_OGG
int Audio::loadOgg(Path path){

	engineTime.skipNextFrame();

	ALenum errori=alGetError();

	AudioSource* source=new AudioSource;

	source->filename=path;
	source->type=AUDIO_OGG;

	int result;

	if(!(source->oggFile = fopen(path.getAbsolute().c_str(), "rb"))){
		console().write("audio error: i/o error, could not open off file '"+path.getRelative()+"'");
		return -1;
	}

		if((result = ov_open(source->oggFile, &source->oggStream, NULL, 0)) < 0){
			fclose(source->oggFile);
	        
			console().write("audio error: could not open ogg stream '"+path.getRelative()+"'");
			return -1;
		}


    source->vorbisInfo = ov_info(&source->oggStream, -1);
    source->vorbisComment = ov_comment(&source->oggStream, -1);

	if(source->vorbisInfo->channels == 1){
        source->format = AL_FORMAT_MONO16;
		source->stereo=false;
	}else{
        source->format = AL_FORMAT_STEREO16;
		source->stereo=true;
	}   
    
	source->freq = source->vorbisInfo->rate;


	alGenBuffers(1, &source->buffer);
	

	errori=alGetError();

	if(errori != AL_NO_ERROR){
		logs().audio.write("[AudioObject] error '"+String(alGetString(errori))+"' loading file '"+path.getRelative()+"'");
		console().write(String("audio ogg error: error '"+String(alGetString(errori))+"' loading file '"+path.getRelative()+"'"));
		return -1;
	}

    alGenSources(1, &source->source);
	
	

	errori=alGetError();

	if(errori==AL_OUT_OF_MEMORY || errori==AL_INVALID_VALUE){
		//there's not more resources for sources

		source->alSourceSet=false;
	}else if(errori!=AL_NO_ERROR){
		return -1;
	}else{
		source->alSourceSet=true;
		alSources.pushBack(source->source);
		source->sourceIndex=sources.size()-1;
		alSourcePriority.pushBack(0);

		while(alSourceUsedBy.size()<=sources.size()){	//note, we haven't actually added this source to the sources list yet, thus the weird index
			alSourceUsedBy.pushBack();
		}
		alSourceUsedBy[alSources.size()-1]=sources.size();

		source->alSourceIndex=alSources.size()-1;
	}

	

	if(!source->stream){
		std::vector < char > bufferData;
		#ifdef _WIN32
		int endian = 0;             // 0 for Little-Endian, 1 for Big-Endian
		#else
			#ifdef __LITTLE_ENDIAN__
				int endian=0;
			#else
				int endian=1;
			#endif
		#endif
		int bitStream;
		long bytes;
		char arr[BUFFER_SIZE];    // Local fixed size array

		 do {
			// Read up to a buffer's worth of decoded sound data
			bytes = ov_read(&source->oggStream, arr, BUFFER_SIZE, endian, 2, 1, &bitStream);
			// Append to end of buffer
			bufferData.insert(bufferData.end(), arr, arr + bytes);
		} while (bytes > 0);

		alBufferData(source->buffer, source->format, &bufferData[0], static_cast < ALsizei > (bufferData.size()), source->freq);

		if(source->alSourceSet){
			alSourcei (source->source,AL_BUFFER,   source->buffer   );
		}
	}

	ALenum error=alGetError();

	if(error == AL_NO_ERROR){
		sources.pushBack(source);
		source->sourceIndex=sources.size()-1;
		return sources.size()-1;
	}else{
		logs().audio.write("[AudioObject] error 1 '"+String(alGetString(error))+"' loading file '"+path.getRelative()+"'");
		console().write(String("audio ogg error: error 1 '"+String(alGetString(error))+"' loading file '"+path.getRelative()+"'"));
		return -1;
	}
}
#endif

int Audio::loadFakeWav(Path file){
	AudioSource* source=new AudioSource;

	source->filename=file;
	source->type=AUDIO_WAV;

	if(!os().fileExists(file)){
		console().write("audio error: i/o error, could not open off file '"+file.getRelative()+"'");
		return -1;
	}

	source->fakeLen=wavLen((ALbyte*)file.getAbsolute().c_str());

	sources.pushBack(source);
	source->sourceIndex=sources.size()-1;
	return sources.size()-1;
}

#ifdef FEATURE_OGG
int Audio::loadFakeOgg(Path file){
	
	AudioSource* source=new AudioSource;

	source->filename=file;
	source->type=AUDIO_OGG;

	if(!(source->oggFile = fopen(file.getAbsolute().c_str(), "rb"))){
		console().write("audio error: i/o error, could not open off file '"+file.getRelative()+"'");
		return -1;
	}

	int result;

	if((result = ov_open(source->oggFile, &source->oggStream, NULL, 0)) < 0){
		fclose(source->oggFile);
        
		console().write("audio error: could not open ogg stream '"+file.getRelative()+"'");
		return -1;
	}

	source->fakeLen=ov_time_total(&source->oggStream,-1);

	sources.pushBack(source);
	source->sourceIndex=sources.size()-1;
	return sources.size()-1;

}
#endif

int Audio::load(Path file){
	// Variables to load into.

	if(!os().fileExists(file)){
		logs().audio.write("[AudioObject] Error, file '"+file.getRelative()+"' doesn't exist");
		console().write("audio error: file '"+file.getRelative()+"' doesn't exist");

		return -1;
	}

	for(int i=0; i<sources.size(); i++){
		if(sources[i]->filename==file){
			console().write("NOTE: the file "+file.getRelative()+" was already loaded as another audio object.  This is not an error, but may not be what was intended");
		}
	}

	String filetype=file.getExtension();

	if(filetype=="wav"){
		if(hardware.audio){
			return loadWav(file);
		}else{
			return loadFakeWav(file);
		}
	}else if(filetype=="ogg"){

		#ifdef FEATURE_OGG
			if(hardware.audio){
				#ifdef OGG_STREAM
					return loadOggStream(file);
				#else
					return loadOgg(file);
				#endif
			}else{
				return loadFakeOgg(file);
			}
		#else
			console().write("audio error: this version of the engine can't open oggs");
			return -1;
		#endif

	}else{
		logs().audio.write("[AudioObject] error: unknown filetype '"+filetype+"' for file '"+file.getRelative()+"'");
		console().write("audio error: unknown filetype '"+filetype+"' for file '"+file.getRelative()+"'");

		return -1;

	}


	
	
}

bool Audio::deInit(){
	active=false;
	return true;
}



void SetListenerOrientation(float fx, float fy, float fz, float ux, float uy, float uz)
{
	//set the orientation using an array of floats
	float vec[6];
	vec[0] = fx;
	vec[1] = fy;
	vec[2] = fz;
	vec[3] = ux;
	vec[4] = uy;
	vec[5] = uz;
	alListenerfv(AL_ORIENTATION, vec);

}

void Audio::perFrame(){

	if(!active){	//update the fake audio objects
		for(int i=0; i<sources.size(); i++){
			sources[i]->perFrame();
		}
	}

	if(active){

		for(int i=0; i<sources.size(); i++){
			sources[i]->perFrame();
		}

		alListener3f(AL_POSITION,    level->camera->pos.x,level->camera->pos.y,level->camera->pos.z);
		alListener3f(AL_VELOCITY,    level->camera->velocity.x,level->camera->velocity.y,level->camera->velocity.z);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		level->camera->transform();

		double mMatrix[16];
		glGetDoublev(GL_MODELVIEW_MATRIX,mMatrix);

		SetListenerOrientation(-mMatrix[2],-mMatrix[6],-mMatrix[10],mMatrix[1],mMatrix[5],mMatrix[9]);
			//set listener orientation, first 3 numbers are for the forward vector, last three are for the up vecto

		if(volume<0){volume=0;}
		if(volume>1){volume=1;}

		
		alListenerf(AL_GAIN, volume);
	}
}

