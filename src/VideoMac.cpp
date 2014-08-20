#include "video.h"
#include "log.hpp"

#ifdef _WIN32
#include "../NeHeGl.h"
#endif

#include "VideoCapture.h"



VideoSource* currentVideoSource;
    

Video video;

#define MAX_VFW_DEVICES		10

bool Video::listCaptureSources(){
	return true;
}

int makePowerOf2(int i){
    if(i==0){
    	return 0;    
    }else if(i>0 && i<=2){
    	return 2;    
    }else if(i>2 && i<=4){
    	return 4;    
    }else if(i>4 && i<=8){
    	return 8;    
    }else if(i>8 && i<=16){
    	return 16;    
    }else if(i>16 && i<=32){
    	return 32;    
    }else if(i>32 && i<=64){
    	return 64;    
    }else if(i>64 && i<=128){
    	return 128;    
    }else if(i>128 && i<=256){
    	return 256;    
    }else if(i>256 && i<=512){
    	return 512;    
    }else if(i>512 && i<=1024){
    	return 1024;    
    }

	return 0;
}

void VideoSource::error(String error){
	logs().video.write(error);
}

void VideoSource::note(String note){
	logs().video.write(note);
}

bool VideoSource::preload(){
	return true;
}
    
    
bool VideoSource::getFrame(int frame){
	// Grabs A Frame From The Stream, for AVI sources
	return true;
}


bool VideoSource::getFrame(){
    
	   
	return true;
} 
    
bool VideoSource::sourceCapture(short driver){
	return true;
}

VideoSource::VideoSource(){
	fps=-1;    
	frame=0;
	floatFrame=0;
	resizeX=-1;
	resizeY=-1;
	
	autoPlay=true;

	powerOf2=false;
}    

bool VideoSource::sourceAvi(String s){
	return false;
}

VideoSource::~VideoSource(){

}
