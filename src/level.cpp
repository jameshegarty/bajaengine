#include "level.hpp"

#include <iostream>
#include <fstream>

#ifdef _WIN32
	#include <windows.h>	// Header File For Windows
#endif

#include "GlHeader.hpp"
#include "Log.hpp"
#include "HelperLibMath.hpp"
#include "FileIo.hpp"
#include "Bmp.hpp"
#include "texture.hpp"
#include "camera.hpp" 
#include "SceneGraph.hpp"
#include "conf.h"
#include "foam.hpp"
#include "draw.hpp"
#include "editor.h"
#include "os.h"
#include "leaks.h"

using namespace std;

Array<Level*> loadedLevels;

Level* level;

bool newLevel(){
	script().run("if (lvl==nil) then lvl={} end");
	script().run("if (lvl."+level->name+"==nil) then lvl."+level->name+"={} end");


	script().rename(level->luaObj,"obj"+level->name);
	script().rename(level->luaMat,"mat"+level->name);

	script().remove(level->luaAddress);	//level
	script().remove(level->luaFog);

	script().rename(level->luaCamera,"camera"+level->name);
	script().run("lvl."+level->name+".camera=camera"+level->name);

	script().syncFrom(NULL);
	script().syncTo(NULL);

	Level* t=new Level;
	loadedLevels.pushBack(t);
	level=t;

	level->scriptInit();

	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}

bool deleteLevel(String name){
	int FL=-1;

	for(int i=0; i<loadedLevels.size(); i++){
		if(loadedLevels[i]->name==name){
			FL=i;
		}
	}

	if(FL==-1){
		console().write("error, level "+name+" not found");
		return false;
	}

	delete loadedLevels[FL];

	loadedLevels.erase(FL);

	return true;
}

bool hideLevel(String name){
	for(int i=0; i<loadedLevels.size(); i++){

		if(loadedLevels[i]->name==name){
			loadedLevels[i]->visible=false;
			return true;
		}
	}

	return false;
}

bool showLevel(String name){


	for(int i=0; i<loadedLevels.size(); i++){

		if(loadedLevels[i]->name==name){

			loadedLevels[i]->visible=true;

			script().run("if (lvl==nil) then lvl={} end");
			script().run("if (lvl."+level->name+"==nil) then lvl."+level->name+"={} end");

			script().rename(level->luaObj,"obj"+level->name);
			script().rename(level->luaMat,"mat"+level->name);

			script().remove(level->luaAddress);	//level
			script().remove(level->luaFog);

			script().rename(level->luaCamera,"camera"+level->name);
			script().run("lvl."+level->name+".camera=camera"+level->name);
			
			script().syncFrom(NULL);
			script().syncTo(NULL);
			
			level=loadedLevels[i];

			level->scriptInit(true);

			script().rename(level->luaMat,"mat");
			
			script().rename(level->luaObj,"obj");
			
			script().syncTo(NULL);

			return true;
		}
	}

	console().write("error, level '"+name+"' not found");

	return false;
}


bool Level::luaUploadAll(){	//NOT CURRENTLY USING
	for(int i=0; i<nonstaticObjects.size(); i++){
		luaUpload(nonstaticObjects[i]);
	}


	for(int i=0; i<panels.size(); i++){
		luaUpload(panels[i]);
	}


	return true;
}

bool Fog::update(){
	float temp[4];
	temp[0]=color.x;
	temp[1]=color.y;
	temp[2]=color.z;
	
	if(on){
		glClearColor (color.x,color.y,color.z,1.0f);
		temp[3]=1;
	}else{
		glClearColor (color.x,color.y,color.z,1.0f);
		temp[3]=0;	//so that shaders don't show fog when it's off
	}
	
	glFogfv (GL_FOG_COLOR, temp);

	if(on){
		 glEnable (GL_FOG);
		 glFogi(GL_FOG_MODE,GL_LINEAR);
		 glHint (GL_FOG_HINT, GL_NICEST);
	}else{
		 glDisable (GL_FOG);
	}

	glFogf(GL_FOG_START,fogNear);
	glFogf(GL_FOG_END,fogFar);

	
	return true;
}

void Fog::perFrame(){
	if(conf->glow){

		glDisable(GL_DEPTH_TEST);

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);

		glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
		glLoadIdentity ();

		glOrtho(-1,1,-1,1,-1,1);

		glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
		glLoadIdentity ();

		glDisable(GL_LIGHTING);


		glColor3f(color.x,color.y,color.z);

		glBegin(GL_QUADS);
			glTexCoord2d(1,1); glVertex3f(1,1,0.0f);
			glTexCoord2d(1,0); glVertex3f(1,-1,0.0f);
			glTexCoord2d(0,0); glVertex3f(-1,-1,0.0f);
			glTexCoord2d(0,1); glVertex3f(-1,1,0.0f);

		glEnd();
	}else{
		glClearColor (color.x,color.y,color.z,1.0f);
	}

	
}


Fog::Fog(){
	fogNear=100;
	fogFar=500;

	on=true;
	
	color.x=1.0f;
	color.y=1.0f;
	color.z=1.0f;
}



void Level::loadingScreen(){
	//what the hell is this?
	
	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));
	
	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity();
	
	glOrtho(0,conf->sizeX,-conf->sizeY,0,-1,1);
	
	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	
	glLoadIdentity ();	
	
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	
	glDisable(GL_BLEND);
	
	glColor4f(1,0,0,1);


	glBegin(GL_QUADS);
		glVertex3f((float)0			,0				,0);
		glVertex3f((float)conf->sizeX	,0				,0);
		glVertex3f((float)conf->sizeX	,-conf->sizeY	,0);
		glVertex3f((float)0			,-conf->sizeY	,0);
	glEnd();
}

bool Level::copy(String obj1, String obj2){

	if(level->objectKey.find(obj1)){
		//objectKey only contains polymeshes

		level->add(obj2,"mesh");

		Object* fm=level->objectKey[obj1];
		Object* to=level->objectKey[obj2];

		to->become(fm);
		
		return true;
	}else{

		console().write("copy error, source object not found");

		return 0;
	}

	return false;
}

bool Level::rename(String from, String to){

	if(objectKey.find(from)){
		Object* obj=objectKey[from];
		objectKey.remove(from);

		luaRemove(obj);

		obj->name=to;

		objectKey[to]=obj;

		luaUpload(obj);

		return true;
	}

	return false;
}

bool Level::remove(String name){
 
	if(objectKey.find(name)){
		Object* o=objectKey[name];


		sceneGraph.remove(o->sceneGraphNode);

		if(o->material!=-1){
			if(materials[o->material].nonstaticRefList.find(o)!=-1){
				materials[o->material].nonstaticRefList.eraseValue(o);
				materials[o->material].nonstaticRefs--;
			}
		}

		objects.erase(objects.find(objectKey[name]));

		if(staticObjects.find(objectKey[name])!=-1){
			staticObjects.erase(staticObjects.find(objectKey[name]));
			drawRefreshAll();
		}
	
		if(nonstaticObjects.find(objectKey[name])!=-1){
			nonstaticObjects.erase(nonstaticObjects.find(objectKey[name]));
		}

		if(colliders.find(objectKey[name])!=-1){
			colliders.erase(colliders.find(objectKey[name]));
		}

		if(transparentObjects.find(objectKey[name])!=-1){
			transparentObjects.erase(transparentObjects.find(objectKey[name]));
		}

		if(water==objectKey[name]){
			water=NULL;
		}

		script().run("obj[\""+name+"\"]=nil");

		objectKey.remove(name);

		delete o;
		leaks.rem("Level::objects");

	}else if(lightKey.find(name)){
		Light* light=lightKey[name];

		sceneGraph.remove(light->sceneGraphNode);

		lights.erase(lights.find(lightKey[name]));

		lightKey.remove(name);
		delete light;
		leaks.rem("Level::lights");

	}else if(particleKey.find(name)){
		
		delete particleKey[name];
		leaks.rem("Level::particles");

		particles.erase(particles.dataRef(particleKey[name]));
		
	}else if(nullKey.find(name)){
		
		NullObject* null=nullKey[name];

		sceneGraph.remove(null->sceneGraphNode);

		

		delete nullKey[name];
		leaks.rem("Level::nulls");

		nulls.erase(nulls.dataRef(null));
		
	}else if(panelKey.find(name)){
	
		luaRemove(panelKey[name]);
		delete panelKey[name];
		leaks.rem("Level::panels");

		int l=panels.find(panelKey[name]);
		
		if(l!=-1){
			panels.erase(l);
		}else{
			console().write("delete error");
		}
		

		panelKey.remove(name);
	}else if(instanceKey.find(name)){

		Instance* instance=instanceKey[name];

		if(instance->sceneGraphNode->children.size()>0){
			for(int i=0; i<instance->sceneGraphNode->children.size(); i++){
				instance->sceneGraphNode->parent->children.pushBack(instance->sceneGraphNode->children[i]);
				instance->sceneGraphNode->children[i].parent=instance->sceneGraphNode->parent;
			}
		}

		for(int i=0; i<instance->sceneGraphNode->parent->children.size(); i++){
			if(&instance->sceneGraphNode->parent->children[i]==instance->sceneGraphNode){
				instance->sceneGraphNode->parent->children.erase(i);
				break;
			}
		}

		luaRemove(instanceKey[name]);
		
		int l=instances.find(instanceKey[name]);
		
		if(l!=-1){
			instances.erase(l);
		}else{
			console().write("delete error");
		}

		delete instanceKey[name];
		leaks.rem("Level::instances");

		instanceKey.remove(name);
	}else{

		console().write("delete error: object '"+name+"' not found");
		return false;
	}

	return true;
}

ObjectAddress Level::add(String name,String type){

	if( (objectKey.find(name) ||
		lightKey.find(name) ||
		sphereKey.find(name) ||
		diskKey.find(name) ||
		cameraKey.find(name) ||
		panelKey.find(name) ||
		instanceKey.find(name) ||
		thumbnailsKey.find(name) ||
		particleKey.find(name) ) && (type!="Material" && type!="material")
		){
			console().write("add error: an object named '"+name+"' already exists!");

			ObjectAddress a;
			a.type=NULLOBJECT;
			return a;
	}


	if(materialKey.find(name) && (type=="material" || type=="Material")){

			console().write("add error: a material named '"+name+"' already exists!");

			ObjectAddress a;
			a.type=NULLOBJECT;
			return a;
	}


	if(type=="object" || type=="poly" || type=="polygon" || type=="mesh" || type=="polymesh" || type=="PolyMesh"){

		Object* c=new Object;
		leaks.alloc("Level::objects");
		c->name=name;
		c->type=0;
		c->material=0;
		defaultMaterial();
		objects.pushBack(c);
		nonstaticObjects.pushBack(c);
		objectKey[name]=c;

		luaUpload(c);
		
		ObjectAddress t;
		t.type=OBJECT;
		t.object=c;

		SceneGraphNode node;
		node.object=t;
		node.parent=&sceneGraph.root;
		sceneGraph.root.children.pushBack(node);
		c->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return t;

	}else if(type=="camera" || type=="Camera"){

		Camera* c=new Camera;
		c->name=name;
		cameras.pushBack(c);
		cameraKey[name]=c;

		luaUpload(c);

		ObjectAddress t;
		t.type=CAMERA;
		t.camera=c;

		return t;

	}else if(type=="panel" || type=="Panel"){

		Panel* p=new Panel;
		leaks.alloc("Level::panels");
		p->name=name;
		panels.pushBack(p);
		panelKey[name]=p;

		luaUpload(p);
		
		

		ObjectAddress t;
		t.type=PANEL;
		t.panel=p;


		return t;

	}else if(type=="thumbnails" || type=="Thumbnails"){

		Thumbnails* t=new Thumbnails;
		t->name=name;
		thumbnails.pushBack(t);
		thumbnailsKey[name]=t;

		luaUpload(t);
		
		

		ObjectAddress a;
		a.type=THUMBNAILS;
		a.thumbnails=t;


		return a;

	}else if(type=="videopanel" || type=="VideoPanel"){

		VideoPanel* t=new VideoPanel;
		t->name=name;
		videopanels.pushBack(t);
		videopanelKey[name]=t;

		luaUpload(t);

		ObjectAddress a;
		a.type=VIDEOPANEL;
		a.videopanel=t;

		return a;

	}else if(type=="textpanel" || type=="TextPanel"){

		TextPanel* t=new TextPanel;
		t->name=name;
		textpanels.pushBack(t);
		textpanelKey[name]=t;

		luaUpload(t);
	
		ObjectAddress ta;
		ta.type=TEXTPANEL;
		ta.textpanel=t;

		return ta;

	}else if(type=="spotlight" || type=="SpotLight"){
		Light* l=new Light;
		l->name=name;
		l->type=LIGHT_SPOT;
		lights.pushBack(l);
		lightKey[name]=l;

		luaUpload(l);

		ObjectAddress t;
		t.type=LIGHT;
		t.light=l;

		SceneGraphNode node;
		node.object=t;
		node.parent=&sceneGraph.root;
		sceneGraph.root.children.pushBack(node);
		l->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return t;

	}else if(type=="pointlight" || type=="PointLight" || type=="Point Light"){
		Light* l=new Light;
		l->name=name;
		l->type=LIGHT_POINT;
		lights.pushBack(l);
		lightKey[name]=l;

		luaUpload(l);

		ObjectAddress t;
		t.type=LIGHT;
		t.light=l;

		SceneGraphNode node;
		node.object=t;
		node.parent=&sceneGraph.root;
		sceneGraph.root.children.pushBack(node);
		l->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];


		return t;

	}else if(type=="hair" || type=="Hair"){

		Hair* t=new Hair;
		t->name=name;
		hair.pushBack(t);
		hairKey[name]=t;

		luaUpload(t);
		
		

		ObjectAddress a;
		a.type=HAIR;
		a.hair=t;

	
		return a;

	}else if(type=="line" || type=="Line"){

		Line3d* t=new Line3d;
		t->name=name;
		lines.pushBack(t);
		lineKey[name]=t;

		luaUpload(t);
		
		

		ObjectAddress a;
		a.type=LINE;
		a.line=t;

		SceneGraphNode node;
		node.object=a;
		sceneGraph.root.children.pushBack(node);
		t->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return a;

	}else if(type=="instance" || type=="Instance"){

		Instance* t=new Instance;
		t->name=name;
		instances.pushBack(t);
		instanceKey[name]=t;

		luaUpload(t);
		
	
		ObjectAddress a;
		a.type=INSTANCE;
		a.instance=t;

		SceneGraphNode node;
		node.object=a;
		node.parent=&sceneGraph.root;
		sceneGraph.root.children.pushBack(node);
		t->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return a;

	}else if(type=="circle" || type=="Circle"){

		Circle* t=new Circle;
		t->name=name;
		circles.pushBack(t);
		circleKey[name]=t;

		luaUpload(t);
		
		

		ObjectAddress a;
		a.type=CIRCLE;
		a.circle=t;

		SceneGraphNode node;
		node.object=a;
		sceneGraph.root.children.pushBack(node);
		t->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return a;

	}else if(type=="sphere" || type=="Sphere"){

		Sphere* t=new Sphere;
		t->name=name;
		spheres.pushBack(t);
		sphereKey[name]=t;

		luaUpload(t);		

		ObjectAddress a;
		a.type=LINE;
		a.sphere=t;

		SceneGraphNode node;
		node.object=a;
		sceneGraph.root.children.pushBack(node);
		t->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return a;

	}else if(type=="box" || type=="Box"){

		Box* t=new Box;
		t->name=name;
		boxes.pushBack(t);
		boxKey[name]=t;

		luaUpload(t);		

		ObjectAddress a;
		a.type=LINE;
		a.box=t;

		SceneGraphNode node;
		node.object=a;
		sceneGraph.root.children.pushBack(node);
		t->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return a;

	}else if(type=="disk" || type=="Disk"){

		Disk* t=new Disk;
		t->name=name;
		disks.pushBack(t);
		diskKey[name]=t;

		luaUpload(t);		

		ObjectAddress a;
		a.type=DISK;
		a.disk=t;

		SceneGraphNode node;
		node.object=a;
		sceneGraph.root.children.pushBack(node);
		t->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return a;

	}else if(type=="line2d" || type=="Line2d"){

		Line2d* t=new Line2d;
		t->name=name;
		lines2d.pushBack(t);
		line2dKey[name]=t;

		luaUpload(t);
		
		

		ObjectAddress a;
		a.type=LINE2D;
		a.line2d=t;

	
		return a;

	}else if(type=="point" || type=="Point"){

		Point3d* t=new Point3d;
		t->name=name;
		points.pushBack(t);
		pointKey[name]=t;

		luaUpload(t);
		
		

		ObjectAddress a;
		a.type=POINT3D;
		a.point=t;

		SceneGraphNode node;
		node.object=a;
		sceneGraph.root.children.pushBack(node);
		t->sceneGraphNode=&sceneGraph.root.children[sceneGraph.root.children.size()-1];

		return a;

	}else if(type=="material" || type=="Material"){

		materials.pushBack();
		materials[materials.size()-1].name=name;
		materialKey[name]=&materials[materials.size()-1];
		materialIndexKey[name]=materials.size()-1;
		materialCount++;

		//don't upload now, we'll add it when we actually need it in materials[].update()
		//actually, upload it now, if we're adding it at runtime it's nonstatic
		luaUpload(&materials[materials.size()-1]);
		materials[materials.size()-1].uploaded=true;
	
		

		ObjectAddress a;
		a.type=MATERIAL;
		a.material=&materials[materials.size()-1];
	

		return a;

	}else if(type=="particle" || type=="particles" || type=="Particle"){
		Particle* p=new Particle;
		p->name=name;
		particles.pushBack(p);
		particleKey[name]=p;
		luaUpload(p);

		ObjectAddress t;
		t.type=PARTICLE;
		t.particle=p;

		return t;
	}else if(type=="particle2d" || type=="particles2d" || type=="Particle2d"){
		Particle2d* p=new Particle2d;
		p->name=name;
		particles2d.pushBack(p);
		particle2dKey[name]=p;
		luaUpload(p);

		ObjectAddress t;
		t.type=PARTICLE_2D;
		t.particle2d=p;

		
		return t;
	}else if(type=="null"){
		NullObject* p=new NullObject;
		p->name=name;
		nulls.pushBack(p);
		nullKey[name]=p;
		luaUpload(p);

		ObjectAddress t;
		t.type=NULL3D;
		t.null=p;

		
		return t;
	}else if(type=="curve"){
		Curve* p=new Curve;
		p->name=name;
		curves.pushBack(p);
		curveKey[name]=p;
		luaUpload(p);

		ObjectAddress t;
		t.type=CURVE;
		t.curve=p;

		
		return t;
	}else{

		console().write("level.add error: unknown type '"+type+"'");
	}

	ObjectAddress t;
	t.type=NONE;
	t.object=NULL;
	return t;
	
	

}

bool Level::scriptInit(bool reload){
	
	
	luaManagerAddress=script().add("levelManager");
		
		
		script().add("delete",luaLevelDelete,luaManagerAddress);
		script().add("list",luaLevelList,luaManagerAddress);
		script().add("swap",luaLevelSwap,luaManagerAddress);

		script().add("hide",luaLevelHide,luaManagerAddress);
		script().add("show",luaLevelShow,luaManagerAddress);
		script().add("new",luaLevelNew,luaManagerAddress);


	luaAddress=script().add("level");
		script().add("load",luaLoad,luaAddress);
		script().add("save",luaSave,luaAddress);
		script().add("add",luaAdd,luaAddress);
		script().add("mask",luaMask,luaAddress);
		script().add("delete",luaDelete,luaAddress);
		script().add("copy",luaCopy,luaAddress);
		script().add("name",&name,luaAddress);
		script().add("rename",luaRename,luaAddress);

		LuaType* lvp=script().add("viewport",luaAddress);
			LuaType* lvpp=script().add("pos",lvp);
				script().add("x",&viewportPos.x,lvpp);
				script().add("y",&viewportPos.y,lvpp);
			LuaType* lvps=script().add("scale",lvp);
				script().add("x",&viewportScale.x,lvps);
				script().add("y",&viewportScale.y,lvps);

		LuaType* lluaa=script().add("ambient",luaAddress);
			script().add("r",&ambient.x,lluaa);
			script().add("g",&ambient.y,lluaa);
			script().add("b",&ambient.z,lluaa);

	if(!reload){
		luaObj=script().add("obj");
		luaMat=script().add("mat");
	
	}

	luaFog=script().add("fog");
		script().add("on",&fog.on,luaFog);
		script().add("near",&fog.fogNear,luaFog);
		script().add("far",&fog.fogFar,luaFog);

		LuaType* clua=script().add("color",luaFog);
			script().add("r",&fog.color.x,clua);
			script().add("g",&fog.color.y,clua);
			script().add("b",&fog.color.z,clua);

	luaCamera=script().add("camera");
		luaCameraFov=script().add("fov",&camera->fov,luaCamera);
		luaCameraFov=script().add("orthographic",&camera->orthographic,luaCamera);
		luaCameraFly=script().add("fly",&camera->fly,luaCamera);
		luaCameraSpeed=script().add("speed",&camera->speed,luaCamera);
		luaCameraFreeze=script().add("freeze",&camera->freeze,luaCamera);
		luaCameraFreezeFrustum=script().add("freezeFrustum",&camera->freezeFrustum,luaCamera);

		LuaType* cccp=script().add("pos",luaCamera);
			luaCameraPosX=script().add("x",&camera->pos.x,cccp);
			luaCameraPosY=script().add("y",&camera->pos.y,cccp);
			luaCameraPosZ=script().add("z",&camera->pos.z,cccp);

		LuaType* cccr=script().add("rot",luaCamera);
			luaCameraRotX=script().add("x",&camera->rot.x,cccr);
			luaCameraRotY=script().add("y",&camera->rot.y,cccr);
			luaCameraRotZ=script().add("z",&camera->rot.z,cccr);


		LuaType* cccs=script().add("scale",luaCamera);
			luaCameraScaleX=script().add("x",&camera->ellipsoidRadius.x,cccs);
			luaCameraScaleY=script().add("y",&camera->ellipsoidRadius.y,cccs);
			luaCameraScaleZ=script().add("z",&camera->ellipsoidRadius.z,cccs);

		LuaType* cccc=script().add("clip",luaCamera);
			luaCameraClipNear=script().add("near",&camera->clipNear,cccc);
			luaCameraClipFar=script().add("far",&camera->clipFar,cccc);

		LuaType* cccl=script().add("limit",luaCamera);
			LuaType* ccclX=script().add("x",cccl);
				luaCameraLimitXOn=script().add("on",&camera->limitXOn,ccclX);
				luaCameraLimitXMin=script().add("min",&camera->limitXMin,ccclX);
				luaCameraLimitXMax=script().add("max",&camera->limitXMax,ccclX);

	if(reload){
		script().syncTo();
	}

	return true;
}

Level::Level(){
	objectCount=0;

	ambient.x=1;
	ambient.y=1;
	ambient.z=1;

	visible=true;

	Camera* defaultCam=new Camera;
	camera=defaultCam;


	water=NULL;

	textureCount=0;
	lightmapCount=0;
	materialCount=0;

	viewportPos.x=0;
	viewportPos.y=0;

	viewportScale.x=1;
	viewportScale.y=1;
}

void Level::perFrame(){
	fog.perFrame();
}


void Level::update(){
	fog.update();

	float amb[4];

	amb[0]=ambient.x;
	amb[1]=ambient.y;
	amb[2]=ambient.z;
	amb[3]=1;

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);

	for(int i=0; i<materials.size(); i++){
		if(materials[i].uploaded || materials[i].nonstaticRefs>0){
			materials[i].update();
		}
	}

	for(int i=0; i<nonstaticObjects.size(); i++){
		nonstaticObjects[i]->update();
	}

	for(int i=0; i<transparentObjects.size(); i++){
		transparentObjects[i]->update();
	}

	for(int i=0; i<ikEffectors.size(); i++){
		ikEffectors[i]->update();
	}

	for(int i=0; i<ikRoots.size(); i++){
		ikRoots[i]->update();
	}

	for(int i=0; i<nulls.size(); i++){
		nulls[i]->update();
	}
}

float Light::distance(){
	return math.distance(level->camera->pos.x,level->camera->pos.y,level->camera->pos.z,
		pos.x,pos.y,pos.z);

}

bool reloadLevel(){
	
	level->objectCount=0;
	level->lightmapCount=0;

	for(int i=0; i<level->objects.size(); i++){
		delete level->objects[i];
	}

	level->nonstaticObjects.clear();
	level->transparentObjects.clear();
	level->staticObjects.clear();
	level->reflectiveObjects.clear();
	level->colliders.clear();
	level->objects.clear();



	level->textureCount=0;

	level->load(level->file);

	drawRefreshAll();


	return true;
}

Level::~Level(){

	script().run("mat"+name+"=nil");
	script().run("obj"+name+"=nil");
	script().run("collectgarbage('collect')");

	script().deleteGlobal(luaMat);
	script().deleteGlobal(luaObj);

	glDeleteLists(glListName,1);

	for(int i=0; i<nonstaticObjects.size(); i++){
		glDeleteLists(nonstaticObjects[i]->glListName,1);
		glDeleteLists(nonstaticObjects[i]->glListNameQuick,1);
	}

	for(int i=0; i<transparentObjects.size(); i++){
		glDeleteLists(transparentObjects[i]->glListName,1);
		glDeleteLists(transparentObjects[i]->glListNameQuick,1);
	}

	for(int i=0; i<textureCount; i++){
		const GLuint tempuiui=textures[i].glName();
		glDeleteTextures(1,&tempuiui);
	}

	for(int i=0; i<lightmapCount; i++){
		const GLuint tempuiui=lightmaps[i].glName();
		glDeleteTextures(1,&tempuiui);
	}

	for(int i=0; i<objects.size(); i++){
		delete objects[i];
	}

	for(int i=0; i<panels.size(); i++){
		delete panels[i];
	}

	for(int i=0; i<nulls.size(); i++){
		delete nulls[i];
	}

}

bool Level::defaultMaterial(){
	if(materialCount==0){

		

#ifdef DEVELOPER

		String texfile;

		if(editor.projectList.size()>0 && editor.levelList.size()>0){
			texfile=editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel]+"/default.bmp";

			if(!os().folderExists(editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel])){
				os().mkdir(editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel]);
			}

			if(!os().fileExists(texfile)){
				os().copy(String("../tools/default.bmp"),texfile);
			}
		}
#endif

		add("default","material");
		materials[0].diffuse.w=1;
		materials[0].diffuse.x=1;
		materials[0].diffuse.y=1;
		materials[0].diffuse.z=1;

		materials[0].tex0scale.x=6;

#ifdef DEVELOPER
		materials[0].addTexture("diffuse",texfile);
#endif
	}

	return true;
}

bool Level::objectExists(String name){
	if(objectKey.find(name)){
		return true;
	}

	if(nullKey.find(name)){
		return true;
	}

	if(lightKey.find(name)){
		return true;
	}

	if(panelKey.find(name)){
		return true;
	}

	if(textpanelKey.find(name)){
		return true;
	}

	if(instanceKey.find(name)){
		return true;
	}

	if(ikRootKey.find(name)){
		return true;

	}

	if(ikJointKey.find(name)){
		return true;
	}

	if(ikEffectorKey.find(name)){
		return true;
	}

	return false;
}
