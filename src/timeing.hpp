#ifndef TIME_H
#define TIME_H


#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"

#ifndef _WIN32
#define LARGE_INTEGER double
#endif

class EngineTime{
	private:
		Map<String,LARGE_INTEGER> timers;
		Map<String,LARGE_INTEGER> stimers;
		Map<String,LARGE_INTEGER> avg;

		Map<String,double> timersSingle;

		LARGE_INTEGER lfcount;
		LARGE_INTEGER fcount;
		LARGE_INTEGER fcountavg;

		
		bool skipFrame;

	public:
		void clearTimer(String name);

		void startTimer(String name);
		void endTimer(String name,bool add=false);
		double getTimer(String name);
		double getTimerAvg(String name);
		double getTimerPercent(String name);

		void startTimerSingle(String name);
		double getTimerSingle(String name);

		float idealFps;
    	float fps;
    	void calculateFps();
    	void perFrame();
    	float time;
		float frameTime;	//the time it takes to draw a frame
		float frameCount;	//the current frame we're on, ie, the number of frames that have been drawn
    	void skipNextFrame();	//call this after doing something that's going to take a really like time, like loading a level, so it doesn't fschk the time
    	EngineTime();
	
};

extern EngineTime engineTime;

#endif
