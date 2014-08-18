#include "imagetools.h"
#include "texture.hpp"
#include "FileIo.hpp"
#include "script.hpp"
#include "shader.hpp"
#include "conf.h"
#include "HelperLibMath.hpp"
#include "Log.hpp"

#include <math.h>

#include "GlHeader.hpp"

bool ImageTools::thumbnail(Path in, String out, int size){

	console().write("start thumbnail");

	int tex=texture.load(in);
	ImageData im=texture.info(tex);

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	shader.reset();

	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();

	glOrtho(0,1,0,-1,-100,100);

	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();

	glDisable(GL_LIGHTING);

	glDisable(GL_BLEND);

#ifdef LOOP_DEBUG
	logs().main.write("thumbnail texture DISABLE");
#endif

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

	glBindTexture(GL_TEXTURE_2D,tex);

	float w;
	float h;
	int fileW;
	int fileH;
	float z=0;

	if(im.width>im.height){
		w=(float)size;
		h=(float)size*((float)im.height/(float)im.width);
	}else{
		h=(float)size;
		w=(float)size*((float)im.width/(float)im.height);
	}


	fileW=w;
	fileH=h;

	ImageData image;

	image.width=fileW;
	image.height=fileH;

	unsigned char* dat=new unsigned char[fileW*fileH*3];


	w=w/conf->sizeX;
	h=h/conf->sizeY;

	glColor4f(1.0f,1.0f,1.0f,1.0f);

	glBegin(GL_QUADS);
			glTexCoord2d(0,0); glVertex3f(0		,-h,z);
			glTexCoord2d(1,0); glVertex3f(w	,-h,z);
			glTexCoord2d(1,-1); glVertex3f(w	,0,z);
			glTexCoord2d(0,-1); glVertex3f(0		,0,z);
		glEnd();


	console().write("thumbnail draw done");

	glReadPixels(0,0,fileW,fileH,GL_RGB,GL_UNSIGNED_BYTE,dat);
	
	image.data=dat;
	image.bitsPP=24;
	image.bytesPP=3;
	image.size=w*h*3;

	bool t=saveImage(out,image);

	console().write("thumbnail save done");

	return t;
}

void writeImageData(ImageData &im,String filenamr){
	FILE *file;

	file = fopen(filenamr.c_str(), "wb");
	fwrite(im.data, im.width*im.height*im.bytesPP, 1, file);
	fclose(file);
}

bool ImageTools::bmpAlphaToTga(FloatVector3d color, String bmpIn,String tgaOut){
	ImageData im=loadImage(bmpIn);

	if(!im.loadSuccess){
		console().write("error reading file "+bmpIn);
		return false;
	}

	if(im.bytesPP!=3){
		console().write("bad bit depth "+bmpIn);
		return false;
	}

	ImageData o;
	o.width=im.width;
	o.height=im.height;
	o.bytesPP=4;
	o.bitsPP=32;
	o.size=o.width*o.height*o.bytesPP;
	o.data=new unsigned char[o.width*o.height*o.bytesPP];

	for(int i=0; i<(o.width*o.height); i++){
		o.data[i*4+0]=color.x*255;
		o.data[i*4+1]=color.y*255;
		o.data[i*4+2]=color.z*255;

		o.data[i*4+3]=im.data[i*3];
	}

	bool t=saveImage(tgaOut,o);

	return t;

}
		
bool ImageTools::bmpColorAlphaToTga(String bmpColorIn ,String bmpAlphaIn, String tgaOut){

	return false;
}


static int luaThumbnail(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=3){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	if(args[1].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	if(args[2].type!=SCRIPT_DOUBLE_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}



	imageTools().thumbnail(args[0].value.ss,args[1].value.ss, args[2].value.ds);

	return 0;
}

static int luaBmpAToTga(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=5){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_DOUBLE_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	if(args[1].type!=SCRIPT_DOUBLE_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	if(args[2].type!=SCRIPT_DOUBLE_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}


	if(args[3].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	if(args[4].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	
	if(!imageTools().bmpAlphaToTga(FloatVector3d(args[0].value.ds, args[1].value.ds, args[2].value.ds), args[3].value.ss,args[4].value.ss)){
		console().write("error writing image");
	}

	return 0;
}


bool imageToolsScriptInit(){
	LuaType* w=script().add("imageTools");

	script().add("thumbnail",&luaThumbnail,w);
	script().add("bmpAToTga",&luaBmpAToTga,w);

	return true;
}

ImageTools& imageTools(){
	static ImageTools o;
	return o;
}
