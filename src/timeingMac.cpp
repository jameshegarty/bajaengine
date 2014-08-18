#include "timeing.hpp"

#include <math.h>

#include "script.hpp"
#include "conf.h"

#include <Carbon/Carbon.h>

EngineTime engineTime;

EngineTime::EngineTime(){
	idealFps=60;
	fps=0;
	time=1;

	frameCount=0;
	skipFrame=false;
}

void EngineTime::calculateFps(){
	static double framesPerSecond    = 0.0f;							// This will store our fps
    static double lastTime			= 0.0f;							// This will hold the time from the last frame

	UnsignedWide teatime;
	Microseconds(&teatime);
	double currentTime=double(UnsignedWideToUInt64(teatime))*double(0.000001);
    ++framesPerSecond;

	frameTime=currentTime - lastTime;


    if( ((currentTime - lastTime) > 0.25f) && ((currentTime - lastTime)<1.0f) ){
		lastTime = currentTime;
		fps=framesPerSecond*4;
		framesPerSecond = 0;
	}else if((currentTime - lastTime) > 1.0f){
		//obviously a bad value - due to a level load or something
		//so we just let fps be the same thing it was before

		if(conf->logDroppedFrames){
			console().write("DROPPEDFRAME");
		}

		lastTime = currentTime;
		framesPerSecond = 0;
	}
}

void EngineTime::clearTimer(String name){
	stimers[name]=0;
	timers[name]=0;
}


void EngineTime::startTimer(String name){
	UnsignedWide teatime;
	Microseconds(&teatime);
	double currentTime=double(UnsignedWideToUInt64(teatime));//*double(0.001);
	stimers[name]=currentTime;
}

void EngineTime::endTimer(String name,bool add){
	UnsignedWide teatime;
	Microseconds(&teatime);
	double currentTime=double(UnsignedWideToUInt64(teatime));//*double(0.001);
	currentTime-=stimers[name];
	
	if(add){
		currentTime+=timers[name];
		timers[name]+=currentTime;
	}else{
		timers[name]=currentTime;
	}
	

}

double EngineTime::getTimer(String name){
	return timers[name];
}


double EngineTime::getTimerAvg(String name){
	if(avg[name]==0){
		avg[name]=timers[name];
	}else{
		avg[name]=avg[name]*double(0.95)+timers[name]*double(0.05);
	}
	
	return avg[name];
}

double EngineTime::getTimerPercent(String name){
	double lol=avg[name]/fcountavg;
	lol*=100;
	lol=ceil(lol);
	
	return lol;
}

void EngineTime::perFrame(){

	calculateFps();
		
	float tfps=fps;
	if (tfps<=1){tfps=1;}	//prevent devide by 0 errors
	time=(idealFps/tfps);

	frameCount++;
	
	UnsignedWide teatime;
	Microseconds(&teatime);
	double currentTime=double(UnsignedWideToUInt64(teatime));//*double(0.001);

	fcount=currentTime-lfcount;

	if(fcountavg==0){
		if(lfcount!=0){
			fcountavg=fcount;
		}
	}else{
		fcountavg=(fcountavg*0.95)+(fcount*0.05);
	}

	lfcount=currentTime;

}

void EngineTime::skipNextFrame(){

}


void EngineTime::startTimerSingle(String name){
	UnsignedWide teatime;
	Microseconds(&teatime);
	double currentTime=double(UnsignedWideToUInt64(teatime))*double(0.000001);
	timersSingle[name]=currentTime;
}

double EngineTime::getTimerSingle(String name){
	UnsignedWide teatime;
	Microseconds(&teatime);
	double currentTime=double(UnsignedWideToUInt64(teatime))*double(0.000001);
	return( currentTime-timersSingle[name]);
}
