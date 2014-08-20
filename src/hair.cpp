#include "hair.h"
#include "level.hpp"
#include "sort.hpp"
#include "HelperLibMath.hpp"
#include "random.h"
#include "FileIo.hpp"
#include "timeing.hpp"
#include "Types.hpp"

Hair::Hair(){
	length=5;
	lengthRandom=5;
	curl=1;
	stiffness=1;
	width=5;
	windVelocity.x=1;
	windVelocity.y=0;
	windVelocity.z=1;

	windPower.x=1;
	windPower.y=0;
	windPower.z=1;

	colorBase.x=0.05;
	colorBase.y=0.5;
	colorBase.z=0;
	colorBase.w=1;

	colorBaseRandom.x=0.01;
	colorBaseRandom.y=0.2;
	colorBaseRandom.z=0;
	colorBaseRandom.w=0;

	colorTip.x=0.05;
	colorTip.y=0.8;
	colorTip.z=0;
	colorTip.w=1;

	colorTipRandom.x=0.01;
	colorTipRandom.y=0.2;
	colorTipRandom.z=0;
	colorTipRandom.w=0;

	number=100;
	segments=3;

	visible=true;

	windmap=NULL;
	densitymap=NULL;

	windmapX=0;
	windmapY=0;

	obj=NULL;
}

void Hair::generate(){
	//use the lightmap texcoords to calc position

	/*
	x1t=(rand*(1-y1t))+0.5(y1t)
	y1t=1-sqrt(t)

	t=the pos on the longest side of the triangle 

	to choose the triangle - get the areas and line them up

	frand(0:1)*(1-t)^0.5+(0.5)(-(1-t)^0.5+1)
	-(1-t)^0.5+1

frand(0:1)*(t^(0.5))+0.0(1-(t^(0.5)))
1-(t^(0.5))
	*/


	Array<SortItem> sortlist;
	Array<HairTri> hairtris;

	HairTri buffer;
	bool mode=false;	//true= tris, false=strip
	int tripos=0;

	for(int i=0; i<obj->getVertexCount(); i++){
		if(obj->getVertexRaw(i).end==START_STRIP){
			mode=false;
		}else if(obj->getVertexRaw(i).end==START_TRI){
			mode=true;
			tripos=0;
		}

		if(mode){
			//tris
			if(tripos==0){
				buffer.pos1=obj->getVertex(i).pos;
				buffer.normal1=obj->getVertex(i).normal;
				buffer.coord1=obj->getVertex(i).uvLightmap;
			}else if(tripos==1){
				buffer.pos2=obj->getVertex(i).pos;
				buffer.normal2=obj->getVertex(i).normal;
				buffer.coord2=obj->getVertex(i).uvLightmap;
			}else if(tripos==2){
				buffer.pos3=obj->getVertex(i).pos;
				buffer.normal3=obj->getVertex(i).normal;
				buffer.coord3=obj->getVertex(i).uvLightmap;
			}

			tripos++;

			if(tripos==3){
				hairtris.pushBack(buffer);
				tripos=0;
			}


		}else{
			if(obj->getVertex(i+1).end!=END_STRIP && obj->getVertex(i).end!=END_STRIP){
				buffer.pos1=obj->getVertex(i).pos;
				buffer.pos2=obj->getVertex(i+1).pos;
				buffer.pos3=obj->getVertex(i+2).pos;
				buffer.normal1=obj->getVertex(i).normal;
				buffer.normal2=obj->getVertex(i+1).normal;
				buffer.normal3=obj->getVertex(i+2).normal;
				buffer.coord1=obj->getVertex(i).uvLightmap;
				buffer.coord2=obj->getVertex(i+1).uvLightmap;
				buffer.coord3=obj->getVertex(i+2).uvLightmap;
				hairtris.pushBack(buffer);
			}
		}
	}

	totalArea=0;
	for(int i=0; i<hairtris.size(); i++){
		hairtris[i].area=math.area(hairtris[i].pos1,hairtris[i].pos2,hairtris[i].pos3);
		totalArea+=hairtris[i].area;
	}


	for(int i=0; i<hairtris.size(); i++){
		SortItem r;
		r.key=i;
		r.value=hairtris[i].area;

		sortlist.pushBack(r);
	}

	Array<int> out=sort.sort(sortlist,true);

	Array<HairTri> finalhairtris;

	for(int i=0; i<out.size(); i++){
		finalhairtris.pushBack(hairtris[out[i]]);
	}

	for(int i=0; i<finalhairtris.size(); i++){
		if(i==0){
			finalhairtris[i].seqarea=finalhairtris[i].area;
			finalhairtris[i].prevarea=0;
		}else{
			finalhairtris[i].seqarea=finalhairtris[i-1].seqarea+finalhairtris[i].area;
			finalhairtris[i].prevarea=finalhairtris[i-1].seqarea;
		}
	}

	hairState=new HairState[number];

	float xr=0;
	float xr2=0;
	int trii;
	HairState hb;

	for(int i=0; i<number; i++){
		xr=randomGen.Random()*totalArea;
		xr2=randomGen.Random();

		

		trii=mapToTri(xr,xr2,finalhairtris);

		if(densitymap!=NULL){
			if(!densityTest(trii,xr,xr2,finalhairtris)){
				i--;
				continue;
			}
		}

		hb=generateTheHair(trii,xr,xr2,finalhairtris);

		hairState[i]=hb;
	}

	finalhairtriscopy=finalhairtris;

}


float Hair::densityMapValue(float x,float y){
	// see windMapValue for info

	x=x-math.floor(x);
	y=y-math.floor(y);

	int x_l=math.floor(x*float(densitymapX-1));
	int x_r=math.ceil(x*float(densitymapX-1));

	int y_l=math.floor(y*float(densitymapY-1));
	int y_r=math.ceil(y*float(densitymapY-1));

	float c1=densitymap[densitymapX*y_l+x_l];
	float c2=densitymap[densitymapX*y_l+x_r];
	float c3=densitymap[densitymapX*y_r+x_l];
	float c4=densitymap[densitymapX*y_r+x_r];
	c1/=255;
	c2/=255;
	c3/=255;
	c4/=255;

	float x1=(c3*y)+(c1*(1-y));
	float x2=(c4*y)+(c2*(1-y));

	float final=(x1*x)+(x2*(1-x));

	return final;
}

bool Hair::densityTest(int index,float xr,float xr2,Array<HairTri>& finalhairtris){

	float t=(xr-finalhairtris[index].prevarea)/finalhairtris[index].area;
	float xpos=xr2;
	float ypos=1-math.pow(t,0.5);

	FloatVector2d pcoord=coordOnTri(finalhairtris[index],xpos,ypos);

	float v=densityMapValue(pcoord.x,pcoord.y);

	if(v>randomGen.Random()){
		return true;
	}

	return false;
}

int Hair::mapToTri(float xr,float xr2,Array<HairTri>& finalhairtris){
	
	int pi=(float(xr)/float(totalArea))*finalhairtris.size();

	if(pi<0){pi=0;}

	if(finalhairtris[pi].prevarea<xr && finalhairtris[pi].seqarea>xr){
		//guessed right
		return pi;
	}

	while(finalhairtris[pi].prevarea>xr){
		pi--;
	}

	while(finalhairtris[pi].seqarea<xr){
		pi++;
	}

	return pi;
}

FloatVector3d pointOnTri(HairTri& tri,float x,float y){

	FloatVector3d line=(tri.pos2-tri.pos1);

	//startingpos: pos1

	FloatVector3d final;

	final=tri.pos1+x*line;

	//get the vector to pos3
	FloatVector3d tp3=(tri.pos3-final);

	final=final+tp3*y;

	return final;
}

FloatVector3d normalOnTri(HairTri& tri,float x,float y){
	FloatVector3d line=(tri.normal2-tri.normal1);

	FloatVector3d final;

	final=tri.normal1+x*line;

	//get the vector to pos3
	FloatVector3d tp3=(tri.normal3-final);

	final=final+tp3*y;

	return final;
}


FloatVector2d Hair::coordOnTri(HairTri& tri,float x,float y){

	FloatVector2d line=(tri.coord2-tri.coord1);

	FloatVector2d final;

	final=tri.coord1+x*line;

	//get the vector to pos3
	FloatVector2d tp3=(tri.coord3-final);

	final=final+tp3*y;

	return final;
}

HairState Hair::generateTheHair(int index,float r,float xr2,Array<HairTri>& finalhairtris){
	HairState h;

	float t=(r-finalhairtris[index].prevarea)/finalhairtris[index].area;

	float rseg=segments;
	rseg+=1;	//end vertex
	if(rseg<2){rseg=2;}

	h.segmentColor=new FloatVector4d[(int)rseg];
	h.segmentPos=new FloatVector3d[(int)rseg];

	FloatVector3d basePos;
	FloatVector3d baseNormal;

	float xpos=xr2;
	float ypos=1-math.pow(t,0.5);

	basePos=pointOnTri(finalhairtris[index],xpos,ypos);
	baseNormal=normalOnTri(finalhairtris[index],xpos,ypos);

	if(windmap!=NULL){
		FloatVector2d pcoord=coordOnTri(finalhairtris[index],xpos,ypos);

		h.windCoord=pcoord;

		if(h.windCoord.x>1){
			h.windCoord.x=1;
		}

		if(h.windCoord.y>1){
			h.windCoord.y=1;
		}

		if(h.windCoord.y<0){
			h.windCoord.y=0;
		}

		if(h.windCoord.x<0){
			h.windCoord.x=0;
		}

	}

	FloatVector4d cb=colorBase+randomGen.Random()*colorBaseRandom;
	FloatVector4d ct=colorTip+randomGen.Random()*colorTipRandom;

	h.segmentPos[0]=basePos;
	h.segmentColor[0]=cb;
	h.normal=baseNormal;

	float weight;
	float tl=float(length/float(rseg))+randomGen.Random()*lengthRandom;

	FloatVector3d curlfactor;
	curlfactor.x=0;
	curlfactor.y=0;
	curlfactor.z=0;


	for(int i=0; i<rseg; i++){
		if(curl>0 && i>0){
			curlfactor.x=0+(randomGen.Random()*curl);
			curlfactor.y=0+(randomGen.Random()*curl);
			curlfactor.z=0+(randomGen.Random()*curl);
		}else if(curl>0 && i==0){
			curlfactor.x=0;
			curlfactor.y=0;
			curlfactor.z=0;
		}

		h.segmentPos[i]=basePos+baseNormal*tl*float(i)+curlfactor;
		weight=float(i)/float(rseg-1);

		h.segmentColor[i]=(cb*float(1-weight))+(ct*(weight));

		weight=float(i)/float(rseg);
	}

	return h;
}

float Hair::windMapValue(float x,float y){
	//"sub-pixel" precision on the grass wind sampling so that it doesn't jerk

	//4 corners
	/*				s1
			1(l,l)		2(r,l)  
		s4						s2
			3(l,r)		4(r,r)  
					s3

					               ^

					>
	*/

	x=x-math.floor(x);
	y=y-math.floor(y);

	int x_l=math.floor(x*float(windmapX-1));
	int x_r=math.ceil(x*float(windmapX-1));

	int y_l=math.floor(y*float(windmapY-1));
	int y_r=math.ceil(y*float(windmapY-1));

	float c1=windmap[windmapX*y_l+x_l];
	float c2=windmap[windmapX*y_l+x_r];
	float c3=windmap[windmapX*y_r+x_l];
	float c4=windmap[windmapX*y_r+x_r];
	c1/=255;
	c2/=255;
	c3/=255;
	c4/=255;

	float x1=(c3*y)+(c1*(1-y));
	float x2=(c4*y)+(c2*(1-y));

	float final=(x1*x)+(x2*(1-x));

	return final;
}

void Hair::draw(){

	if(obj==NULL){
		return;
	}

	
	if(length!=pastLength){
		generate();
		pastLength=length;
	}

	if(lengthRandom!=pastLengthRandom){
		generate();
		pastLengthRandom=lengthRandom;
	}

	if(colorTip!=pastColorTip){
		generate();
		pastColorTip=colorTip;
	}

	if(colorTipRandom!=pastColorTipRandom){
		generate();
		pastColorTipRandom=colorTipRandom;
	}

	if(colorBaseRandom!=pastColorBaseRandom){
		generate();
		pastColorBaseRandom=colorBaseRandom;
	}

	if(colorBase!=pastColorBase){
		generate();
		pastColorBase=colorBase;
	}

	if(number!=pastNumber){
		generate();
		pastNumber=number;
	}

	if(lengthRandom!=pastLengthRandom){
		generate();
		pastLengthRandom=lengthRandom;
	}

	if(curl!=pastCurl){
		generate();
		pastCurl=curl;
		
	}

	if(segments!=pastSegments){
		generate();
		pastSegments=segments;
		
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);

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

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	glLineWidth(width);

	FloatVector3d lp;
	FloatVector3d pastLp;
	FloatVector4d lc;
	FloatVector4d pastLc;

	float mod=0;
	windPos+=windVelocity*engineTime.time;

	float rseg=segments;
	rseg+=1;
	if(rseg<2){rseg=2;}

	for(int i=0; i<number; i++){
		glBegin(GL_LINES);

		for(int s=0; s<rseg; s++){
			lp=hairState[i].segmentPos[s];
			
			mod=(float(s)/float(rseg-1));

			mod*=mod;

			mod*=windMapValue(hairState[i].windCoord.x + (windPos.x/windmapX),   hairState[i].windCoord.y + (windPos.z/windmapY))*(1/stiffness);

			lp.x=lp.x+mod*windPower.x;
			lp.y=lp.y+mod*windPower.y;
			lp.z=lp.z+mod*windPower.z;

			lc=hairState[i].segmentColor[s];

			if(s>0){
				glColor4f(pastLc.x,pastLc.y,pastLc.z,pastLc.w);
				glVertex3f(pastLp.x,pastLp.y,pastLp.z);

				glColor4f(lc.x,lc.y,lc.z,lc.w);
				glVertex3f(lp.x,lp.y,lp.z);
			}

			
			pastLp=lp;
			pastLc=lc;
		}

		glEnd();
	}


	/*UNCOMMENT TO DRAW VERTICES
	
	glColor4f(0,0,1,1);

	glPointSize(width+3);
	glBegin(GL_POINTS);
	for(int i=0; i<number; i++){
		

		for(int s=0; s<segments; s++){
			lp=hairState[i].segmentPos[s];

			mod=(float(s)/float(segments-1));

			mod*=mod;

			lp=lp+mod*windMapValue(hairState[i].windCoord.x + (windPos.x/windmapX),   hairState[i].windCoord.y + (windPos.z/windmapY))*(1/stiffness);


			glVertex3f(lp.x,lp.y,lp.z);
		}

	}
	glEnd();*/
	
}

bool Hair::setObject(String objs){

	Object* o=level->objectKey[objs];

	if(o!=NULL){
		obj=o;
		generate();	//will get called because everything is out of date - eh, no we really need it just in case

		return true;
	}else{
		console().write("error, object "+objs+" not found");
		return false;

	}

	return false;
}


int luaHairSetObject(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("Hair setObject Error: Bad arguments, be sure to access as a class");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Hair setObject error: bad arguments, be sure to access as a class");
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		file=args[1].value.ss;
	}else{
		console().write("Hair setObject error: bad arguments, be sure to access as a class");
	}


	if(name==""){
		console().write("Hair setObject error (internal): no name given");
		return 0;
	}

	Hair* t=level->hairKey[name];

	t->setObject(file);

	return 0;
}

bool Hair::loadWindMap(String file){

	ImageData t=loadImage(file);

	if(t.loadSuccess){
		windmap=new unsigned char[t.width*t.height];

		windmapX=t.width;
		windmapY=t.height;

		int ii=0;

		for(int i=0; i<t.width*t.height*t.bytesPP; i+=t.bytesPP){
			windmap[ii]=t.data[i];
			ii++;
		}

		delete []t.data;
	}else{
		console().write("error loading windmap");
	}

	return false;
}

int luaHairLoadWindMap(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("Hair LoadWindMap Error: Bad arguments, be sure to access as a class");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Hair LoadWindMap Error: Bad arguments, be sure to access as a class");
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("Hair LoadWindMap Error: Bad arguments, be sure to access as a class");
	}


	if(name==""){
		console().write("Particle2d error (internal): no name given");
		return 0;
	}

	Hair* t=level->hairKey[name];

	t->loadWindMap(file);

	return 0;
}



bool Hair::loadDensityMap(String file){

	ImageData t=loadImage(file);

	if(t.loadSuccess){
		densitymap=new unsigned char[t.width*t.height];

		densitymapX=t.width;
		densitymapY=t.height;

		int ii=0;

		for(int i=0; i<t.width*t.height*t.bytesPP; i+=t.bytesPP){
			densitymap[ii]=t.data[i];
			ii++;
		}

		delete []t.data;
	}else{
		console().write("error loading densitymap");
	}

	return false;
}

int luaHairLoadDensityMap(lua_State *L){

	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("Hair LoadWindMap Error: Bad arguments, be sure to access as a class");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}

	}else{
		console().write("Hair LoadWindMap Error: Bad arguments, be sure to access as a class");
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		file=args[1].value.ss;
	}else{
		console().write("Hair LoadWindMap Error: Bad arguments, be sure to access as a class");
	}


	if(name==""){
		console().write("Particle2d error (internal): no name given");
		return 0;
	}

	Hair* t=level->hairKey[name];

	t->loadDensityMap(file);

	return 0;
}
