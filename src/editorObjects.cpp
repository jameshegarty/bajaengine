#ifdef DEVELOPER

#include "editor.h"
#include "os.h"

void addPointLightAction(){
	ObjectAddress newlight;

	String lightname="light";

	bool found=false;
	int lightcount=0;

	while(!found){
		if(level->objectKey.find(lightname) ||
			level->lightKey.find(lightname) ||
			level->cameraKey.find(lightname) ||
			level->panelKey.find(lightname) ||
			level->thumbnailsKey.find(lightname) ||
			level->particleKey.find(lightname) 
			){
	
			lightname="light"+String(lightcount);
			lightcount++;
		
		}else{
			found=true;
		}
	}

	newlight=level->add(lightname,"Point Light");

	editor.clearSelection();
	selectedLights.pushBack(newlight.light);

	editor.undo.add("level.delete('"+lightname+"')","editor.add('light')");
}

void addSpotlightAction(){
	ObjectAddress newlight;

	String lightname="spotlight";

	bool found=false;
	int lightcount=0;

	while(!found){
		if(level->objectKey.find(lightname) ||
			level->lightKey.find(lightname) ||
			level->cameraKey.find(lightname) ||
			level->panelKey.find(lightname) ||
			level->thumbnailsKey.find(lightname) ||
			level->particleKey.find(lightname) 
			){
	
			lightname="spotlight"+String(lightcount);
			lightcount++;
		
		}else{
			found=true;
		}
	}

	newlight=level->add(lightname,"spotlight");

	editor.clearSelection();
	selectedLights.pushBack(newlight.light);

	editor.undo.add("level.delete('"+lightname+"')","editor.add('spotlight')");
}

void addCubeAction(){
	ObjectAddress newcube;

	String cubename="cube";

	bool found=false;
	int cubecount=0;

	while(!found){
		if(level->objectKey.find(cubename) ||
			level->lightKey.find(cubename) ||
			level->cameraKey.find(cubename) ||
			level->panelKey.find(cubename) ||
			level->thumbnailsKey.find(cubename) ||
			level->particleKey.find(cubename) 
			){
	
			cubename="cube"+String(cubecount);
			cubecount++;
		
		}else{
			found=true;
		}
	}
	
	level->defaultMaterial();

	newcube=level->add(cubename,"polygon");

	Vertex v;
	newcube.object->scale.x=1;
	newcube.object->scale.y=1;
	newcube.object->scale.z=1;
	newcube.object->material=0;
	newcube.object->type=0;

	if(level->nonstaticObjects.find(newcube.object)==-1){
		level->nonstaticObjects.pushBack(newcube.object);
	}

	newcube.object->setVertexCount(36);
	v.pos=FloatVector3d(-1.0,-1.0,1.0);
	v.uvLightmap=FloatVector2d(0.0,0.0);
	v.uv0=FloatVector2d(0.0,0.0);
	v.normal=FloatVector3d(0.0,0.0,1.0);
	v.end=START_TRI;
	newcube.object->addVertex(v);	

	v.pos=FloatVector3d(1.0,-1.0,1.0);
	v.uvLightmap=FloatVector2d(float(1)/float(6),0.0);
	v.uv0=FloatVector2d(float(1)/float(6),0.0);
	v.normal=FloatVector3d(0.0,0.0,1.0);
	newcube.object->addVertex(v);	

	v.pos=FloatVector3d(1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(1)/float(6),1.0);
	v.uv0=FloatVector2d(float(1)/float(6),1.0);
	v.normal=FloatVector3d(0.0,0.0,1.0);
	newcube.object->addVertex(v);	

	v.pos=FloatVector3d(1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(1)/float(6),1.0);
	v.uv0=FloatVector2d(float(1)/float(6),1.0);
	v.normal=FloatVector3d(0.0,0.0,1.0);
	newcube.object->addVertex(v);		

	v.pos=FloatVector3d(-1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(0.0,1.0);
	v.uv0=FloatVector2d(0.0,1.0);
	v.normal=FloatVector3d(0.0,0.0,1.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,-1.0,1.0);
	v.uvLightmap=FloatVector2d(0.0,0.0);
	v.uv0=FloatVector2d(0.0,0.0);
	v.normal=FloatVector3d(0.0,0.0,1.0);
	newcube.object->addVertex(v);

	//back
	v.pos=FloatVector3d(-1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(2)/float(6),0.0);
	v.uv0=FloatVector2d(float(2)/float(6),0.0);
	v.normal=FloatVector3d(0.0,0.0,-1.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(2)/float(6),1.0);
	v.uv0=FloatVector2d(float(2)/float(6),1.0);
	v.normal=FloatVector3d(0.0,0.0,-1.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(1)/float(6),1.0);
	v.uv0=FloatVector2d(float(1)/float(6),1.0);
	v.normal=FloatVector3d(0.0,0.0,-1.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(1)/float(6),1.0);
	v.uv0=FloatVector2d(float(1)/float(6),1.0);
	v.normal=FloatVector3d(0.0,0.0,-1.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(1)/float(6),0.0);
	v.uv0=FloatVector2d(float(1)/float(6),0.0);
	v.normal=FloatVector3d(0.0,0.0,-1.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(2)/float(6),0.0);
	v.uv0=FloatVector2d(float(2)/float(6),0.0);
	v.normal=FloatVector3d(0.0,0.0,-1.0);
	newcube.object->addVertex(v);

	//right
	v.pos=FloatVector3d(1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(3)/float(6),0.0);
	v.uv0=FloatVector2d(float(3)/float(6),0.0);
	v.normal=FloatVector3d(1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(3)/float(6),1.0);
	v.uv0=FloatVector2d(float(3)/float(6),1.0);
	v.normal=FloatVector3d(1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(2)/float(6),1.0);
	v.uv0=FloatVector2d(float(2)/float(6),1.0);
	v.normal=FloatVector3d(1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(2)/float(6),1.0);
	v.uv0=FloatVector2d(float(2)/float(6),1.0);
	v.normal=FloatVector3d(1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,-1.0,1.0);
	v.uvLightmap=FloatVector2d(float(2)/float(6),0.0);
	v.uv0=FloatVector2d(float(2)/float(6),0.0);
	v.normal=FloatVector3d(1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(3)/float(6),0.0);
	v.uv0=FloatVector2d(float(3)/float(6),0.0);
	v.normal=FloatVector3d(1.0,0.0,0.0);
	newcube.object->addVertex(v);


	//left
	v.pos=FloatVector3d(-1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(3)/float(6),0.0);
	v.uv0=FloatVector2d(float(3)/float(6),0.0);
	v.normal=FloatVector3d(-1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,-1.0,1.0);
	v.uvLightmap=FloatVector2d(float(4)/float(6),0.0);
	v.uv0=FloatVector2d(float(4)/float(6),0.0);
	v.normal=FloatVector3d(-1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(4)/float(6),1.0);
	v.uv0=FloatVector2d(float(4)/float(6),1.0);
	v.normal=FloatVector3d(-1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(4)/float(6),1.0);
	v.uv0=FloatVector2d(float(4)/float(6),1.0);
	v.normal=FloatVector3d(-1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(3)/float(6),1.0);
	v.uv0=FloatVector2d(float(3)/float(6),1.0);
	v.normal=FloatVector3d(-1.0,0.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(3)/float(6),0.0);
	v.uv0=FloatVector2d(float(3)/float(6),0.0);
	v.normal=FloatVector3d(-1.0,0.0,0.0);
	newcube.object->addVertex(v);

	//top
	v.pos=FloatVector3d(-1.0,1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(4)/float(6),1.0);
	v.uv0=FloatVector2d(float(4)/float(6),1.0);
	v.normal=FloatVector3d(0.0,1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(4)/float(6),0.0);
	v.uv0=FloatVector2d(float(4)/float(6),0.0);
	v.normal=FloatVector3d(0.0,1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(5)/float(6),0.0);
	v.uv0=FloatVector2d(float(5)/float(6),0.0);
	v.normal=FloatVector3d(0.0,1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,1.0,1.0);
	v.uvLightmap=FloatVector2d(float(5)/float(6),0.0);
	v.uv0=FloatVector2d(float(5)/float(6),0.0);
	v.normal=FloatVector3d(0.0,1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(5)/float(6),1.0);
	v.uv0=FloatVector2d(float(5)/float(6),1.0);
	v.normal=FloatVector3d(0.0,1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(4)/float(6),1.0);
	v.uv0=FloatVector2d(float(4)/float(6),1.0);
	v.normal=FloatVector3d(0.0,1.0,0.0);
	newcube.object->addVertex(v);

	//bottom
	v.pos=FloatVector3d(-1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(1.0,1.0);
	v.uv0=FloatVector2d(1.0,1.0);
	v.normal=FloatVector3d(0.0,-1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(float(5)/float(6),1.0);
	v.uv0=FloatVector2d(float(5)/float(6),1.0);
	v.normal=FloatVector3d(0.0,-1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,-1.0,1.0);
	v.uvLightmap=FloatVector2d(float(5)/float(6),0.0);
	v.uv0=FloatVector2d(float(5)/float(6),0.0);
	v.normal=FloatVector3d(0.0,-1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(1.0,-1.0,1.0);
	v.uvLightmap=FloatVector2d(float(5)/float(6),0.0);
	v.uv0=FloatVector2d(float(5)/float(6),0.0);
	v.normal=FloatVector3d(0.0,-1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,-1.0,1.0);
	v.uvLightmap=FloatVector2d(1.0,0.0);
	v.uv0=FloatVector2d(1.0,0.0);
	v.normal=FloatVector3d(0.0,-1.0,0.0);
	newcube.object->addVertex(v);

	v.pos=FloatVector3d(-1.0,-1.0,-1.0);
	v.uvLightmap=FloatVector2d(1.0,1.0);
	v.uv0=FloatVector2d(1.0,1.0);
	v.normal=FloatVector3d(0.0,-1.0,0.0);
	v.end=END_TRI;
	newcube.object->addVertex(v);

	if(selectedPolygons.size()<1){
		selectedPolygons.pushBack();
	}

	if(selectedEdges.size()<1){
		selectedEdges.pushBack();
	}

	if(selectedVertices.size()<1){
		selectedVertices.pushBack();
	}

	selectedObjects.clear();
	selectedObjects.pushBack(newcube.object);
	editor.forceActive("selectobjects","main");

	editor.centerSelected();

	editor.undo.add("editor.deselectAll();level.delete('"+cubename+"')","editor.deselectAll();editor.add('cube');editor.select('"+cubename+"');");

	console().write("Cube '"+cubename+"' added");
}


void addGridAction(){
	ObjectAddress newcube;

	String cubename="grid";

	bool found=false;
	int cubecount=0;

	while(!found){
		if(level->objectKey.find(cubename) ||
			level->lightKey.find(cubename) ||
			level->cameraKey.find(cubename) ||
			level->panelKey.find(cubename) ||
			level->thumbnailsKey.find(cubename) ||
			level->particleKey.find(cubename) 
			){
	
			cubename="grid"+String(cubecount);
			cubecount++;
		
		}else{
			found=true;
		}
	}

	if(level->materialCount==0){

		String texfile=editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel]+"/default.bmp";

		if(!os().folderExists(editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel])){
			os().mkdir(editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel]);
		}

		if(!os().fileExists(texfile)){
			os().copy("../tools/default.bmp",texfile);
		}

		level->add("default","material");
		level->materials[0].diffuse.w=1;
		level->materials[0].diffuse.x=1;
		level->materials[0].diffuse.y=1;
		level->materials[0].diffuse.z=1;

		level->materials[0].tex0scale.x=6;

		level->materials[0].addTexture("diffuse",texfile);
	}

	newcube=level->add(cubename,"polygon");

	Vertex v;
	newcube.object->scale.x=1;
	newcube.object->scale.y=1;
	newcube.object->scale.z=1;
	newcube.object->material=0;
	newcube.object->type=0;

	if(level->nonstaticObjects.find(newcube.object)==-1){
		level->nonstaticObjects.pushBack(newcube.object);
	}

	int cx=10;
	int cz=10;

	int hcx=5;
	int hcz=5;

	float xinc=float(1)/float(cx);
	float zinc=float(1)/float(cz);

	newcube.object->setVertexCount(cx*cz*2*3);

	for(int x=0; x<cx; x++){

		for(int z=0; z<cz; z++){

			v.pos=FloatVector3d(0.0+x-hcx,0.0,0.0+z-hcz);
			v.uvLightmap=FloatVector2d(x*xinc,z*zinc);
			v.uv0=v.uvLightmap;
			v.normal=FloatVector3d(0.0,1.0,0.0);
			v.end=0;
			newcube.object->addVertex(v);

			v.pos=FloatVector3d(0.0+x-hcx,0.0,1.0+z-hcz);
			v.uvLightmap=FloatVector2d(x*xinc,(z+1)*zinc);
			v.uv0=v.uvLightmap;
			v.normal=FloatVector3d(0.0,1.0,0.0);
			v.end=0;
			newcube.object->addVertex(v);

			v.pos=FloatVector3d(1.0+x-hcx,0.0,0.0+z-hcz);
			v.uvLightmap=FloatVector2d((x+1)*xinc,z*zinc);
			v.uv0=v.uvLightmap;
			v.normal=FloatVector3d(0.0,1.0,0.0);
			v.end=0;
			newcube.object->addVertex(v);

			//////
			v.pos=FloatVector3d(0.0+x-hcx,0.0,1.0+z-hcz);
			v.uvLightmap=FloatVector2d(x*xinc,(z+1)*zinc);
			v.uv0=v.uvLightmap;
			v.normal=FloatVector3d(0.0,1.0,0.0);
			v.end=0;
			newcube.object->addVertex(v);

			v.pos=FloatVector3d(1.0+x-hcx,0.0,1.0+z-hcz);
			v.uvLightmap=FloatVector2d((x+1)*xinc,(z+1)*zinc);
			v.uv0=v.uvLightmap;
			v.normal=FloatVector3d(0.0,1.0,0.0);
			v.end=0;
			newcube.object->addVertex(v);

			v.pos=FloatVector3d(1.0+x-hcx,0.0,0.0+z-hcz);
			v.uvLightmap=FloatVector2d((x+1)*xinc,z*zinc);
			v.uv0=v.uvLightmap;
			v.normal=FloatVector3d(0.0,1.0,0.0);
			v.end=0;
			newcube.object->addVertex(v);
		}
	}

	newcube.object->getVertexRaw(0).end=START_TRI;
	newcube.object->getVertexRaw(newcube.object->getVertexCount()-1).end=END_TRI;

	if(selectedPolygons.size()<1){
		selectedPolygons.pushBack();
	}

	if(selectedEdges.size()<1){
		selectedEdges.pushBack();
	}

	if(selectedVertices.size()<1){
		selectedVertices.pushBack();
	}

	selectedObjects.clear();
	selectedObjects.pushBack(newcube.object);
	editor.forceActive("selectobjects","main");

	editor.centerSelected();

	console().write("Grid '"+cubename+"' added");

}

#endif
