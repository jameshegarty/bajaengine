#include "panel.h"

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

#include "os.h"
#include "Helperlib/HelperLibMath.hpp"

#include "Helperlib/Log.hpp"

Map<Path,int> dupTex;
Map<Path,int> panelTexRefs;

int luaPanelScissorOff(lua_State *L){

	//get panel name from script
	String name;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Panel Scissor Error: Bad arguments (should be accessed as a class and have 0 arguments)");
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

	Panel* p=level->panelKey[name];

	p->scissorOff();

	return 0;
}

int luaPanelScissor(lua_State *L){

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

	Panel* t=level->panelKey[name];

	t->scissor(x,y,z,w);

	return 0;
}


int luaPanelLoad(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=2){
		console().write("Panel Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Panel error: bad arguments");
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("Panel error: bad arguments");
	}


	if(name==""){
		console().write("panel error (internal): no name given");
		lua_error(L);
		return 0;
	}

	Panel* t=level->panelKey[name];

	t->load(file,0);

	return 0;
}


int luaPanelLoadMouseover(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=2){
		console().write("Panel Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Panel error: bad arguments");
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("Panel error: bad arguments");
	}


	if(name==""){
		console().write("panel error (internal): no name given");

		lua_pushstring(L,"LOL");
		lua_error(L);

		return 0;
	}

	Panel* t=level->panelKey[name];

	t->load(file,1);

	return 0;
}


int luaPanelDisableMouseover(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);




	if(args.size()!=1){
		console().write("Panel DisableMouseover Error: Bad arguments, should be 1");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Panel error: bad arguments");
	}


	if(name==""){
		console().write("panel error (internal): no name given");
		return 0;
	}

	Panel* t=level->panelKey[name];

	t->disableMouseover();

	return 0;
}



int luaPanelEnableMouseover(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Panel Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Panel error: bad arguments");
	}


	if(name==""){
		console().write("panel error (internal): no name given");
		return 0;

	}

	Panel* t=level->panelKey[name];

	t->enableMouseover();

	return 0;
}

void Panel::disableMouseover(){

	doMouseover=false;
}

void Panel::enableMouseover(){
	if(!doMouseover){
		doMouseover=true;
		currentMouseoverFade=0;
	}
}

void Panel::scissor(float x, float y, float z, float w){
	scissorStore.x=x;
	scissorStore.y=y;
	scissorStore.z=z;
	scissorStore.w=w;
	scissorSet=true;
}

void Panel::scissorOff(){
	scissorSet=false;
}

bool Panel::load(Path file,int type=0){

	//type: 0=normal, 1=mouseover

	if(type==0){
		filename=file;
	}else if(type==1){
		filenameMouseover=file;
	}

	#ifdef LAZY_PANEL_LOAD  
		if(type==0){
		
			ImageData im=texture.loadHeader(file);
			
			if(im.loadSuccess){
				
				scale.x=im.width;
				scale.y=im.height;

				texsize.x=im.width;
				texsize.y=im.height;

				assigned=true;
				return true;
			}else{
				ImageData im=textureLibrary.loadHeader(file);
	
				if(im.loadSuccess){
					
					scale.x=im.width;
					scale.y=im.height;

					texsize.x=im.width;
					texsize.y=im.height;

					assigned=true;
					return true;
				}else{
					console().write("Error 1 loading file '"+file.getRelative()+"', "+im.error);
					return false;
				}
			}

			return false;
		}else if(type==1){

			ImageData im=texture.loadHeader(file);

			if(im.loadSuccess){
				
				scale.x=im.width;
				scale.y=im.height;

				texsize.x=im.width;
				texsize.y=im.height;

				assignedMouseover=true;
				return true;
			}else{
				ImageData im=textureLibrary.loadHeader(file);

				if(im.loadSuccess){
					
					scale.x=im.width;
					scale.y=im.height;

					texsize.x=im.width;
					texsize.y=im.height;

					assignedMouseover=true;
					return true;
				}else{
					console().write("Error 3 loading file '"+file.getRelative()+"', "+im.error);
					return false;
				}
			}

			return false;
		}

	#endif


	//check for dupTexlicate panels

	if(dupTex.find(file)){

		if(type==0){
			textureId=dupTex[file];
			assigned=true;
		}else if(type==1){
			textureIdMouseover=dupTex[file];
			assignedMouseover=true;
		}else{
			textureId=dupTex[file];
			assigned=true;
			
		}

		ImageData im=texture.info(dupTex[file]);
		panelTexRefs[file]++;

		scale.x=im.width;
		scale.y=im.height;

		texsize.x=im.width;
		texsize.y=im.height;

		return true;
	}

	

	int g=texture.loadNoMip(file);

	
	ImageData im=texture.info(g);

	if(g!=-1){
		if(type==0){
			textureId=g;
			assigned=true;
		}else if(type==1){
			textureIdMouseover=g;
			assignedMouseover=true;
		}else{
			textureId=g;
			assigned=true;
		}


		

		scale.x=im.width;
		scale.y=im.height;

		texsize.x=im.width;
		texsize.y=im.height;

		script().syncTo(NULL);

		dupTex[file]=g;
		panelTexRefs[file]=1;

		return true;
	}

	console().write("Error 2 loading file '"+file.getRelative()+"', "+im.error);
	return false;
}

 
void Panel::draw(){

	//to make things with negative scales work right
	bool negScaleX=false;
	bool negScaleY=false;
	if(scale.x<0){scale.x*=-1;negScaleX=true;}
	if(scale.y<0){
		scale.y*=-1;negScaleY=true;
	}

	if(!visible){
		return;
	}

	if(color.w<=0){
		return;
	}
	
	#ifdef LAZY_PANEL_LOAD
		if(!loaded && assigned && color.w>0){
			//lazyload

			if(dupTex.find(filename)){

				textureId=dupTex[filename];
				panelTexRefs[filename]++;
				loaded=true;
			}else{
				
				int g=texture.loadNoMip(filename);
				
				if(g==-1){

					int gg=textureLibrary.loadNoMip(filename);

					ImageData im=texture.info(gg);

					textureId=gg;
					dupTex[filename]=gg;
					panelTexRefs[filename]=1;


					loaded=true;
				}else{
					ImageData im=texture.info(g);

					textureId=g;
					dupTex[filename]=g;
					panelTexRefs[filename]=1;

					loaded=true;
				}
			}
		}

		if(!loadedMouseover && assignedMouseover && doMouseover && (mouseoverFade>0) && mouseover ){
			//lazyload
			if(dupTex.find(filenameMouseover)){

				textureIdMouseover=dupTex[filename];
				panelTexRefs[filename]++;
				loadedMouseover=true;
			}else{

				int g=texture.loadNoMip(filenameMouseover);
				
				if(g==-1){

					int gg=textureLibrary.loadNoMip(filenameMouseover);

					ImageData im=texture.info(gg);

					textureIdMouseover=gg;
					dupTex[filename]=gg;
					panelTexRefs[filename]=1;
					loadedMouseover=true;
				}else{
					ImageData im=texture.info(g);

					textureIdMouseover=g;
					dupTex[filename]=g;
					panelTexRefs[filename]=1;
					loadedMouseover=true;
				}
			}
		}
	#endif

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
		}else if(percent  && smartCenter==0){
			ys=+scissorStore.w;

			xs*=conf->sizeX;
			ys*=conf->sizeY;
			zs*=conf->sizeX;
			ws*=conf->sizeY;

			ys=conf->sizeY+scissorStore.y-ws;
		}else{
			ys=conf->sizeY+ys-ws;
		}



		glScissor(xs,ys,zs,ws);
	}

	float cw=color.w;

	if(mouseoverFade!=0 && mouseover && currentMouseoverFade<1 && assignedMouseover && doMouseover){

		
		currentMouseoverFade+=engineTime.time/(mouseoverFade);
	}else if(mouseoverFade!=0 && !mouseover && currentMouseoverFade>0 && assignedMouseover && doMouseover){
		
		currentMouseoverFade-=engineTime.time/(mouseoverFade);
	}

	if(mouseoverFade!=0 && assignedMouseover && doMouseover){
		cw=color.w*(1-currentMouseoverFade);
	}

	if(assignedMouseover && doMouseover && mouseover  && mouseoverFade==0){

		glColor4f(color.x,color.y,color.z,cw);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,textureIdMouseover);
	}else if(assigned){

		glColor4f(color.x,color.y,color.z,cw);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,textureId);
	}else{
		glDisable(GL_TEXTURE_2D);
		glColor4f(color.x,color.y,color.z,cw);
	}

	float sx=scale.x;
	float sy=scale.y;
	
	float px=pos.x;
	float py=pos.y;
	float pz=pos.z;

	float ratio=scale.x/scale.y;

	if(smartCenter!=0){
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

				//it is in an exact ratio, so no weird stuff needed

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

		float rx=scale.x;
		float ry=scale.y;

		if(assigned){
			rx=texsize.x;
			ry=texsize.y;
			ratio=rx/ry;
		}

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


	if(center  && smartCenter==0){
		px=(conf->sizeX/2)-(sx/2);
		py=(-conf->sizeY/2)-(-sy/2);
	}

	

	float t_x1=0;
	float t_x2=0;
	float t_y1=0;
	float t_y2=0;
	float t_x3=0;
	float t_x4=0;
	float t_y3=0;
	float t_y4=0;



	t_x1=0+texture.info(textureId).offsetTL.x;
	t_y1=0+texture.info(textureId).offsetTL.y;

	t_x2=1+texture.info(textureId).offsetTR.x;
	t_y2=0+texture.info(textureId).offsetTR.y;

	t_x3=1+texture.info(textureId).offsetBR.x;
	t_y3=-1+texture.info(textureId).offsetBR.y;

	t_x4=0+texture.info(textureId).offsetBL.x;
	t_y4=-1+texture.info(textureId).offsetBL.y;

	FloatVector2d p1v,p2v,p3v,p4v,p1tv,p2tv,p3tv,p4tv;
	float p1a,p2a,p3a,p4a,p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y;


	if(negScaleX){sx*=-1;}
	if(negScaleY){sy*=-1;}

	p1v=FloatVector2d(	-pivot.x*sx,	-pivot.y*sy			);
	p1a=math.vectorToAngleDegrees(p1v);

	p2v=FloatVector2d(	(1-pivot.x)*sx,	-pivot.y*sy			);
	p2a=math.vectorToAngleDegrees(p2v);

	p3v=FloatVector2d(	(1-pivot.x)*sx,	((-1)-pivot.y)*sy	);
	p3a=math.vectorToAngleDegrees(p3v);

	p4v=FloatVector2d(	-pivot.x*sx,	((-1)-pivot.y)*sy	);
	p4a=math.vectorToAngleDegrees(p4v);


	p1tv.x=math.cosDegrees(p1a+rot.z)*p1v.magnitude();//.x;
	p1tv.y=math.sinDegrees(p1a+rot.z)*p1v.magnitude();//.y;
	p1x=p1tv.x+px;	//+(px+(pivot.x*sx));
	p1y=p1tv.y+py;	//+(py+(pivot.y*sy));

	p2tv.x=math.cosDegrees(p2a+rot.z)*p2v.magnitude();//.x;
	p2tv.y=math.sinDegrees(p2a+rot.z)*p2v.magnitude();//.y;
	p2x=p2tv.x+px;	//+px-((1-pivot.x)*sx);
	p2y=p2tv.y+py;	//+py+(pivot.y*sy);

	p3tv.x=math.cosDegrees(p3a+rot.z)*p3v.magnitude();	//p3v.x;
	p3tv.y=math.sinDegrees(p3a+rot.z)*p3v.magnitude();	//.y;

	p3x=p3tv.x+px;//-((1-pivot.x)*sx);
	p3y=p3tv.y+py;//-(((-1)-pivot.y)*sy);

	p4tv.x=math.cosDegrees(p4a+rot.z)*p4v.magnitude();//.x;
	p4tv.y=math.sinDegrees(p4a+rot.z)*p4v.magnitude();//.y;
	p4x=p4tv.x+px;
	p4y=p4tv.y+py;

	glBegin(GL_QUADS);
		glTexCoord2d(t_x1,t_y1); glVertex3f(p1x	,p1y,pz);
		glTexCoord2d(t_x2,t_y2); glVertex3f(p2x	,p2y,pz);
		glTexCoord2d(t_x3,t_y3); glVertex3f(p3x	,p3y,pz);
		glTexCoord2d(t_x4,t_y4); glVertex3f(p4x	,p4y,pz);
	glEnd();

	pickerPos.z=pz;

	pickerPos.x=p1x;
	if(p2x<pickerPos.x){pickerPos.x=p2x;}
	if(p3x<pickerPos.x){pickerPos.x=p3x;}
	if(p4x<pickerPos.x){pickerPos.x=p4x;}
	
	pickerPos.y=p1y;
	if(p2y>pickerPos.y){pickerPos.y=p2y;}
	if(p3y>pickerPos.y){pickerPos.y=p3y;}
	if(p4y>pickerPos.y){pickerPos.y=p4y;}

	pickerScale.x=p1x-pickerPos.x;
	if(p2x-pickerPos.x>pickerScale.x){pickerScale.x=p2x-pickerPos.x;}
	if(p3x-pickerPos.x>pickerScale.x){pickerScale.x=p3x-pickerPos.x;}
	if(p4x-pickerPos.x>pickerScale.x){pickerScale.x=p4x-pickerPos.x;}

	pickerScale.y=p1y-pickerPos.y;
	if(pickerPos.y-p2y>pickerScale.y){pickerScale.y=pickerPos.y-p2y;}
	if(pickerPos.y-p3y>pickerScale.y){pickerScale.y=pickerPos.y-p3y;}
	if(pickerPos.y-p4y>pickerScale.y){pickerScale.y=pickerPos.y-p4y;}

	if(mouseoverFade!=0 && assignedMouseover  && doMouseover){
		//draw mouseover part
		cw=color.w*currentMouseoverFade;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,textureIdMouseover);
		glColor4f(color.x,color.y,color.z,cw);

		glBegin(GL_QUADS);
			glTexCoord2d(t_x1,t_y1); glVertex3f(p1x	,p1y,pz);
			glTexCoord2d(t_x2,t_y2); glVertex3f(p2x	,p2y,pz);
			glTexCoord2d(t_x3,t_y3); glVertex3f(p3x	,p3y,pz);
			glTexCoord2d(t_x4,t_y4); glVertex3f(p4x	,p4y,pz);
		glEnd();

	}

	if(scissorSet){
		glDisable(GL_SCISSOR_TEST);
	}

	if(negScaleX){scale.x*=-1;}
	if(negScaleY){
		scale.y*=-1;
	}
}

Panel::Panel(){
	assigned=false;
	textureId=0;

	assignedMouseover=false;
	textureIdMouseover=0;

	mouseoverFade=0;
	currentMouseoverFade=0;

	mouseover=false;

	scissorSet=false;

	color.x=1;
	color.y=1;
	color.z=1;
	color.w=1;

	pos.x=0;
	pos.y=0;

	scale.x=1001;
	scale.y=100;

	pivot.x=0;
	pivot.y=0;

	percent=0;
	center=0;
	keepAspectRatio=false;

	visible=true;

	smartCenter=0;

	clickable=true;
	doMouseover=true;

	#ifdef LAZY_PANEL_LOAD
		loaded=false;
		loadedMouseover=false;
	#endif

	onMouseLUp=NULL;
	onMouseLDown=NULL;

	forced=false;
}

Panel::~Panel(){
	//free OGL memory

	if(panelTexRefs[filename]==1){
		glDeleteTextures(1,(GLuint*)&textureId);
		glDeleteTextures(1,(GLuint*)&textureIdMouseover);
		panelTexRefs.remove(filename);
		dupTex.remove(filename);
	}else{
		panelTexRefs[filename]--;
	}
}

void Panel::forceTexture(unsigned int tex){
	assigned=true;
	textureId=tex;
	loaded=true;
	forced=true;

	if(textureId==-1){
		assigned=false;
		loaded=false;
	}
}
