#ifndef LIGHT_H
#define LIGHT_H

#include "Types.hpp"
#include "objects.hpp"
#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"
#include <iostream>

#ifndef XSICONVERT
	#include "script.hpp"
#endif

#define LIGHT_POINT 0
#define LIGHT_SPOT 1

class Light{
	private:
		Object getObj();
	public:
		int shadowMapTexture;
		Matrix4d4d lightProjectionMatrix;
		Matrix4d4d lightViewMatrix;

		bool visible;

		unsigned int sceneGraphLevel;

		SceneGraphNode* sceneGraphNode;

		unsigned char type;	//0=point light,1=spot light
		unsigned int flags;

		float intensity;
		float umbra;
		bool shadow;
		int shadowQuality;	//1=128x128 2=256 3=512 4=1024 5=2048

		FloatVector3d pos;
		FloatVector3d color;
		FloatVector3d rot;
		FloatVector3d scale;

		float coneAngle;	//0-180 (the total angle, not just half)
		float coneSpread;	//0-180 (the spotlight exponent value thingy) 0==fullbright 180==full diffuse
		float falloffStart;
		float falloffEnd;
		String name;

		void write(std::ofstream& out);
		unsigned long size();

		float distance();	//distance from camera

		Light();
};

#endif
