#include "videopanel.h"

#include "GlHeader.hpp"

#include "level.hpp"
#include "texture.hpp"
#include "picking.h"
#include <math.h>
#include "conf.h"
#include "shader.hpp"

unsigned char* blank;

FloatVector4d VideoPanel::shaderParameter(int id){
	if(id<1){id=1;}

	while(id>shaderParameters.size()){
		shaderParameters.pushBack(0);
	}

	return shaderParameters[id-1];

}

bool VideoPanel::shaderParameter(int id, FloatVector4d value){

	if(id<1){id=1;}

	while(id>shaderParameters.size()){
		shaderParameters.pushBack(0);
	}

	shaderParameters[id-1]=value;

	return true;
}


bool VideoPanel::loadShader(String code){

	if(code==""){
		return SHADER_NONE;
	}

	if(!remapShaders.find(code)){
		shaderId=shader.addFragment(code);


		if(shaderId==SHADER_NONE){
			

			console().write("error loading shader");
			return false;
		}

		remapShaders[code]=shaderId;
	}else{
		shaderId=remapShaders[code];
	}

	return true;
}

void VideoPanel::autoplay(int layer,bool autoplay){

	if(layer<1){return;}
	Bool b;
	b.b=autoplay;

	while(layer>layersAutoplay.size()){
		Bool l;
		l.b=true;
		layersAutoplay.pushBack(l);
	}

	while(layer>layers.size()){
		layers.pushBack("");
	}

	layersAutoplay[layer-1]=b;

	if(remap.find(layers[layer-1])){
		if(remap[layers[layer-1]]!=NULL){
			remap[layers[layer-1]]->autoPlay=b.b;
		}
	}

	return;
}

void VideoPanel::frame(int layer,float frame){

	if(layer<1){return;}
	while(layer>layersFrame.size()){
		
		layersFrame.pushBack(0);
	}

	while(layer>layers.size()){
		layers.pushBack("");
	}

	layersFrame[layer-1]=frame;

	if(remap.find(layers[layer-1])){
		if(remap[layers[layer-1]]!=NULL){
			remap[layers[layer-1]]->floatFrame=frame;
			remap[layers[layer-1]]->frame=frame;
		}
	}

	return;
}

float VideoPanel::frame(int layer){

	if(layer<1){return 0;}
	
	while(layer>layers.size()){
		layers.pushBack("");
	}

	if(remap.find(layers[layer-1])){
		if(remap[layers[layer-1]]!=NULL){
			return remap[layers[layer-1]]->floatFrame;
		}else{
			return 0;
		}

	}else{
		return 0;
	}
	
}

void VideoPanel::fps(int layer,float speed){

	if(layer<1){return;}

	while(layer>layersSpeed.size()){
		
		layersSpeed.pushBack(1);
	}

	while(layer>layers.size()){
		layers.pushBack("");
	}

	layersSpeed[layer-1]=speed;

	if(remap.find(layers[layer-1])){
		if(remap[layers[layer-1]]!=NULL){
			remap[layers[layer-1]]->fps=speed;
		}
	}

	return;
}

float VideoPanel::fps(int layer){

	if(layer<1){return 0;}

	while(layer>layers.size()){
		layers.pushBack("");
	}

	if(remap.find(layers[layer-1])){
		if(remap[layers[layer-1]]!=NULL){
			return remap[layers[layer-1]]->fps;
		}else{
			return 0;
		}

	}else{
		return 0;
	}

}

#define VID_SOFTWARE_RESIZE

void VideoPanel::draw(){

	if(!visible){
		return;
	}

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
	glDisable(GL_TEXTURE_2D);

	glColor4f(color.x,color.y,color.z,color.w);


	if(shaderId==SHADER_NONE){
		shader.reset();
	}else{
		shader.set(shaderId);

		for(int lol=0; lol<shaderParameters.size(); lol++){
			shader.localParameterFragment(lol,shaderParameters[lol]);
		}
	}

	VideoSource* vs;

	float px,py,pz,sx,sy;

	pz=pos.z;

	if(percent){
		px=pos.x*conf->sizeX;
		py=pos.y*conf->sizeY;
		sx=scale.x*conf->sizeX;
		sy=scale.y*conf->sizeY;
	}else{
		px=pos.x;
		py=pos.y;
		sx=scale.x;
		sy=scale.y;
	}

	for(int i=0; i<layers.size(); i++){

		if(layers[i].substr(0,8)=="picture:"){
			GLuint texid=remapPictures[layers[i]];

			if(i==0){
				glActiveTextureARB(GL_TEXTURE0_ARB);
			}else if(i==1){
				glActiveTextureARB(GL_TEXTURE1_ARB);
			}else if(i==2){
				glActiveTextureARB(GL_TEXTURE2_ARB);
			}else if(i==3){
				glActiveTextureARB(GL_TEXTURE3_ARB);
			}



			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,texid);

		}else{

			vs=remap[layers[i]];

			if(vs!=NULL){
				if(i==0){
					glActiveTextureARB(GL_TEXTURE0_ARB);
				}else if(i==1){
					glActiveTextureARB(GL_TEXTURE1_ARB);
				}else if(i==2){
					glActiveTextureARB(GL_TEXTURE2_ARB);
				}else if(i==3){
					glActiveTextureARB(GL_TEXTURE3_ARB);
				}

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,vs->texid);

				vs->getFrame();

				#ifdef VID_SOFTWARE_RESIZE
				glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, vs->widthp2, vs->heightp2, GL_BGR_EXT, GL_UNSIGNED_BYTE, vs->data);
				#else
				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, vs->width, vs->height, GL_BGR_EXT , GL_UNSIGNED_BYTE, vs->data);
				#endif
			}
		}
	
	}

	float tx=1;

	glBegin(GL_QUADS);
		glTexCoord2d(0,0); glVertex3f(px		,py,pz);
		glTexCoord2d(tx,0); glVertex3f(px+sx	,py,pz);
		glTexCoord2d(tx,-tx); glVertex3f(px+sx	,py-sy,pz);
		glTexCoord2d(0,-tx); glVertex3f(px		,py-sy,pz);
	glEnd();

}

bool VideoPanel::load(int layer, String source){
	//VideoSource
	
	while(layer>layers.size()){
		layers.pushBack("");
	}

	if(source==""){
		layers[layer-1]="";
		return true;

	}

	layers[layer-1]=source;

	if(source.substr(0,7)=="camera:"){
		VideoSource* vs;

		if(!remap.find(source)){
			remap[source]=new VideoSource;
			vs=remap[source];
		}else{
			return true;
		}

		short driver=0;

		if(source.substr(7,-1)=="1"){
			driver=0;
		}else if(source.substr(7,-1)=="2"){
			driver=1;
		}


		#ifdef VID_SOFTWARE_RESIZE
		vs->powerOf2=true;
		#endif

		if(!vs->sourceCapture(driver)){

			console().write("error, could not access camera");
			return false;
		}else{
			//console().write("successfully connected to camera");
		}

		glGenTextures(1, (GLuint*)&vs->texid);
		glBindTexture(GL_TEXTURE_2D,vs->texid);
		
		vs->getFrame();


		#ifdef VID_SOFTWARE_RESIZE
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vs->widthp2,vs->heightp2, 0,  GL_BGR_EXT, GL_UNSIGNED_BYTE, blank);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		#else
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, vs->width,vs->height, GL_BGR_EXT , GL_UNSIGNED_BYTE, vs->data);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		#endif


		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	}else if(source.substr(0,5)=="file:"){
		VideoSource* vs;

		if(!remap.find(source)){
			vs=new VideoSource;
			remap[source]=vs;
		}else{
			return true;
		}


		#ifdef VID_SOFTWARE_RESIZE
		vs->powerOf2=true;
		#endif

		String x=source.substr(5,-1);

		if(  !vs->sourceAvi(x.c_str())  ){
			console().write("error loading video");
			return false;
		}else{
			//console().write("video loaded successfully");
		}

		

		glGenTextures(1, (GLuint*)&vs->texid);
		glBindTexture(GL_TEXTURE_2D,vs->texid);
		
		vs->getFrame();

		#ifdef VID_SOFTWARE_RESIZE
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, vs->widthp2,vs->heightp2, 0,  GL_BGR_EXT, GL_UNSIGNED_BYTE, blank);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		#else
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, remap[source]->width, remap[source]->height, GL_BGR_EXT , GL_UNSIGNED_BYTE, remap[source]->data);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		#endif

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	}else if(source.substr(0,8)=="picture:"){
		String file=source.substr(8,-1);

		int g=texture.load(file);

		ImageData im=texture.info(g);

		if(g!=-1){

			remapPictures[source]=g;

			console().write("picture '"+file+"', loaded successfully");
		}else{

			console().write("Error loading file '"+file+"', "+im.error);
			return false;
		}
		
	}else{

		console().write("error, unknown video source type '"+source+"', should be file:, picture: or camera:");
		return false;
	}


	return true;

}



int luaVideoPanelLoad(lua_State *L){

	//get panel name from script
	String name;
	int layer=1;
	String file;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=3){
		console().write("VideoPanel::load Error: Bad number of arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{

		console().write("VideoPanel::load error: bad arguments, should be accessed as a class");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){

		layer=args[1].value.ds;

	}else{
		console().write("VideoPanel::load error: bad arguments, first argument should be a number");
		return 0;
	}

	if(args[2].type==SCRIPT_STRING_STORED){

		file=args[2].value.ss;

	}else{
		console().write("VideoPanel::load error: bad arguments, second argument should be a string");
		return 0;
	}

	VideoPanel* t=level->videopanelKey[name];

	t->load(layer,file);

	return 0;
}


int luaVideoPanelShader(lua_State *L){

	//get panel name from script
	String name;
	String code;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("VideoPanel::shader Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("VideoPanel::shader error: bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		code=args[1].value.ss;

	}else{
		console().write("VideoPanel::shader error: bad arguments");
		return 0;
	}

	VideoPanel* t=level->videopanelKey[name];

	t->loadShader(code);

	return 1;
}


int luaVideoPanelAutoplay(lua_State *L){

	//get panel name from script
	String name;
	int layer=1;
	bool ap;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=3){
		console().write("VideoPanel::autoplay Error: Bad number of arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("VideoPanel::autoplay error: bad arguments, should be accessed as a class");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){

		layer=args[1].value.ds;

	}else{
		console().write("VideoPanel::autoplay error: bad arguments, first argument should be a number");
		return 0;
	}

	if(args[2].type==SCRIPT_BOOL_STORED){

		ap=args[2].value.bs;

	}else{
		console().write("VideoPanel::autoplay error: bad arguments, second argument should be a string");
		return 0;
	}

	VideoPanel* t=level->videopanelKey[name];

	t->autoplay(layer,ap);

	return 0;
}




int luaVideoPanelShaderParameter(lua_State *L){

	//get panel name from script
	String name;
	int layer=1;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=6 && args.size()!=3  && args.size()!=2){
		console().write("VideoPanel::shaderparameter Error: Bad number of arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("VideoPanel::shaderparameter error: bad arguments, should be accessed as a class");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){

		layer=args[1].value.ds;

	}else{
		console().write("VideoPanel::shaderparameter error: bad arguments, first argument should be a number");
		return 0;
	}

	if(args.size()==3){
		
	}else if(args.size()==6){
		FloatVector4d final;

		if(args[2].type==SCRIPT_DOUBLE_STORED){

			final.x=args[2].value.ds;

		}else{
			console().write("VideoPanel::shaderparameter error: bad arguments, second argument should be a double");
			return 0;
		}

		if(args[3].type==SCRIPT_DOUBLE_STORED){

			final.y=args[3].value.ds;

		}else{
			console().write("VideoPanel::shaderparameter error: bad arguments, second argument should be a double");
			return 0;
		}

		if(args[4].type==SCRIPT_DOUBLE_STORED){

			final.z=args[4].value.ds;

		}else{
			console().write("VideoPanel::shaderparameter error: bad arguments, second argument should be a double");
			return 0;
		}

		if(args[5].type==SCRIPT_DOUBLE_STORED){

			final.w=args[5].value.ds;

		}else{
			console().write("VideoPanel::shaderparameter error: bad arguments, second argument should be a double");
			return 0;
		}

		VideoPanel* t=level->videopanelKey[name];

		t->shaderParameter(layer,final);
	}else{

		VideoPanel* t=level->videopanelKey[name];

		script().resultTable(L);
		script().result(L,"x",t->shaderParameter(layer).x);
		script().result(L,"y",t->shaderParameter(layer).y);
		script().result(L,"z",t->shaderParameter(layer).z);
		script().result(L,"w",t->shaderParameter(layer).w);
		return 1;

	}

	return 0;
}

int luaVideoPanelFrame(lua_State *L){

	//get panel name from script
	String name;
	int layer=1;
	double frame;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=3  && args.size()!=2){
		console().write("VideoPanel::frame Error: Bad number of arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{

		console().write("VideoPanel::frame error: bad arguments, should be accessed as a class");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){

		layer=args[1].value.ds;

	}else{
		console().write("VideoPanel::frame error: bad arguments, first argument should be a number");
		return 0;
	}

	if(args.size()==3){
		if(args[2].type==SCRIPT_DOUBLE_STORED){

			frame=args[2].value.ds;

		}else{
			console().write("VideoPanel::frame error: bad arguments, second argument should be a string");
			return 0;
		}

		VideoPanel* t=level->videopanelKey[name];

		t->frame(layer,frame);
	}else{
		VideoPanel* t=level->videopanelKey[name];

		script().result(L,t->frame(layer));
		return 1;

	}

	return 0;
}



int luaVideoPanelFps(lua_State *L){

	//get panel name from script
	String name;
	int layer=1;
	double speed;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=3  && args.size()!=2){
		console().write("VideoPanel::fps Error: Bad number of arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("VideoPanel::fps error: bad arguments, should be accessed as a class");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){

		layer=args[1].value.ds;

	}else{
		console().write("VideoPanel::fps error: bad arguments, first argument should be a number");
		return 0;
	}

	if(args.size()==3){
		if(args[2].type==SCRIPT_DOUBLE_STORED){

			speed=args[2].value.ds;

		}else{
			console().write("VideoPanel::fps error: bad arguments, second argument should be a string");
			return 0;
		}

		VideoPanel* t=level->videopanelKey[name];

		t->fps(layer,speed);

		return 0;
	}else{
		VideoPanel* t=level->videopanelKey[name];

		script().result(L,t->fps(layer));

		return 1;
	}

}

VideoPanel::VideoPanel(){
	pos.x=0;
	pos.y=0;
	scale.x=1;
	scale.y=1;
	mouseover=false;
	percent=true;

	visible=true;

	color.x=1;
	color.y=1;
	color.z=1;
	color.w=1;

	shaderId=-1;

	blank=new unsigned char[1024*512*3];
}