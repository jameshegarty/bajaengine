#include "textpanel.h"

#ifdef _WIN32
#include <gl/gl.h>
#elif LINUX
#include <gl.h>
#else
#include <GLUT/glut.h>
#endif

#include "level.hpp"
#include "texture.hpp"
#include "conf.h"
#include "script.hpp"
#include "picking.h"
#include "timeing.hpp"

#include "Helperlib/Log.hpp"


int luaTextPanelScissor(lua_State *L){

	//get panel name from script
	String name;
	float x;
	float y;
	float z;
	float w;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=5){
		console().write("Panel Scissor Error: Bad arguments (should be accessed as a class and have 4 arguments)");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}

	}else{
		console().write("Panel Scissor Error: bad arguments (should be accessed as a class)");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){

		x=args[1].value.ds;

	}else{
		console().write("Panel error: bad arguments");
		return 0;
	}

	if(args[2].type==SCRIPT_DOUBLE_STORED){

		y=args[2].value.ds;

	}else{
		console().write("Panel error: bad arguments");
		return 0;
	}


	if(args[3].type==SCRIPT_DOUBLE_STORED){

		z=args[3].value.ds;

	}else{
		console().write("Panel error: bad arguments");
		return 0;
	}


	if(args[4].type==SCRIPT_DOUBLE_STORED){

		w=args[4].value.ds;

	}else{
		console().write("Panel error: bad arguments");
		return 0;
	}



	if(name==""){
		console().write("panel error (internal): no name given");
		lua_error(L);
		return 0;
	}

	TextPanel* t=level->textpanelKey[name];

	t->scissor(x,y,z,w);

	return 0;
}



int luaTextPanelFont(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=1 && args.size()!=2){
		console().write("TextPanel Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}

	}else{
		console().write("TextPanel error: bad arguments");
	}

	if(args.size()==1){
		TextPanel* t=level->textpanelKey[name];

		Array<String> k=text.luaFonts.indices();
		Array<FontId> v=text.luaFonts.values();
		for(int i=0; i<k.size(); i++){
			if(v[i]==t->font){
				script().result(L,k[i]);
				return 1;
			}
		}

		//script().result(L,t->font);
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("TextPanel error: bad arguments");
	}

	if(name==""){
		console().write("TextPanel error (internal): no name given");
		return 0;
	}

	TextPanel* t=level->textpanelKey[name];

	t->setFont(file);

	return 0;
}

int luaTextPanelText(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2 && args.size()!=1){
		console().write("TextPanel:text() Error: Bad arguments.  There should be 1 or 2 arguments, but there was "+String(args.size()));
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("TextPanel text() error: bad arguments, should be accessed as a class");
		return 0;
	}


	if(name==""){
		console().write("TextPanel error (internal): no name given");
		return 0;

	}
	TextPanel* t=level->textpanelKey[name];

	if(args.size()==2){
		if(args[1].type==SCRIPT_STRING_STORED){

			file=args[1].value.ss;
		}else if(args[1].type==SCRIPT_DOUBLE_STORED){

			file=String(args[1].value.ds);

		}else{
			console().write("TextPanel error: bad arguments, could not convert type");
		}

		t->setText(file);
	}else{
		script().result(L,t->storedtext);
		return 1;
	}

	return 0;
}



void TextPanel::scissor(float x, float y, float z, float w){
	scissorStore.x=x;
	scissorStore.y=y;
	scissorStore.z=z;
	scissorStore.w=w;
	scissorSet=true;
}


bool TextPanel::setFont(String ifont){


	if(!text.luaFonts.find(ifont)){
		console().write("Error, no font named '"+ifont+"'");
	}else{
		font=text.luaFonts[ifont];
	}

	return false;
}

bool TextPanel::setText(String it){
	if(text.fonts.size()==0){
		logs().main.write("Error, could not set text.  No fonts loaded");
		return false;
	}

	script().syncFrom(NULL);

	storedtext=it;

	FontId f=font;

	if(font==-1){
		f=text.getLuaDefaultFont();
	}

	float rx=1;
	float ry=1;

	if(pixelsize.x!=0 && pixelsize.y!=0  && percent){

		rx=(1/scale.x)*pixelsize.x;
		ry=(1/scale.y)*pixelsize.y;

	}

	pixelsize=text.getSize(f,storedtext);
	scale=text.getSize(f,storedtext);

	scale.x/=rx;
	scale.y/=ry;
	
	script().syncTo(NULL);

	return false;
}


void TextPanel::draw(){


	if(!visible){
		return;
	}

	if(color.w<=0){
		return;
	}

	if(text.fonts.size()==0){
		logs().main.write("Error, trying to draw a text panel when there are no loaded fonts.");
		return;
	}

	FontId f=font;

	if(font==-1){
		f=text.getLuaDefaultFont();
	}

	if(font>=text.fonts.size()){
		f=text.getLuaDefaultFont();
	}


	if(text.fonts[f].spacing!=oldFontSpacing){
		setText(storedtext);
		oldFontSpacing=text.fonts[f].spacing;
	}

	if(text.fonts[f].leading!=oldFontLeading){
		setText(storedtext);
		oldFontLeading=text.fonts[f].leading;
	}

	float zs=0;
	float ws=0;
	float xs=0;
	float ys=0;

	if(scissorSet){
		glEnable(GL_SCISSOR_TEST);

		zs=scissorStore.z;
		ws=scissorStore.w;

		if(zs<0){zs=0;}
		if(ws<0){ws=0;}
		
		xs=scissorStore.x;
		ys=scissorStore.y;
		
		if(smartCenter!=0 && percent){
			if( (conf->sizeX*(1/smartCenter)) > conf->sizeY){
				//if wider than tall
				xs=xs*conf->sizeY*smartCenter;
				ys=ys*conf->sizeY;

				zs=zs*conf->sizeY*smartCenter;
				ws=ws*conf->sizeY;

				xs+=(conf->sizeX-(conf->sizeY*smartCenter))/2;

			}else if( (conf->sizeY*(smartCenter)) > conf->sizeX){
				//if taller than wide

				xs=xs*conf->sizeX;
				ys=ys*conf->sizeX*(1/smartCenter);

				zs=zs*conf->sizeX;
				ws=ws*conf->sizeX*(1/smartCenter);

				float adj=(conf->sizeY-(conf->sizeX*(1/smartCenter)))/2;
				ys-=adj;
			}

			ys=conf->sizeY+ys-ws;
		}


		if(percent  && smartCenter==0){
			ys=+scissorStore.w;

			xs*=conf->sizeX;
			ys*=conf->sizeY;
			zs*=conf->sizeX;
			ws*=conf->sizeY;

			ys=conf->sizeY+scissorStore.y-ws;
		}

		glScissor(xs,ys,zs,ws);
	}

	float sx=scale.x;
	float sy=scale.y;
	
	float px=pos.x;
	float py=pos.y;
	float pz=pos.z;

		if(smartCenter!=0 && percent){

			if( (conf->sizeX*(1/smartCenter)) > conf->sizeY){
				//if wider than tall
				px=pos.x*conf->sizeY*smartCenter;
				py=pos.y*conf->sizeY;


				sx=scale.x*conf->sizeY*smartCenter;
				sy=scale.y*conf->sizeY;

				px+=(conf->sizeX-(conf->sizeY*smartCenter))/2;

			}else if( (conf->sizeY*(smartCenter)) > conf->sizeX){
				//if taller than wide

				px=pos.x*conf->sizeX;
				py=pos.y*conf->sizeX*(1/smartCenter);


				sx=scale.x*conf->sizeX;
				sy=scale.y*conf->sizeX*(1/smartCenter);

				float adj=(conf->sizeY-(conf->sizeX*(1/smartCenter)))/2;
				py-=adj;
		
			}else{
				px=pos.x*conf->sizeX;
				py=pos.y*conf->sizeY;

				sx=scale.x*conf->sizeX;
				sy=scale.y*conf->sizeY;
			}
	}

	if(percent  && smartCenter==0){
		sx=scale.x*conf->sizeX;
		sy=scale.y*conf->sizeY;

		px=pos.x*conf->sizeX;
		py=pos.y*conf->sizeY;
	}

	if(keepAspectRatio){

		textscale=text.getSize(f,storedtext);

		float ratio=textscale.x/textscale.y;

		float rx=textscale.x;
		float ry=textscale.y;

		if(sx>(sy*ratio)){

			if(rx>ry){
				sx=sy*ratio;
			}else{	//scale.y>scale.x -- taller than wider
				sy=sx*(1/ratio);
			}

		}else{ //sy>sx

			if(rx<ry){
				sx=sy*ratio;
			}else{	//scale.y>scale.x -- taller than wider
				sy=sx*(1/ratio);
			}


		}
	}

	if(center){
		px=(conf->sizeX/2)-(sx/2);
		py=(-conf->sizeY/2)-(-sy/2);
	}

	FloatVector2d rs=text.getSize(f,storedtext);	//not smart, we need the actual size on screen

	FloatVector2d fontRescaleFactor;
	fontRescaleFactor.x=sx/rs.x;
	fontRescaleFactor.y=sy/rs.y;

	glTranslatef(0,0,pos.z);
	glColor4f(color.x,color.y,color.z,color.w);

	if(align=="center"){
		FloatVector2d lol=text.getSizeSmart(f,storedtext);
		FloatVector2d lold=text.getSize(f,storedtext);
		FloatVector2d lolr=text.getDisplacement(f,storedtext);

		px-=lol.x*fontRescaleFactor.x*0.5;
		py+=lol.y*fontRescaleFactor.y*0.5;

		text.draw2dRescale(f,px,py,pz,storedtext,fontRescaleFactor);
	}else if(align=="right"){ 
		Array<std::string> lines;
		std::string buffer;
		
		for(int c=0; c<storedtext.size(); c++){
			if(storedtext[c]=='\n'){
				lines.pushBack(buffer);
				buffer="";
			}else{
				buffer+=storedtext[c];
			}
		}

		lines.pushBack(buffer);

		float y=0;

		for(int i=0; i<lines.size(); i++){
			FloatVector2d lold=text.getSize(f,lines[i]);
			text.draw2dRescale(f,px-lold.x,py-y,pz,lines[i],fontRescaleFactor);
			y+=text.leading(f)*fontRescaleFactor.y;
		}

		pickerPos.x=px-sx;
		pickerPos.y=py;
		pickerPos.z=pz;

		pickerScale.x=sx;
		pickerScale.y=sy;
	}else{
		text.draw2dRescale(f,px,py,pz,storedtext,fontRescaleFactor);

		pickerPos.x=px;
		pickerPos.y=py;
		pickerPos.z=pz;

		pickerScale.x=sx;
		pickerScale.y=sy;
	}

	glTranslatef(0,0,-pos.z);

	if(scissorSet){
		glDisable(GL_SCISSOR_TEST);
	}

}
 
TextPanel::TextPanel(){
	storedtext="";

	clickable=true;
	mouseover=false;
	onMouseLUp=NULL;
	onMouseLDown=NULL;

	color.x=1;
	color.y=1;
	color.z=1;
	color.w=1;

	pos.x=0;
	pos.y=0;

	scale.x=100;
	scale.y=100;

	percent=false;
	center=false;
	keepAspectRatio=false;

	visible=true;

	font=-1;
	align="";

	smartCenter=true;

	oldFontSpacing=1;
	oldFontLeading=1;

	scissorSet=false;
}

