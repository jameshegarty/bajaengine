#include "Text.hpp"
#include "Log.hpp"
#include "conf.h"
#include "Bmp.hpp"

#ifdef _WIN32
	#include <windows.h>
#endif

#include "GlHeader.hpp"

#include "endian.hpp"
#include <iostream>

#include "Gl/Extensions.h"

#include "os.h"

using namespace std;

Text text;

///////////////////////////////////////////////////////////////////////////////////

int Text::leading(FontId id){
	if(id==-1){id=0;}
	return int(fonts[id].leading*fonts[id].h);
}


void Text::draw2d(FontId id, float x, float y, String text){
	if(id<fonts.size()){
		fonts[id].print(x, y,0, text.c_str());
	}
}

void Text::draw2d(FontId id, float x, float y, float z,String text){
	if(id<fonts.size()){
		fonts[id].print(x, y,z, text.c_str());
	}
}	
void Text::draw2dRescale(FontId id, float x, float y, float z, String text,FloatVector2d rescale){
	
	if(id<fonts.size()){

		FloatVector2d temp=fonts[id].rescaleFactor;
		fonts[id].rescaleFactor=rescale;
		fonts[id].print(x, y, z, text.c_str());
		fonts[id].rescaleFactor=temp;
	}
	

}

void Text::draw2dRescale(FontId id, float x, float y, String text,FloatVector2d rescale){
	draw2dRescale(id,x,y,0,text,rescale);
}

void Text::draw2d(FontId id, TextPos pos, float x, float y, String text){
	
	if(id<fonts.size()){
	
		if(TEXT_POS_CENTER==pos){
			FloatVector2d s=getDisplacement(id,text);
			y-=s.y/2;
		}else if(TEXT_POS_CL==pos){
			
		}
		
		fonts[id].print(x, y, text.c_str());
	}
}

void Text::draw2d(float x, float y, String text){
	
	if(fonts.size()>0){
	
		draw2d(0,x,y,text);
	}
}

FloatVector2d Text::getSize(FontId id, String text){
	FloatVector3d temp;

	if(id>=fonts.size()){
		console().write("ERROR: trying to get size of a font that doesn't exist");
		console().write("FontID: "+String(id));
		console().write("Text: "+text);
		return temp;
	}

	return fonts[id].getSize(text);
}

FloatVector2d Text::getSizeSmart(FontId id, String text){

	return fonts[id].getSizeSmart(text);
}

FloatVector2d Text::getDisplacement(FontId id, String text){

	return fonts[id].getDisplacement(text);
}


bool Text::init(){

#ifdef DEVELOPER
	#ifndef HEADLESS
	fonts.pushBack();

	if(defaultFont.set()){
		fonts[0].file=defaultFont;
		fonts[0].name="developer";
	} 

	bool i=fonts[0].init();

	if(!i){
		logs().text.write("default font load error");
		return false;
	}

	#ifdef DEVELOPER
		uploadFont(0);
	#endif

	#endif

	return true;
#else
	return true;
#endif

}

Font* Text::getFont(FontId id){
	return &fonts[id];
}


Text::Text(){
	defaultFont.setRelative("developer.lmf");

	if(os().fileExists(String("../tools/developer.lmf"))){
		defaultFont.setRelative("../tools/developer.lmf");
	}else if(os().fileExists(String("../../tools/developer.lmf"))){
		defaultFont.setRelative("../../tools/developer.lmf");
	}else{
#ifdef DEVELOPER
		if(!os().fileExists(defaultFont)){
			os().error("Error: developer.lmf not found!");
		}
#endif
	}

}

Text::~Text(){
	for(int i=0; i<fonts.size(); i++){
		fonts[i].clean();
	}
}

bool Text::setDefaultFont(String font){

	defaultFont=font;
	return true;
}

int luaFontLoad(lua_State *L){
	String name=luaL_checkstring(L,1);
	String file=luaL_checkstring(L,2);
	
	FontId fid=text.addFont(file);
	text.luaFonts[name]=fid;

	text.fonts[fid].name=name;

	text.uploadFont(text.fonts.size()-1);

	return 0;
}

int luaFontDefault(lua_State *L){
	String name=luaL_checkstring(L,1);

	text.setLuaDefaultFont(text.luaFonts[name]);

	return 0;
}

int luaTextGetSize(lua_State *L){
	String f=luaL_checkstring(L,1);
	String textt=luaL_checkstring(L,2);

	FontId id;

	if(!text.luaFonts.find(f)){
		console().write("getSize error: font '"+f+"' not found.");
		return 0;
	}else{
		id=text.luaFonts[f];	
	}

	FloatVector2d lol=text.getSize(id,textt);

	script().resultTable(L);
		script().result(L,"x",lol.x);
		script().result(L,"y",lol.y);
	
	return 1;
}


int luaTextGetDisplacement(lua_State *L){

	String f=luaL_checkstring(L,1);
	String textt=luaL_checkstring(L,2);

	FontId id;

	if(!text.luaFonts.find(f)){
		console().write("getSize error: font '"+f+"' not found.");
		return 0;
	}else{
		id=text.luaFonts[f];	
	}

	FloatVector2d lol=text.getDisplacement(id,textt);

	script().resultTable(L);
		script().result(L,"x",lol.x);
		script().result(L,"y",lol.y);

	return 1;
}


int luaTextGetSizeSmart(lua_State *L){
	
	String f=luaL_checkstring(L,1);
	String textt=luaL_checkstring(L,2);

	FontId id;

	if(!text.luaFonts.find(f)){
		console().write("getSize error: font '"+f+"' not found.");
		return 0;
	}else{
		id=text.luaFonts[f];	
	}

	FloatVector2d lol=text.getSizeSmart(id,textt);

	script().resultTable(L);
		script().result(L,"x",lol.x);
		script().result(L,"y",lol.y);

	return 1;
}

void Text::uploadFont(FontId id){

	LuaType* m=script().add(fonts[id].name,fontObj);
		if(fonts[id].type!=FONT_TYPE_BITMAP){
			script().add("size",&fonts[id].size,m);
		}

		script().add("leading",&fonts[id].leading,m);
		script().add("spacing",&fonts[id].spacing,m);
}

FontId Text::addFont(Path f){
	Font nf;

	nf.file=f;

	fonts.pushBack(nf);

	fonts[fonts.size()-1].init();

	return fonts.size()-1;
}

bool Text::setLuaDefaultFont(FontId f){

	luaDefaultFont=f;
	return true;
}

FontId Text::getLuaDefaultFont(){

	return luaDefaultFont;
}

bool Text::scriptInit(){
	fontObj=script().add("font");

	LuaType* t=script().add("text");
		script().add("loadFont",&luaFontLoad,t);
		script().add("defaultFont",&luaFontDefault,t);


		script().add("getDisplacement",&luaTextGetDisplacement,t);
		script().add("getSize",&luaTextGetSize,t);
		script().add("getSizeSmart",&luaTextGetSizeSmart,t);

	return true;

}
