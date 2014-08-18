#ifndef LIGHTMAP_H
#define LIGHTMAP_H

#ifdef DEVELOPER

#include "objects.hpp"
#include <string>

class ObjectShadowVolume{
	public:
		bool built;
		FloatVector3d pos;	//the pos of this object when this was generated
		FloatVector3d scale;
		FloatVector3d rot;
		Array<Plane3d> planes;
		Array<Ray3d> edgesStore;
		String name;
		//Array<FloatVector3d> normals;

		ObjectShadowVolume(){
			built=false;
		}
};

class LightShadowVolume{
	public:
		bool built;
		FloatVector3d pos;	//the pos of the light when this was generated
		Array<ObjectShadowVolume> objects;

		LightShadowVolume(){
			built=false;
		}
};

extern Map<String,LightShadowVolume> shadowVolumes;	//<light name, volume obj>

class Lightmap{
	public:
		bool scriptInit();
		bool build(Object* obj,int sx,int sy);
};

extern Lightmap lightmap;
#endif

#endif
