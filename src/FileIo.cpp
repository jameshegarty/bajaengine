#include "FileIo.hpp"
#include "Bmp.hpp"
#include "Tga.hpp"
#include "jpg.hpp"
#include "script.hpp"
#include "Helperlib/HelperLibMath.hpp"
#include "leaks.h"
#include "level.hpp"

Array<ImageData> loadedImages;

ImageData::ImageData(){
	loadSuccess=false;
	width=0;
	height=0;
	error="";
}

ImageData::~ImageData(){

}

ImageData FileLoader::load(Path file){
	ImageData temp;
	return temp;
}

bool FileLoader::save(Path file,ImageData data){
	return false;
}

String getFileType(String input,char delim){
	String f="";

	int end=input.size();
	int start=input.size();

	for(int i=input.size()-1; i>0; i--){
		if(input[i]==delim){
			break;
		}else{
			start--;
		}
	}

	input=input.substr(start,end);
	input=input.toLower();

	return input;
}

ImageData loadImage(Path file){
	return loadImage(file,"");
}

#ifndef XSICONVERT
FloatVector4d getImageValue(String name,float x,float y){
	FloatVector4d final;

	int i=0;

	for(i=0; i<loadedImages.size(); i++){
		if(loadedImages[i].name==name){
			break;
		}
	}

	if(i==loadedImages.size()){
		console().write("getimagevalueerror");
		return final;
	}

	ImageData* im=&loadedImages[i];

	if(im->bytesPP==3){
		final.w=1;
	}

	if(x<0 || x>=im->width || y<0 || y>=im->height){
		console().write("get image error: out of bounds x:"+String(x)+" y:"+String(y)+" on image:"+im->name);
		return final;
	}

	if(math.floor(x)==x && math.floor(y)==y){
		final.x=im->data[int(y)*im->width*im->bytesPP+int(x)*im->bytesPP];
		final.y=im->data[int(y)*im->width*im->bytesPP+int(x)*im->bytesPP+1];
		final.z=im->data[int(y)*im->width*im->bytesPP+int(x)*im->bytesPP+2];
		final.x/=255;
		final.y/=255;
		final.z/=255;

		if(im->bytesPP==4){
			final.w=im->data[int(y)*im->width*im->bytesPP+int(x)+3];
			final.w/=255;
		}
	}
	
	return final;
}
#endif

#ifndef XSICONVERT

int luaGetImageValue(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=3){
		console().write("getimagevalue bad args");
		return 0;
	}

	String name;
	double x;
	double y;

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("getimagevalue bad args");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){
		x=args[1].value.ds;
	}else{
		console().write("getimagevalue bad args");
		return 0;
	}

	if(args[2].type==SCRIPT_DOUBLE_STORED){
		y=args[2].value.ds;
	}else{
		console().write("getimagevalue bad args");
		return 0;
	}

	FloatVector4d res=getImageValue(name,x,y);
	
	script().resultTable(L);
		script().result(L,"r",res.x);
		script().result(L,"g",res.y);
		script().result(L,"b",res.z);
		script().result(L,"a",res.w);

	return 1;
}

int luaSetImageValue(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=6 && args.size()!=7){
		console().write("set value error, bad args");
		return 0;
	}

	String name;
	double x,y,r,g,b,a;
	r=0;
	g=0;
	b=0;
	a=0;

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){

				name=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("set value error, bad args");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){
		x=args[1].value.ds;
	}else{
		console().write("set value error, bad args");
		return 0;
	}


	if(args[2].type==SCRIPT_DOUBLE_STORED){
		y=args[2].value.ds;
	}else{
		console().write("set value error, bad args");
		return 0;
	}


	if(args[3].type==SCRIPT_DOUBLE_STORED){
		r=args[3].value.ds;
	}else{
		console().write("set value error, bad args");
		return 0;
	}


	if(args[4].type==SCRIPT_DOUBLE_STORED){
		g=args[4].value.ds;
	}else{
		console().write("set value error, bad args");
		return 0;
	}


	if(args[5].type==SCRIPT_DOUBLE_STORED){
		b=args[5].value.ds;
	}else{
		console().write("set value error, bad args");
		return 0;
	}

	if(args.size()==7){
		if(args[6].type==SCRIPT_DOUBLE_STORED){
			a=args[6].value.ds;
		}else{
			console().write("set value error, bad args");
			return 0;
		}
	}

	if(x!=math.floor(x) || y!=math.floor(y)){
		console().write("error, x and y must be integer values");
		return 0;
	}


	int i=0;

	for(i=0; i<loadedImages.size(); i++){
		if(loadedImages[i].name==name){
			break;
		}
	}

	if(i==loadedImages.size()){
		console().write("saveimageerror");
		return 0;
	}

	ImageData* im=&loadedImages[i];

	if(r<0){r=0;}
	if(g<0){g=0;}
	if(b<0){b=0;}
	if(a<0){a=0;}

	if(r>1){r=1;}
	if(g>1){g=1;}
	if(b>1){b=1;}
	if(a>1){a=1;}

	if(y>=im->height){
		console().write("Error, y>height");
		return 0;
	}
	
	if(y<0){
		console().write("Error, y<0");
		return 0;
	}
	
	if(x>=im->width){
		console().write("Error, x>width");
		return 0;
	}
	
	if(x<0){
		console().write("Error, x<0");
		return 0;
	}
	
	im->data[int(y)*im->width*im->bytesPP+int(x)*im->bytesPP]=r*255;
	im->data[int(y)*im->width*im->bytesPP+int(x)*im->bytesPP+1]=g*255;
	im->data[int(y)*im->width*im->bytesPP+int(x)*im->bytesPP+2]=b*255;

	if(args.size()==7){
		im->data[int(y)*im->width*im->bytesPP+int(x)*im->bytesPP+3]=a*255;
	}

	return 0;
}

int luaSaveImage(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("save error, bad args");
		return 0;
	}

	String name;
	String file;

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){

				name=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("save error, bad args");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		file=args[1].value.ss;
	}else{
		console().write("save error, bad args");
		return 0;
	}

	int i=0;

	for(i=0; i<loadedImages.size(); i++){
		if(loadedImages[i].name==name){
			break;
		}
	}

	if(i==loadedImages.size()){
		console().write("SAVE ERROR SOURCE NOT FOUND");
		return 0;
	}

	saveImage(file,loadedImages[i]);

	return 0;
}

#endif

ImageData loadImage(Path file, String name){
	ImageData t;

	String ext=file.getExtension();

	if(file.getExtension()=="bmp"){
		t=bmp.load(file);
	}else if(file.getExtension()=="tga"){
		t=tga.load(file);
	}else if(file.getExtension()=="jpg"){
		t=jpg.load(file);
	}else if(file.getExtension()=="jpeg"){
		t=jpg.load(file);
	}else{
		ImageData t;
		t.loadSuccess=false;
		t.error="unknown file type '"+file.getExtension()+"'";
		return t;
	}

	if(name==""){
		name=file.getFilenameNoExtension();
	}

#ifndef XSICONVERT
	LuaType* img=script().add(name,luaImage);
		script().add("name",name,img);
		LuaType* size=script().add("size",img);
			script().add("x",t.width,size);
			script().add("y",t.height,size);
		script().add("bytesPerPixel",t.bytesPP,img);
		script().add("getValue",luaGetImageValue,img);
		script().add("setValue",luaSetImageValue,img);
		script().add("save",luaSaveImage,img);
#endif

	t.name=name;
	t.file=file.getAbsolute();
#ifndef XSICONVERT
	t.fileRelative=file.getRelativeTo(level->file.getRelativeDirectory());
#endif
	t.dataLoaded=new bool;
	*t.dataLoaded=true;
	leaks.alloc("ImageData::dataLoaded");

	loadedImages.pushBack(t);

	return t;
}

bool saveImage(Path file,ImageData id){
	if(file.getExtension()=="bmp"){
		return bmp.save(file,id);
	}else if(file.getExtension()=="tga"){
		return tga.save(file,id);
	}else{
		ImageData t;
		t.loadSuccess=false;
		t.error="unknown file type '"+file.getExtension()+"'";
		return false;
	}

}

#include <iostream>


bool checkSize(int size){

	if(size==16){
		return true;
	}else if(size==32){
		return true;
	}else if(size==64){
		return true;
	}else if(size==128){
		return true;
	}else if(size==256){
		return true;
	}else if(size==512){
		return true;
	}else if(size==1024){
		return true;
	}else if(size==2048){
		return true;
	}

	return false;
}

LuaType* luaImage;