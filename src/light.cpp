#include "light.h"

Light::Light(){
	falloffStart=0;
	falloffEnd=100;
	umbra=1;
	color.x=1;
	color.y=1;
	color.z=1;
	intensity=1;
	type=LIGHT_POINT;
	coneAngle=60;
	coneSpread=2;
	shadow=true;
	shadowQuality=2;
	shadowQuality=3;

	visible=true;

	shadowMapTexture=-1;
}

Object Light::getObj(){
	Object obj;
 
	if(type==LIGHT_POINT){
		obj.type=1;
	}else{
		obj.type=2;
	}

	obj.name=name;

	obj.pos=pos;
	obj.rot=rot;
	obj.scale.x=falloffStart;
	obj.scale.y=falloffEnd;
	obj.scale.z=coneAngle;
	obj.setVertexCount(1);
	obj.sceneGraphLevel=sceneGraphLevel;

	Vertex v;
	v.pos=color;
	v.normal.x=intensity;
	v.normal.y=umbra;
	v.normal.z=shadow;
	v.uvLightmap.x=coneSpread;
	obj.addVertex(v);

	obj.sceneGraphNode=sceneGraphNode;

	return obj;
}

void Light::write(std::ofstream& out){
	Object obj=getObj();

	obj.write(out);


}

unsigned long Light::size(){
	Object obj=getObj();
	return obj.size();
}