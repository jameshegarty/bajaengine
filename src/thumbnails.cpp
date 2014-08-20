#include "thumbnails.h"

#ifdef _WIN32
#include <gl/gl.h>												// Header File For The OpenGL32 Library
#include <gl/glu.h>												// Header File For The GLu32 Library
#include "gl/extensions.h"
#elif LINUX
	#include <gl.h>
#else
#include <GLUT/glut.h>
#endif

#include "level.hpp"
#include "texture.hpp"
#include "picking.h"
#include <math.h>

#include "conf.h"

#include "os.h"

#define LATE_LOAD
#define VISIBLE_SET
#define DISPLAY_LIST

bool Thumbnails::load(Thumb* t, String file){
	int g=texture.load(file);

	ImageData im=texture.info(g);

	if(g!=-1){
		t->thumbTex=g;
		return true;
	}

	console().write("Error loading file '"+file+"', "+im.error);
	return false;
}

bool Thumbnails::add(String file){

	if(file==""){
		return false;
	}

	if(remap.find(file)){
		visibleSet.pushBack(&thumbs[remap[file]]);
		return true;
	}

	Thumb t;
	
	#ifndef LATE_LOAD
		if(!load(&t,file)){
			return false;
		}
		t.loaded=true;
	#else
		if(!os().fileExists(file)){
			console().write("file "+file+" doesn't exist");
			return false;
		}

	#endif

	t.path=file;
	thumbs.pushBack(t);

	remap[file]=thumbs.size()-1;

	visibleSet.pushBack(&thumbs[thumbs.size()-1]);

	return true;
}

Thumb::Thumb(){
	rotate=0;
	loaded=false;
	
}


void Thumbnails::clear(){
	dirty=true;
	visibleSet.clear();
}

Array<Thumb*> Thumbnails::currentlyVisible(){
	return currentlyVisibleThumbs;
}

Array<Thumb*> Thumbnails::getVisibleSet(){
	return visibleSet;
}

void Thumbnails::doDraw(){
	currentlyVisibleThumbs.clear();

	if(!visible){return;}

	float thumbsPerLine=0;

	thumbsPerLine=scale.x/(thumbnailSize+20);
	thumbsPerLine=floor(thumbsPerLine)-1;

	float spacing=0;

	picsPerRow=thumbsPerLine;

	spacing=scale.x/(thumbsPerLine+1);

	float col=0;
	float row=0;

	float px=0;
	float py=0;
	float pz=pos.z;

	

	float shim=(spacing-thumbnailSize)/2;

	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE5_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE6_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE7_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE8_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	glColor4f(1.0f,1.0f,1.0f,1.0f);

	float t_x1=0;
	float t_x2=0;
	float t_y1=0;
	float t_y2=0;
	float t_x3=0;
	float t_x4=0;
	float t_y3=0;
	float t_y4=0;

	float ts=0;

	float rot;

	float sx=thumbnailSize;
	float sy=thumbnailSize*((float)3/(float)4);

	int id=0;

	#ifdef VISIBLE_SET
		for(int i=0; i<visibleSet.size(); i++){
	#else
		for(int i=0; i<thumbs.size(); i++){
	#endif
		
	

	px=pos.x+(col*spacing)+shim;
	py=pos.y-row*(thumbnailSize+20);

	#ifdef VISIBLE_SET
	visibleSet[i]->pos.x=px;
	visibleSet[i]->pos.y=py;
	rot=visibleSet[i]->rotate;
	#else
	thumbs[i].pos.x=px;
	thumbs[i].pos.y=py;
	rot=thumbs[i].rotate;
	#endif

	sx=thumbnailSize;
	sy=thumbnailSize*((float)3/(float)4);

	if(rot>180){
		while(rot>180){
			rot-=90;
		}

	}else if(rot<-180){
		while(rot<-180){
			rot+=90;
		}
	}

	if(rot==0){

		t_x1=0;
		t_y1=0+visibleSet[i]->offsetTL.y;

		t_x2=1;
		t_y2=0+visibleSet[i]->offsetTR.y;

		t_x3=1;
		t_y3=-1;

		t_x4=0;
		t_y4=-1;
	}else if(rot==90){

		t_x1=0;
		t_y1=-1;

		t_x2=0;
		t_y2=0+visibleSet[i]->offsetTL.y;

		t_x3=1;
		t_y3=0+visibleSet[i]->offsetTL.y;

		t_x4=1;
		t_y4=-1;

		ts=sx;
		sx=sy;
		sy=ts;

	}else if(rot==-90){

		t_x1=1;
		t_y1=0+visibleSet[i]->offsetTL.y;

		t_x2=1;
		t_y2=-1;

		t_x3=0;
		t_y3=-1;

		t_x4=0;
		t_y4=0+visibleSet[i]->offsetTL.y;

		ts=sx;
		sx=sy;
		sy=ts;

	}else if(rot==180){

		t_x1=0;
		t_y1=-1;

		t_x2=1;
		t_y2=-1;

		t_x3=1;
		t_y3=0+visibleSet[i]->offsetTR.y;

		t_x4=0;
		t_y4=0+visibleSet[i]->offsetTR.y;
	}else if(rot==-180){

		t_x1=1;
		t_y1=-1;

		t_x2=0;
		t_y2=-1;

		t_x3=0;
		t_y3=0+visibleSet[i]->offsetTR.y;

		t_x4=1;
		t_y4=0+visibleSet[i]->offsetTR.y;

	}else{
		t_x1=0+visibleSet[i]->offsetTR.y;
		t_y1=0;

		t_x2=1;
		t_y2=0;

		t_x3=1;
		t_y3=-1;

		t_x4=0+visibleSet[i]->offsetTR.y;
		t_y4=-1;
	}




	#ifdef VISIBLE_SET
		visibleSet[i]->scale.x=sx;
		visibleSet[i]->scale.y=sy;

		if(px<conf->sizeX && py> -conf->sizeY && !visibleSet[i]->loaded && (py-sy)<0){
			load(visibleSet[i],visibleSet[i]->path);
			visibleSet[i]->loaded=true;
			visibleSet[i]->offsetTL=texture.info(visibleSet[i]->thumbTex).offsetTL;
			visibleSet[i]->offsetTR=texture.info(visibleSet[i]->thumbTex).offsetTR;
			visibleSet[i]->offsetBL=texture.info(visibleSet[i]->thumbTex).offsetBL;

		}

		glBindTexture(GL_TEXTURE_2D,visibleSet[i]->thumbTex);
	#else
		thumbs[i].scale.x=sx;
		thumbs[i].scale.y=sy;

		if(px<conf->sizeX && py> -conf->sizeY && !thumbs[i].loaded){
			load(&thumbs[i],thumbs[i].path);
			thumbs[i].loaded=true;
		}

		glBindTexture(GL_TEXTURE_2D,thumbs[i].thumbTex);
	#endif

			if(px<conf->sizeX && py> -conf->sizeY  && (py-sy)<0){

				currentlyVisibleThumbs.pushBack(visibleSet[i]);

				id=remap[visibleSet[i]->path];

				glBegin(GL_QUADS);
					glTexCoord2d(t_x1,t_y1); glVertex3f(px		,py,pz);
					glTexCoord2d(t_x2,t_y2); glVertex3f(px+sx	,py,pz);
					glTexCoord2d(t_x3,t_y3); glVertex3f(px+sx	,py-sy,pz);
					glTexCoord2d(t_x4,t_y4); glVertex3f(px		,py-sy,pz);
				glEnd();
			}
		

		col++;

		if(col>thumbsPerLine){
			col=0;
			row++;
		}
	}
}

int glListName=-1;
bool dl=true;

void Thumbnails::draw(){

	

	if(!visible){return;}

#ifdef DISPLAY_LIST

	if(pos!=lastPos){
		dirty=true;
		lastPos=pos;
	}

	if(scale!=lastScale){
		dirty=true;
		lastScale=scale;
	}

	if(thumbnailSize!=lastThumbnailSize){
		dirty=true;
		lastThumbnailSize=thumbnailSize;
	}

	if(dirty){

		if(glListName==-1){
			glDeleteLists(glListName,1);
			glListName=glGenLists(1);
		}
		
		glNewList(glListName,GL_COMPILE);	
		doDraw();

		glEndList();

		dirty=false;
	}

	if(dl){
		glCallList(glListName);
	}
#else
	doDraw();
#endif

}

FloatVector2d Thumbnails::thumbPos(String s){
	int i=remap[s];

	return FloatVector2d(thumbs[i].pos.x,thumbs[i].pos.y);
}

FloatVector2d Thumbnails::thumbScale(String s){
	int i=remap[s];

	return FloatVector2d(thumbs[i].scale.x,thumbs[i].scale.y);
}


void Thumbnails::storeVisibleSet(){
	storedVisibleSet=visibleSet;
}

void Thumbnails::restoreVisibleSet(){
	visibleSet=storedVisibleSet;
}


bool Thumbnails::rotate(String s,float degrees){



	dirty=true;

	int i=remap[s];

	if(thumbs.size()>0){
		thumbs[i].rotate=degrees;
	}

	return true;
}

int luaThumbnailsAdd(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("Thumbnail Add Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("thumbnail add error: bad arguments");
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("thumbnail add error: bad arguments");
	}

	Thumbnails* t=level->thumbnailsKey[name];

	t->add(file);

	return 0;
}



int luaThumbnailsClear(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);



	if(args.size()!=1){
		console().write("thumbnail clear Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("thumbnail clear error: bad arguments");
	}

	Thumbnails* t=level->thumbnailsKey[name];

	t->clear();

	return 0;
}

int luaThumbnailsCurrentlyVisible(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);



	if(args.size()!=1){
		console().write("thumbnail clear Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("thumbnail clear error: bad arguments");
	}

	Thumbnails* t=level->thumbnailsKey[name];

	Array<Thumb*> l=t->currentlyVisible();

	script().resultTable(L);

	for(int i=0; i<l.size(); i++){
		script().result(L,i+1,l[i]->path);
	}

	return 1;
}


int luaThumbnailsGetVisibleSet(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);



	if(args.size()!=1){
		console().write("thumbnail getvisibleset Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("thumbnail getvisibleset error: bad arguments");
	}

	Thumbnails* t=level->thumbnailsKey[name];

	Array<Thumb*> l=t->getVisibleSet();

	script().resultTable(L);

	for(int i=0; i<l.size(); i++){
		script().result(L,i+1,l[i]->path);
	}

	return 1;
}



int luaThumbnailsStoreVisibleSet(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);



	if(args.size()!=1){
		console().write("thumbnail visibleset Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("thumbnail visibleset error: bad arguments");
	}

	Thumbnails* t=level->thumbnailsKey[name];

	t->storeVisibleSet();

	return 0;
}


int luaThumbnailsRestoreVisibleSet(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=1){
		console().write("thumbnail restorevisibleset Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("thumbnail restorevisibleset error: bad arguments");
	}


	Thumbnails* t=level->thumbnailsKey[name];

	t->restoreVisibleSet();

	return 0;
}

int luaThumbnailsThumbPos(lua_State *L){
	
	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("Thumbnail thumbpos Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}

	}else{
		console().write("Thumbnail thumbpos error: bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("Thumbnail thumbpos error: bad arguments");
		return 0;
	}


	Thumbnails* t=level->thumbnailsKey[name];

	FloatVector2d r=t->thumbPos(file);

	script().resultTable(L);
	script().result(L,"x",r.x);
	script().result(L,"y",r.y);
	return 1;
}


int luaThumbnailsThumbScale(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=2){
		console().write("Thumbnail thumbscale Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Thumbnail thumbscale error: bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("Thumbnail thumbscale error: bad arguments");
		return 0;
	}

	Thumbnails* t=level->thumbnailsKey[name];

	FloatVector2d r=t->thumbScale(file);

	script().resultTable(L);
	script().result(L,"x",r.x);
	script().result(L,"y",r.y);

	return 1;
}




int luaThumbnailsIsThumbVisible(lua_State *L){
	
	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("IsThumbVisible Error: Bad arguments, should be accessed as a class");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Thumbnail thumbvisible error: bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("Thumbnail thumbvisible error: bad arguments");
		return 0;
	}

	Thumbnails* t=level->thumbnailsKey[name];

	bool r=t->isThumbVisible(file);

	if(r){
		script().result(L,true);
	}else{
		script().result(L,false);
	}
	return 1;
}


int luaThumbnailsRotate(lua_State *L){

	//get panel name from script
	String name;
	String file;
	double rot=0;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=3){
		console().write("Thumbnail rotate Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}
	}else{
		console().write("Thumbnail rotate error: bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("Thumbnail rotate error: bad arguments");
		return 0;
	}

	if(args[2].type==SCRIPT_DOUBLE_STORED){

		rot=args[2].value.ds;

	}else{
		console().write("Thumbnail rotate error: bad arguments");
		return 0;
	}

	Thumbnails* t=level->thumbnailsKey[name];

	t->rotate(file,rot);

	return 0;
}



void Thumbnails::setMouseover(int i){
	mouseoverThumbnail=thumbs[i].path;

}

bool Thumbnails::isThumbVisible(String name){

	for(int i=0; i<visibleSet.size(); i++){
		if(visibleSet[i]=&thumbs[i]){
			return true;
		}
	}

	return false;
}

Thumbnails::Thumbnails(){
	thumbnailSize=64;
	scale.x=500;
	scale.y=500;
	picsPerRow=0;
	mouseover=false;
	mouseoverThumbnail="";
	visible=true;
	dirty=true;
}