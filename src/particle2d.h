#ifndef PARTICLE_2D_H
#define PARTICLE_2D_H

#include "HelperLibString.hpp"
#include "Types.hpp"
#include "script.hpp"
/*enum ParticleType{
	PARTICLE_LINES,
};*/

class Particle2dState{
	public:
		FloatVector2d pos;
		FloatVector2d scale;
		FloatVector2d velocity;
		FloatVector4d color;
		float life;		//current life
		float kill;
		bool visible;
		float seed;

		Particle2dState();
};

class Particle2d{
	private:
		int oldNumber;
		Particle2dState* particles;
		void remake(Particle2dState* s);
		int texid;

	public:
		String name;

		FloatVector4d color;
		FloatVector4d colorRandom;

		FloatVector2d scale;	//size of each particle
		FloatVector2d scaleRandom;
		FloatVector2d scaleBias;

		FloatVector2d pos;
		FloatVector2d area;				//area of release zone
		FloatVector2d velocity;			//x,y
		FloatVector2d velocityRandom;	//5, with a velocity of 0 means that it can vary from -5 to +5
		FloatVector2d velocityJitterAmp;		//the amt of velocity change when a jitter occurs
		FloatVector2d velocityJitterFreq;		//probability that a jitter will occur in a frame.  0=no jitter, 1=every frame

		int number;
		String type; //"lines" "sprite" "square"

		float ratio;	// w/h , uses x scale to calc

		float life;
		float lifeRandom;

		bool on;

		bool load(String file);	//load sprite
		void draw();
		Particle2d();
};

int luaParticle2dLoad(lua_State *L);


#endif
