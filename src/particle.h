#ifndef PARTICLE_H
#define PARTICLE_H

#include "HelperLibString.hpp"
#include "Types.hpp"
#include "script.hpp"
#include "os.h"

#include "GlHeader.hpp"

class ParticleState{
	public:
		FloatVector3d pos;
		FloatVector3d velocity;
		FloatVector4d color;
		FloatVector3d scale;
		float seed;
		float life;		//current life
		float kill;
		bool visible;
		bool reSpawn;
		ParticleState();
};

class Particle{
	private:
		float oldNumber;
		ParticleState* particles;
		void remake(ParticleState* s);

		int texid;

		unsigned int buffer;
		bool vboset;

	public:
		String name;

		bool visible;

		float ratio;
		FloatVector4d color;
		FloatVector4d colorRandom;

		FloatVector3d scale;	//size of the particle
		FloatVector3d scaleRandom;
		FloatVector3d scaleBias;

		FloatVector3d gravity;
		FloatVector3d pos;
		FloatVector3d area;		//area of release zone
		FloatVector3d velocity;
		FloatVector3d velocityRandom;

		FloatVector3d velocityJitterAmp;		//the amt of velocity change when a jitter occurs
		FloatVector3d velocityJitterFreq;		//probability that a jitter will occur in a frame.  0=no jitter, 1=every frame

		float thickness;
		
		float number;
		String type; //"lines" "sprite" "square"
		String orient;	//"y"=camera y "xyz"
		float life;
		float lifeRandom;

		bool on;
		bool load(Path);
		void draw();
		Particle();
};

int luaParticleLoad(lua_State *L);

#endif
