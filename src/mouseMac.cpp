#include "mouse.h"
#include "conf.h"

#include "script.hpp"
#include "texture.hpp"
#include "timeing.hpp"
#include "os.h"

#include "Log.hpp"
#include <Carbon/Carbon.h>

#include "keyboard.h"

extern int nativeY;

Mouse::Mouse(){
    x=0;
    y=0;
    change_x=0;
    change_y=0;
    l=false;
    r=false;
    m=false;
	wheelUp=false;
	wheelDown=false;

	textureId=-1;
	textureScale.x=32;
	textureScale.y=32;
	color.x=1;
	color.y=1;
	color.z=1;
	color.w=1;

	visible=true;


	wait=0.5;
	wait=10;

	virtualMouse=true;
	lastVirtualMouse=true;
}


bool Mouse::left(){
	return l;    
}

bool Mouse::middle(){
	return m;    
}
    
    
bool Mouse::right(){
	return r;    
}
    
    
void Mouse::update(){
	
	if(!conf->windowed){
		
		Point loc;
		GetMouse(&loc);
    //assert(false);
		messageX=loc.h;
		messageY=loc.v;
		
		
		if(messageX>0 && messageY>0){
			change_x=float(messageX)-float(conf->sizeX)*0.5f;
			change_y=float(conf->sizeY)*0.5f-float(messageY);
		}else{
			change_x=0;
			change_y=0;
		}
		
		x+=change_x;
		y+=change_y;
		
		CGDirectDisplayID main=CGMainDisplayID();


		CGPoint pos;
		pos.x=float(conf->sizeX)*0.5f;
		pos.y=float(conf->sizeY)*0.5f;
		
		CGSetLocalEventsSuppressionInterval(0.0);
		CGWarpMouseCursorPosition(pos);
	}else{
		float lastX=x;
		float lastY=y;
	
		x=messageX;
		y=-messageY;
		
		change_x=x-lastX;
		change_y=y-lastY;
	}
		
	if(x<0){x=0;}
	if(y>0){y=0;}
	if(x>conf->sizeX){x=conf->sizeX;}
	if(y<-conf->sizeY){y=-conf->sizeY;}
	
	percentX=x/float(conf->sizeX);
	percentY=y/float(conf->sizeY);
	percent_change_x=change_x/float(conf->sizeX);
	percent_change_y=change_y/float(conf->sizeY);
	x_from_center=x-float(conf->sizeX)*0.5f;
	y_from_center=y-float(conf->sizeY)*0.5f;
	
	lDown=false;
	mDown=false;
	rDown=false;
	
	lUp=false;
	mUp=false;
	rUp=false;
	
	if(messageLDown){
		if(!l){lDown=true;}
		l=true;
	}else if(messageLUp){
		if(l){lUp=true;}
		l=false;
		
	}
	
	if(messageMDown && !keyboard.option){
		if(!m){mDown=true;}
		m=true;
		
	}else if(messageMUp && !keyboard.option){
		if(m){mUp=true;}
		mUp=true;
	}
	
	if(messageMDown && keyboard.option){
		if(!l){lDown=true;}
		l=true;
		
	}else if(messageMUp && keyboard.option){
		if(l){lUp=true;}
		lUp=true;
	}
	
	if(messageRDown && !keyboard.ctrl){
		if(!r){rDown=true;}
		r=true;
		
	}else if(messageRUp && !keyboard.ctrl){
		if(r){rUp=true;}
		rUp=true;
	}
	
	if(messageRDown && keyboard.ctrl){
		if(!l){lDown=true;}
		l=true;
		
	}else if(messageRUp && keyboard.ctrl){
		if(l){lUp=true;}
		lUp=true;
	}
	
	if(!virtualMouse){
		glutSetCursor( GLUT_CURSOR_INHERIT);
	}else{
		glutSetCursor( GLUT_CURSOR_NONE);
	}
	
	messageLUp=false;
	messageLDown=false;
	messageMUp=false;
	messageMDown=false;
	messageRUp=false;
	messageRDown=false;
}

    
void Mouse::show(){

}

void Mouse::hide(){
	
}

int luaMouseLoad(lua_State *L){
	String file = luaL_checkstring(L,1);

	
	int g=texture.load(file);

	if(g!=-1){
		mouse.textureId=g;

		ImageData im=texture.info(g);

		mouse.textureScale.x=im.width;
		mouse.textureScale.y=im.height;

		return 0;
	}else{
		int g=textureLibrary.load(file);

		if(g!=-1){
			mouse.textureId=g;

			ImageData im=texture.info(g);

			mouse.textureScale.x=im.width;
			mouse.textureScale.y=im.height;

			return 0;
		}
	}

	console().write("Error loading file '"+file+"'");
	
	return 0;
}



bool Mouse::scriptInit(){
	LuaType* m=script().add("mouse");

			script().add("load",&luaMouseLoad,m);
			script().add("virtual",&mouse.virtualMouse,m);

			script().add("l",&mouse.l,m);
			script().add("lUp",&mouse.lUp,m);
			script().add("lDown",&mouse.lDown,m);
			script().add("lDouble",&mouse.lDouble,m);
			script().add("r",&mouse.r,m);
			script().add("rUp",&mouse.rUp,m);
			script().add("rDown",&mouse.rDown,m);
			script().add("m",&mouse.m,m);
			script().add("mUp",&mouse.mUp,m);
			script().add("mDown",&mouse.mDown,m);

			script().add("wheelUp",&mouse.wheelUp,m);
			script().add("wheelDown",&mouse.wheelDown,m);
			script().add("visible",&mouse.visible,m);

			LuaType* c=script().add("color",m);
				script().add("r",&mouse.color.x,c);
				script().add("g",&mouse.color.y,c);
				script().add("b",&mouse.color.z,c);
				script().add("a",&mouse.color.w,c);

			LuaType* p=script().add("pos",m);
				script().add("x",&mouse.x,p);
				script().add("y",&mouse.y,p);

			LuaType* pp=script().add("posPercent",m);
				script().add("x",&mouse.percentX,pp);
				script().add("y",&mouse.percentY,pp);

			LuaType* pc=script().add("change",m);
				script().add("x",&mouse.change_x,pc);
				script().add("y",&mouse.change_y,pc);

			LuaType* ppc=script().add("changePercent",m);
				script().add("x",&mouse.percent_change_x,ppc);
				script().add("y",&mouse.percent_change_y,ppc);
	return true;
}

Mouse mouse;
