#include "Types.hpp"

#include <iostream>
#include <fstream>

#include "HelperLibMath.hpp"
#include "conf.h"
#include "SceneGraph.hpp"
#include "texture.hpp"
#include "Filters.h"

#ifndef XSICONVERT
	#include "level.hpp"

	#ifdef _WIN32
	#include <gl/gl.h>
	#include "gl/extensions.h"
	#elif LINUX
		#include <gl.h>
	#else
		#include <GLUT/glut.h>
	#endif
	
	#include "Log.hpp"
	#include "os.h"

	
#endif

#ifdef XSICONVERT
	extern Array<String> jointMap;
#endif

#include "objects.hpp"
#include "picking.h"


TextureImage::TextureImage(){
	flags=0;
	glSet=false;
	ref=false;
	loadedTextureLevel=-1;
	bytesPP=0;
	filePos=-1;
	dataLoaded=false;
}


unsigned long Vertex::size(){
	unsigned long size=0;

	size+=sizeof(pos);
	size+=sizeof(normal);
	size+=sizeof(binormal);
	size+=sizeof(tangent);
	size+=sizeof(uv0);
	size+=sizeof(end);
	size+=sizeof(envelopeIndex);

	return size;
}

bool Vertex::write(std::ofstream& out){
	out.write((char*)&pos,sizeof(pos));
	out.write((char*)&normal,sizeof(normal));
	out.write((char*)&binormal,sizeof(binormal));
	out.write((char*)&tangent,sizeof(tangent));
	out.write((char*)&uvLightmap,sizeof(uvLightmap));
	out.write((char*)&end,sizeof(end));
	out.write((char*)&envelopeIndex,sizeof(envelopeIndex));

	return true;
}



bool Material::write(std::ofstream& out){
	std::cout << "name: " << name << std::endl;
	std::cout << "diffuse : ["  << diffuse.x << "," << diffuse.y << "," << diffuse.z << "," << diffuse.w << "] " << std::endl;

	name.write(&out);

	out.write((char*)&shader,sizeof(shader));

	out.write((char*)&diffuse,sizeof(diffuse));
	out.write((char*)&specular,sizeof(specular));
	out.write((char*)&ambient,sizeof(ambient));
	out.write((char*)&reflectivity,sizeof(reflectivity));

	out.write((char*)&tex0pos,sizeof(tex0pos));
	out.write((char*)&tex0scale,sizeof(tex0scale));

	out.write((char*)&tex1pos,sizeof(tex1pos));
	out.write((char*)&tex1scale,sizeof(tex1scale));

	out.write((char*)&tex2pos,sizeof(tex2pos));
	out.write((char*)&tex2scale,sizeof(tex2scale));

	out.write((char*)&tex3pos,sizeof(tex3pos));
	out.write((char*)&tex3scale,sizeof(tex3scale));

	out.write((char*)&tex4pos,sizeof(tex4pos));
	out.write((char*)&tex4scale,sizeof(tex4scale));

	out.write((char*)&tex5pos,sizeof(tex5pos));
	out.write((char*)&tex5scale,sizeof(tex5scale));

	out.write((char*)&tex6pos,sizeof(tex6pos));
	out.write((char*)&tex6scale,sizeof(tex6scale));


	out.write((char*)&shiny,sizeof(shiny));
	out.write((char*)&textureDiffuse,sizeof(textureDiffuse));
	out.write((char*)&textureDisplacement,sizeof(textureDisplacement));
	out.write((char*)&textureSpecular,sizeof(textureSpecular));
	out.write((char*)&textureBump,sizeof(textureBump));

	out.write((char*)&textureEnvironmentFront,sizeof(textureEnvironmentFront));
	out.write((char*)&textureEnvironmentBack,sizeof(textureEnvironmentBack));
	out.write((char*)&textureEnvironmentLeft,sizeof(textureEnvironmentLeft));
	out.write((char*)&textureEnvironmentRight,sizeof(textureEnvironmentRight));
	out.write((char*)&textureEnvironmentTop,sizeof(textureEnvironmentTop));
	out.write((char*)&textureEnvironmentBottom,sizeof(textureEnvironmentBottom));

	
	return true;
}

unsigned long Material::size(){

	unsigned long size=0;

	size+=name.filesize();

	size+=sizeof(shader);

	size+=sizeof(diffuse);
	size+=sizeof(ambient);
	size+=sizeof(specular);
	size+=sizeof(reflectivity);

	size+=sizeof(tex0pos);
	size+=sizeof(tex0scale);

	size+=sizeof(tex1pos);
	size+=sizeof(tex1scale);

	size+=sizeof(tex2pos);
	size+=sizeof(tex2scale);

	size+=sizeof(tex3pos);
	size+=sizeof(tex3scale);

	size+=sizeof(tex4pos);
	size+=sizeof(tex4scale);

	size+=sizeof(tex5pos);
	size+=sizeof(tex5scale);

	size+=sizeof(tex6pos);
	size+=sizeof(tex6scale);


	size+=sizeof(shiny);
	size+=sizeof(textureDiffuse);
	size+=sizeof(textureDisplacement);
	size+=sizeof(textureSpecular);
	size+=sizeof(textureBump);

	size+=sizeof(textureEnvironmentFront);
	size+=sizeof(textureEnvironmentBack);
	size+=sizeof(textureEnvironmentLeft);
	size+=sizeof(textureEnvironmentRight);
	size+=sizeof(textureEnvironmentTop);
	size+=sizeof(textureEnvironmentBottom);


	return size;
}

Material::Material(){
	textureDiffuse=-1;
	textureDisplacement=-1;
	textureSpecular=-1;
	textureBump=-1;

	textureEnvironmentFront=-1;
	textureEnvironmentBack=-1;
	textureEnvironmentLeft=-1;
	textureEnvironmentRight=-1;
	textureEnvironmentTop=-1;
	textureEnvironmentBottom=-1;

	tex0scale=FloatVector2d(1,1);
	tex1scale=FloatVector2d(1,1);
	tex2scale=FloatVector2d(1,1);
	tex3scale=FloatVector2d(1,1);
	tex4scale=FloatVector2d(1,1);
	tex5scale=FloatVector2d(1,1);
	tex6scale=FloatVector2d(1,1);
	texLightmapScale=FloatVector2d(1,1);

	shiny=50;

	nonstaticRefs=0;
	oldNonstaticRefs=0;

	dirty=false;


	uploaded=false;

	shader=0;
}


Vertex::Vertex(){
	end=false;
}


bool TextureImage::glNameSet(){
	return glSet;
}

void TextureImage::glNameSet(bool set){
	glSet=set;
}

unsigned int TextureImage::glName(){
	return gl;
}

void TextureImage::load(){

	if(filePos!=-1  && file!=""){

		if(! *dataLoaded){

			std::ifstream in (file.c_str(), std::ios::in|std::ios::binary);

			data=new unsigned char[x*y*bytesPP];

			in.seekg(filePos);

			in.read((char*)data,x*y*bytesPP);

			in.close();

			*dataLoaded=true;
		}

	}
}

void TextureImage::reload(){
#ifndef XSICONVERT
	glBindTexture(GL_TEXTURE_2D,glName());

	if(!*dataLoaded){
		load();
	}

	texture.load(this);
#endif
}

unsigned int TextureImage::glName(unsigned int i){
	gl=i;
	glSet=true;
	return gl;
}

unsigned long TextureImage::size(){
	unsigned long size=0;

	if(ref){
		size+=sizeof(ref);
		size+=sizeof(flags);
		size+=file.filesize();
		size+=fileRelative.filesize();
	}else{
		size+=sizeof(ref);
		size+=sizeof(x);
		size+=sizeof(y);
		size+=sizeof(bytesPP);

		size+=x*y*bytesPP;
	}

	return size;

}

bool TextureImage::write(std::ofstream& out){
	//dont forget to add it to ::size, idiot

	if(ref){
		out.write((char*)&ref,sizeof(ref));
		out.write((char*)&flags,sizeof(flags));
		file.write(&out);
		fileRelative.write(&out);
		
	}else{
		out.write((char*)&ref,sizeof(ref));
		out.write((char*)&x,sizeof(x));
		out.write((char*)&y,sizeof(y));
		out.write((char*)&bytesPP,sizeof(bytesPP));

		out.write((char*)data,x*y*bytesPP);
	}

	return true;
}

unsigned long Object::size(){
	unsigned long size=0;

	size+=sizeof(type);
	size+=sizeof(flags);
	size+=sizeof(material);
	size+=sizeof(lightmap);
	size+=sizeof(sceneGraphLevel);

	size+=sizeof(pos);
	size+=sizeof(rot);
	size+=sizeof(scale);

	size+=name.filesize();
	size+=action.filesize();

	size+=constraintPosName.filesize();
	size+=4;
	size+=constraintRotName.filesize();
	size+=4;
	size+=constraintScaleName.filesize();
	size+=4;

	size+=sizeof(vertexCount);

	if(type==8){
		#ifdef XSICONVERT
			size+=jointMap[vertices[0].pos.x].filesize();
		#endif
	}else if(type==5){
		//root
		size+=4;
		size+=4*3*3;
		#ifdef XSICONVERT
			for(int i=0; i<vertexCount; i++){
				size+=jointMap[vertices[i].pos.x].filesize();
			}
		#endif
	}else if(type==7){	//joint
		size+=4;
		size+=4*3*3;

		#ifdef XSICONVERT
			size+=4;	//length
			size+=jointMap[vertices[0].pos.y].filesize();
			size+=3*4;	//base pos/rot/scale
		#endif
	}else if(type==9){	//curve

		size+=4;	//degree
		size+=4;	//closed

		#ifdef XSICONVERT
			size+=3*4*(vertexCount);	//pos
		#endif
	}else{	//effector
		
		if(vertexCount>0){
			size+=vertexCount*vertices[0].size();	//sizeof(Vertex);
		}
	}

	size+=sizeof(unsigned int);	//animations.size()

	size+=sizeof(unsigned int);	//envelopes.size()

	if(type==0 || type==3){
		size+=4;
		size+=4*3*3;
	}


	return size;
}

bool Object::write(std::ofstream& out){

	std::cout << "write object '" << name << "'" << std::endl;

	out.write((char*)&type,sizeof(type));
	out.write((char*)&flags,sizeof(flags));
	out.write((char*)&material,sizeof(material));
	out.write((char*)&lightmap,sizeof(lightmap));
	out.write((char*)&sceneGraphLevel,sizeof(sceneGraphLevel));

	name.write(&out);
	action.write(&out);

	constraintPosName.write(&out);
	out.write((char*)&constraintPosType,4);
	constraintRotName.write(&out);
	out.write((char*)&constraintRotType,4);
	constraintScaleName.write(&out);
	out.write((char*)&constraintScaleType,4);

	//write transforms
	out.write((char*)&pos,sizeof(pos));
	out.write((char*)&rot,sizeof(rot));
	out.write((char*)&scale,sizeof(scale));

	//then vertices
	out.write((char*)&vertexCount,sizeof(vertexCount));


	if(type==8){
		//effector
		#ifdef XSICONVERT
			jointMap[vertices[0].pos.x].write(&out);
		#endif
	}else if(type==5){
		//root
		#ifdef XSICONVERT

			unsigned int bp=0;
			if(hasBasePose){bp=1;}
			out.write((char*)&bp,4);

			out.write((char*)&basePos.x,4);
			out.write((char*)&basePos.y,4);
			out.write((char*)&basePos.z,4);

			out.write((char*)&baseRot.x,4);
			out.write((char*)&baseRot.y,4);
			out.write((char*)&baseRot.z,4);

			out.write((char*)&baseScale.x,4);
			out.write((char*)&baseScale.y,4);
			out.write((char*)&baseScale.z,4);

			for(int i=0; i<vertexCount; i++){
				jointMap[vertices[i].pos.x].write(&out);
			}
		#endif

	}else if(type==7){
		//joint

		unsigned int bp=0;
		if(hasBasePose){bp=1;}
		out.write((char*)&bp,4);

		out.write((char*)&basePos.x,4);
		out.write((char*)&basePos.y,4);
		out.write((char*)&basePos.z,4);

		out.write((char*)&baseRot.x,4);
		out.write((char*)&baseRot.y,4);
		out.write((char*)&baseRot.z,4);

		out.write((char*)&baseScale.x,4);
		out.write((char*)&baseScale.y,4);
		out.write((char*)&baseScale.z,4);


		#ifdef XSICONVERT
			out.write((char*)&vertices[0].pos.x,4);
			jointMap[vertices[0].pos.y].write(&out);
			out.write((char*)&vertices[0].normal.x,4);
			out.write((char*)&vertices[0].normal.y,4);
			out.write((char*)&vertices[0].normal.z,4);
		#endif
	}else if(type==9){
		unsigned int t=vertices[0].pos.x;
		out.write((char*)&t,4);
		unsigned int tt=vertices[0].pos.y;
		out.write((char*)&tt,4);

		for(unsigned int i=0; i<vertexCount; i++){
			out.write((char*)&vertices[i+1].pos.x,4);
			out.write((char*)&vertices[i+1].pos.y,4);
			out.write((char*)&vertices[i+1].pos.z,4);
		}
	}else{
		for(unsigned int i=0; i<vertexCount; i++){
			vertices[i].write(out);
		}

	}

	//then animation
	unsigned int anisize=0;
	out.write((char*)&anisize,sizeof(anisize));


	//then envelopes
	unsigned int envsize=0;
	out.write((char*)&envsize,sizeof(envsize));

	if(type==0 || type==3){
		unsigned int bp=0;
		if(hasBasePose){bp=1;}
		out.write((char*)&bp,4);

		out.write((char*)&basePos.x,4);
		out.write((char*)&basePos.y,4);
		out.write((char*)&basePos.z,4);

		out.write((char*)&baseRot.x,4);
		out.write((char*)&baseRot.y,4);
		out.write((char*)&baseRot.z,4);

		out.write((char*)&baseScale.x,4);
		out.write((char*)&baseScale.y,4);
		out.write((char*)&baseScale.z,4);
	}


	return true;
}

void Object::buildBox(){

	//calculate bounding boxes
	box.nx=1000000;
	box.ny=1000000;
	box.nz=1000000;

	box.px=-1000000;
	box.py=-1000000;
	box.pz=-1000000;

	uv0Box.nx=1000000;
	uv0Box.ny=1000000;
	uv0Box.px=-1000000;
	uv0Box.py=-1000000;

	uv1Box.nx=1000000;
	uv1Box.ny=1000000;
	uv1Box.px=-1000000;
	uv1Box.py=-1000000;

	uv2Box.nx=1000000;
	uv2Box.ny=1000000;
	uv2Box.px=-1000000;
	uv2Box.py=-1000000;

	uv3Box.nx=1000000;
	uv3Box.ny=1000000;
	uv3Box.px=-1000000;
	uv3Box.py=-1000000;

	uv4Box.nx=1000000;
	uv4Box.ny=1000000;
	uv4Box.px=-1000000;
	uv4Box.py=-1000000;

	uv5Box.nx=1000000;
	uv5Box.ny=1000000;
	uv5Box.px=-1000000;
	uv5Box.py=-1000000;

	uv6Box.nx=1000000;
	uv6Box.ny=1000000;
	uv6Box.px=-1000000;
	uv6Box.py=-1000000;

	for(unsigned int i=0; i<vertexCount; i++){
		if(vertices[i].pos.x<box.nx){box.nx=vertices[i].pos.x;}
		if(vertices[i].pos.y<box.ny){box.ny=vertices[i].pos.y;}
		if(vertices[i].pos.z<box.nz){box.nz=vertices[i].pos.z;}

		if(vertices[i].pos.x>box.px){box.px=vertices[i].pos.x;}
		if(vertices[i].pos.y>box.py){box.py=vertices[i].pos.y;}
		if(vertices[i].pos.z>box.pz){box.pz=vertices[i].pos.z;}

		if(vertices[i].uv0.x<uv0Box.nx){uv0Box.nx=vertices[i].uv0.x;}
		if(vertices[i].uv0.y<uv0Box.ny){uv0Box.ny=vertices[i].uv0.y;}
		if(vertices[i].uv0.x>uv0Box.px){uv0Box.px=vertices[i].uv0.x;}
		if(vertices[i].uv0.y>uv0Box.py){uv0Box.py=vertices[i].uv0.y;}

		if(vertices[i].uv1.x<uv1Box.nx){uv1Box.nx=vertices[i].uv1.x;}
		if(vertices[i].uv1.y<uv1Box.ny){uv1Box.ny=vertices[i].uv1.y;}
		if(vertices[i].uv1.x>uv1Box.px){uv1Box.px=vertices[i].uv1.x;}
		if(vertices[i].uv1.y>uv1Box.py){uv1Box.py=vertices[i].uv1.y;}

		if(vertices[i].uv2.x<uv2Box.nx){uv2Box.nx=vertices[i].uv2.x;}
		if(vertices[i].uv2.y<uv2Box.ny){uv2Box.ny=vertices[i].uv2.y;}
		if(vertices[i].uv2.x>uv2Box.px){uv2Box.px=vertices[i].uv2.x;}
		if(vertices[i].uv2.y>uv2Box.py){uv2Box.py=vertices[i].uv2.y;}

		if(vertices[i].uv3.x<uv3Box.nx){uv3Box.nx=vertices[i].uv3.x;}
		if(vertices[i].uv3.y<uv3Box.ny){uv3Box.ny=vertices[i].uv3.y;}
		if(vertices[i].uv3.x>uv3Box.px){uv3Box.px=vertices[i].uv3.x;}
		if(vertices[i].uv3.y>uv3Box.py){uv3Box.py=vertices[i].uv3.y;}

		if(vertices[i].uv4.x<uv4Box.nx){uv4Box.nx=vertices[i].uv4.x;}
		if(vertices[i].uv4.y<uv4Box.ny){uv4Box.ny=vertices[i].uv4.y;}
		if(vertices[i].uv4.x>uv4Box.px){uv4Box.px=vertices[i].uv4.x;}
		if(vertices[i].uv4.y>uv4Box.py){uv4Box.py=vertices[i].uv4.y;}

		if(vertices[i].uv5.x<uv5Box.nx){uv5Box.nx=vertices[i].uv5.x;}
		if(vertices[i].uv5.y<uv5Box.ny){uv5Box.ny=vertices[i].uv5.y;}
		if(vertices[i].uv5.x>uv5Box.px){uv5Box.px=vertices[i].uv5.x;}
		if(vertices[i].uv5.y>uv5Box.py){uv5Box.py=vertices[i].uv5.y;}

		if(vertices[i].uv6.x<uv6Box.nx){uv6Box.nx=vertices[i].uv6.x;}
		if(vertices[i].uv6.y<uv6Box.ny){uv6Box.ny=vertices[i].uv6.y;}
		if(vertices[i].uv6.x>uv6Box.px){uv6Box.px=vertices[i].uv6.x;}
		if(vertices[i].uv6.y>uv6Box.py){uv6Box.py=vertices[i].uv6.y;}
	}

	rawBox=box;
}


void Object::draw(int mode){

#ifndef XSICONVERT

	if(!visible){return;}

	int tcnt=0;

	for(unsigned int v=0; v<vertexCount; v++){
		if(vertices[v].end==START_TRI || (mode==1 && (v%3)==0)){
			glBegin(GL_TRIANGLES);
		}else if(vertices[v].end==START_STRIP){
			glBegin(GL_TRIANGLE_STRIP);
		}


		glVertex3f(vertices[v].pos.x,
					vertices[v].pos.y,
					vertices[v].pos.z);


		if(vertices[v].end==END_TRI || (mode==1 && ((v+1)%3)==0)){
			glEnd();

			if(mode==1){
				tcnt++;
			}
		}else if(vertices[v].end==END_STRIP){
			glEnd();
		}

	}

#endif
}	

#ifndef XSICONVERT
#ifdef DEVELOPER
void Object::drawPolygon(){
	draw(1);
}
#endif
#endif

#ifndef XSICONVERT
#ifdef DEVELOPER
void Object::drawEdge(){


	if(!visible){return;}

	glLineWidth(3);

	for(int v=0; v<getVertexCount(); v+=3){

		glBegin(GL_LINES);
		glVertex3f(getVertexRaw(v).pos.x,getVertexRaw(v).pos.y,getVertexRaw(v).pos.z);
		glVertex3f(getVertexRaw(v+1).pos.x,getVertexRaw(v+1).pos.y,getVertexRaw(v+1).pos.z);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(getVertexRaw(v+1).pos.x,getVertexRaw(v+1).pos.y,getVertexRaw(v+1).pos.z);
		glVertex3f(getVertexRaw(v+2).pos.x,getVertexRaw(v+2).pos.y,getVertexRaw(v+2).pos.z);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(getVertexRaw(v+2).pos.x,getVertexRaw(v+2).pos.y,getVertexRaw(v+2).pos.z);
		glVertex3f(getVertexRaw(v).pos.x,getVertexRaw(v).pos.y,getVertexRaw(v).pos.z);
		glEnd();

	}

}
#endif
#endif

#ifndef XSICONVERT
#ifdef DEVELOPER
void Object::drawVertex(){

	if(!visible){return;}

	glPointSize(10);

	for(int v=0; v<getVertexCount(); v++){
		glBegin(GL_POINTS);
		glVertex3f(getVertexRaw(v).pos.x,getVertexRaw(v).pos.y,getVertexRaw(v).pos.z);
		glEnd();
	}

	
	glPointSize(1);

}
#endif
#endif

void Object::drawNormals(){

#ifndef XSICONVERT

	if(conf->normalsSolo!=name && conf->normalsSolo!=""){
		return;
	}


#ifndef SOFTWARE_TRANSFORMS
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
			
	
	glRotatef(rot.z,0.0f,0.0f,1.0f);
	glRotatef(rot.y,0.0f,1.0f,0.0f);
	glRotatef(rot.x,1.0f,0.0f,0.0f);

	glScalef(scale.x,scale.y,scale.z);



#endif

	for(unsigned int v=0; v<vertices.size(); v++){
		
		glColor4f(0,0,1,1);

		glBegin(GL_LINES);
		glVertex3f(vertices[v].pos.x,vertices[v].pos.y,vertices[v].pos.z);
		glVertex3f(vertices[v].pos.x+vertices[v].normal.x,vertices[v].pos.y+vertices[v].normal.y,vertices[v].pos.z+vertices[v].normal.z);
		glEnd();

		glColor4f(1,0,0,1);
		glBegin(GL_LINES);
			glVertex3f(vertices[v].pos.x,vertices[v].pos.y,vertices[v].pos.z);

			glVertex3f(vertices[v].pos.x+vertices[v].tangent.x,
				vertices[v].pos.y+vertices[v].tangent.y,
				vertices[v].pos.z+vertices[v].tangent.z);

		glEnd();

		glColor4f(0,1,0,1);
		glBegin(GL_LINES);
			glVertex3f(vertices[v].pos.x,vertices[v].pos.y,vertices[v].pos.z);

			glVertex3f(vertices[v].pos.x+vertices[v].binormal.x,
				vertices[v].pos.y+vertices[v].binormal.y,
				vertices[v].pos.z+vertices[v].binormal.z);

		glEnd();

	}
#ifndef SOFTWARE_TRANSFORMS
	glPopMatrix();
#endif
#endif

}


void Object::drawBB(){

#ifndef XSICONVERT
	glColor4f(1,1,0,1);

	glBegin(GL_LINES);
		glVertex3f(box.nx,box.ny,box.nz);
		glVertex3f(box.px,box.ny,box.nz);

		glVertex3f(box.nx,box.ny,box.nz);
		glVertex3f(box.nx,box.py,box.nz);

		glVertex3f(box.nx,box.ny,box.nz);
		glVertex3f(box.nx,box.ny,box.pz);


		glVertex3f(box.px,box.ny,box.nz);
		glVertex3f(box.px,box.py,box.nz);

		glVertex3f(box.px,box.ny,box.pz);
		glVertex3f(box.px,box.py,box.pz);
		
		glVertex3f(box.nx,box.ny,box.pz);
		glVertex3f(box.nx,box.py,box.pz);

		glVertex3f(box.nx,box.py,box.nz);
		glVertex3f(box.px,box.py,box.nz);

		glVertex3f(box.nx,box.py,box.nz);
		glVertex3f(box.nx,box.py,box.pz);


		glVertex3f(box.px,box.ny,box.nz);
		glVertex3f(box.px,box.ny,box.pz);

		glVertex3f(box.px,box.py,box.nz);
		glVertex3f(box.px,box.py,box.pz);

		glVertex3f(box.nx,box.ny,box.pz);
		glVertex3f(box.px,box.ny,box.pz);

		glVertex3f(box.nx,box.py,box.pz);
		glVertex3f(box.px,box.py,box.pz);
	glEnd();

	glColor4f(1,0,0,1);

	glPointSize(10);

	glBegin(GL_POINTS);
		FloatVector3d c=box.center();
		glVertex3f(c.x,c.y,c.z);
	glEnd();

	glPointSize(1);
#endif
}


Object::Object(){
	vertexCount=0;
	flags=0;
	lightmap=-1;
	material=-1;
	oldMaterial=-1;

	sceneGraphLevel=0;

	dirty=true;
	glListName=0;
	glListNameQuick=0;

	#ifdef DEVELOPER
		glListNameQuickPolygon=0;
		listBuiltQuickPolygon=false;
	#endif

	listBuilt=false;
	listBuiltQuick=false;

	nofog=false;
	noclip=false;

	collide=false;
	oldCollide=false;

	oldTex0scale=FloatVector2d(1,1);
	oldTex1scale=FloatVector2d(1,1);
	oldTex2scale=FloatVector2d(1,1);
	oldTex3scale=FloatVector2d(1,1);
	oldTex4scale=FloatVector2d(1,1);
	oldTex5scale=FloatVector2d(1,1);
	oldTex6scale=FloatVector2d(1,1);
	oldTexLightmapScale=FloatVector2d(1,1);

	rebuildTransformedVertexCache=true;

	boxBuilt=false;

	rotMode="global";

	constraintPos=NULL;
	constraintRot=NULL;
	constraintScale=NULL;

	hasBasePose=false;

	envelopeVertices=NULL;
	envelopeVerticesCount=0;
	glListNameEnvelope=NULL;

	#ifdef ODE
		mass=1;
		friction=0;
		bounce=0;
		bounceThreshold=0;
		collideType="triangle";
		oldCollideType="";
		dynamic=false;


		geomIdInit=false;
		bodyIdInit=false;
	#endif

	scale.x=1;
	scale.y=1;
	scale.z=1;
}

void Material::update(){

	#ifndef XSICONVERT
	dirty=false;
	

	if(ambient!=oldAmbient){
		oldAmbient=ambient;
		dirty=true;
	}

	if(specular!=oldSpecular){
		oldSpecular=specular;
		dirty=true;
	}


	if(shiny!=oldShiny){
		oldShiny=shiny;
		dirty=true;
	}

	if(nonstaticRefs>0){
		if(oldNonstaticRefs==0 && !uploaded){
			level->luaUpload(this);
			uploaded=true;
		}
	}


	oldNonstaticRefs=nonstaticRefs;


	if(diffuse!=oldDiffuse){
		

		if(diffuse.w<1 && diffuse.w!=oldDiffuse.w){
			//check to make sure it's in the list of transparent obejcts

			for(int i=0; i<nonstaticRefs; i++){
				if(level->transparentObjects.find(nonstaticRefList[i])!=-1){

				}else{
					level->transparentObjects.pushBack(nonstaticRefList[i]);

					int nsr=level->nonstaticObjects.find(nonstaticRefList[i]);

					if(nsr!=-1){
						level->nonstaticObjects.erase(nsr);
					}else{

						logs().renderer.write("MAJOR ERROR LOLOLOL");
					}

				}

			}

		}

		oldDiffuse=diffuse;

		dirty=true;
	}

#endif
}

#ifndef XSICONVERT

bool Material::deleteTexture(String type){
	if(type.toLower()=="diffuse"){
		textureDiffuse=-1;
	}else if(type.toLower()=="specular"){
		textureSpecular=-1;
	}else if(type.toLower()=="layer1"){
		textureEnvironmentFront=-1;
	}else if(type.toLower()=="layer2"){
		textureEnvironmentLeft=-1;
	}else if(type.toLower()=="layer3"){
		textureEnvironmentTop=-1;
	}else if(type.toLower()=="mask1"){
		textureEnvironmentBack=-1;
	}else if(type.toLower()=="mask2"){
		textureEnvironmentRight=-1;
	}else if(type.toLower()=="mask3"){
		textureEnvironmentBottom=-1;
	}else if(type.toLower()=="bump"){
		textureBump=-1;
	}else if(type.toLower()=="displacement"){
		textureDisplacement=-1;
	}else{
		console().write("unknown texture type "+type);
		return false;
	}

	dirty=true;

	return true;
}

bool Material::addTexture(String type, Path file){

	if(file==""){
		console().write("error, bad texture file");
		return false;
	}

	level->textureCount++;
	TextureImage t;
	t.ref=true;
	t.file=file.getAbsolute();
	t.fileRelative=file.getRelativeTo(level->file.getRelativeDirectory());

	String name;

	Array<String> a=file.getRelative().explode(".");

	for(int i=0; i<a.size()-1; i++){
		name+=a[i];
	}

	Array<String> b=name.explode("/");

	name=b[b.size()-1];

	t.name=name;
	
	if(type.toLower()=="bump"){
		ImageData imagedata=loadImage(file);

		ImageData f=sobel(imagedata);

		t.x=f.width;
		t.y=f.height;
		t.data=f.data;
		t.bytesPP=f.bytesPP;

		texture.load(&t);

		level->textures.pushBack(t);

		delete[] t.data;
		t.dataLoaded=new bool;
		*t.dataLoaded=false;

	}else{
		int id=texture.load(file);

		ImageData imd=texture.info(id);

		t.bytesPP=imd.bytesPP;
		t.x=imd.width;
		t.y=imd.height;
		t.glName(id);

		level->textures.pushBack(t);
	}

	if(type.toLower()=="diffuse" || type.toLower()=="base"){
		textureDiffuse=level->textures.size()-1;
	}else if(type.toLower()=="specular"){
		textureSpecular=level->textures.size()-1;
	}else if(type.toLower()=="layer1" || type.toLower()=="color 1"  || type.toLower()=="front"){
		textureEnvironmentFront=level->textures.size()-1;
	}else if(type.toLower()=="layer2" || type.toLower()=="color 2"  || type.toLower()=="left"){
		textureEnvironmentLeft=level->textures.size()-1;
	}else if(type.toLower()=="layer3" || type.toLower()=="color 3"  || type.toLower()=="top"){
		textureEnvironmentTop=level->textures.size()-1;
	}else if(type.toLower()=="mask1" || type.toLower()=="mask 1"  || type.toLower()=="back"){
		textureEnvironmentBack=level->textures.size()-1;
	}else if(type.toLower()=="mask2" || type.toLower()=="mask 2"  || type.toLower()=="right"){
		textureEnvironmentRight=level->textures.size()-1;
	}else if(type.toLower()=="mask3" || type.toLower()=="mask 3"  || type.toLower()=="bottom"){
		textureEnvironmentBottom=level->textures.size()-1;
	}else if(type.toLower()=="bump"){
		textureBump=level->textures.size()-1;
	}else if(type.toLower()=="displacement"){
		textureDisplacement=level->textures.size()-1;
	}else{
		console().write("unknown texture type "+type);
		return false;
	}

	dirty=true;

	for(int i=0; i<nonstaticRefList.size(); i++){
		nonstaticRefList[i]->dirty=true;
		nonstaticRefList[i]->listBuilt=false;
	}

	return true;
}
#endif

Vertex Object::transformVertex(Vertex v,FloatVector3d pos,FloatVector3d rot, FloatVector3d scale){
	Vertex final=v;

	
	final.pos.x=final.pos.x*scale.x;
	final.pos.y=final.pos.y*scale.y;
	final.pos.z=final.pos.z*scale.z;

	
	if(rot.x!=0){	//rot.y

		float dist;	
		float ndist;	
		float nbdist;	
		float tdist;	
		FloatVector2d v1;
		FloatVector2d v2;
		FloatVector2d nv1;
		FloatVector2d nv2;
		FloatVector2d bnv2;
		FloatVector2d tnv2;
		float normalAngleNew=0;
		float normalAngle=0;

		float binormalAngleNew=0;
		float binormalAngle=0;

		float tangentAngleNew=0;
		float tangentAngle=0;

		float angleNew=0;
		float angle=0;

		v1.x=0;
		v1.y=-1;	//must be -1 for some reason
		
		v2.x=final.pos.z;
		v2.y=final.pos.y;
			
		angle=math.vectorToAngle(v1,v2);
		angleNew=angle+rot.x;

		nv1.x=0;
		nv1.y=-1;	//must be -1 for some reason
		

		nv2.x=final.normal.z;
		nv2.y=final.normal.y;
			
		bnv2.x=final.binormal.z;
		bnv2.y=final.binormal.y;

		tnv2.x=final.tangent.z;
		tnv2.y=final.tangent.y;

		normalAngle=math.vectorToAngle(nv1,nv2);
		normalAngleNew=normalAngle+rot.x;

		binormalAngle=math.vectorToAngle(nv1,bnv2);
		binormalAngleNew=binormalAngle+rot.x;

		tangentAngle=math.vectorToAngle(nv1,tnv2);
		tangentAngleNew=tangentAngle+rot.x;


		dist=math.distance(0,0,final.pos.z,final.pos.y);

		ndist=math.distance(0,0,final.normal.z,final.normal.y);

		nbdist=math.distance(0,0,final.binormal.z,final.binormal.y);

		tdist=math.distance(0,0,final.tangent.z,final.tangent.y);

		if(final.pos.z!=0 || final.pos.y!=0){
			final.pos.z=(- cos((angleNew-90) * PI/180) * dist);	
			final.pos.y=(sin((angleNew-90) * PI/180) * dist);
		}

		if(final.normal.z!=0 || final.normal.y!=0){
			final.normal.z=(- cos((normalAngleNew-90) * PI/180) * ndist);
			final.normal.y=(sin((normalAngleNew-90) * PI/180) * ndist);
		}
			
		if(final.binormal.z!=0 || final.binormal.y!=0){
			final.binormal.z=(- cos((binormalAngleNew-90) * PI/180) * nbdist);
			final.binormal.y=(sin((binormalAngleNew-90) * PI/180) * nbdist);
		}

		if(final.tangent.z!=0 || final.tangent.y!=0){
			final.tangent.z=(- cos((tangentAngleNew-90) * PI/180) * tdist);
			final.tangent.y=(sin((tangentAngleNew-90) * PI/180) * tdist);
		}
	}

	if(rot.y!=0){	//rot.y

		float dist;	
		float ndist;
		float nbdist;
		float tdist;
		FloatVector2d v1;
		FloatVector2d v2;
		FloatVector2d nv1;
		FloatVector2d nv2;
		FloatVector2d bnv2;
		FloatVector2d tnv2;
		float normalAngleNew=0;
		float normalAngle=0;

		float binormalAngleNew=0;
		float binormalAngle=0;

		float tangentAngleNew=0;
		float tangentAngle=0;

		float angleNew=0;
		float angle=0;

		v1.x=0;
		v1.y=-1;	//must be -1 for some reason
		
		v2.x=final.pos.x;
		v2.y=final.pos.z;
			
		angle=math.vectorToAngle(v1,v2);
		angleNew=angle+rot.y;

		nv1.x=0;
		nv1.y=-1;	//must be -1 for some reason
		

		nv2.x=final.normal.x;
		nv2.y=final.normal.z;
			
		bnv2.x=final.binormal.x;
		bnv2.y=final.binormal.z;
			
		tnv2.x=final.tangent.x;
		tnv2.y=final.tangent.z;
			
		normalAngle=math.vectorToAngle(nv1,nv2);
		normalAngleNew=normalAngle+rot.y;

		binormalAngle=math.vectorToAngle(nv1,bnv2);
		binormalAngleNew=binormalAngle+rot.y;

		tangentAngle=math.vectorToAngle(nv1,tnv2);
		tangentAngleNew=tangentAngle+rot.y;

		

		dist=math.distance(0,0,final.pos.x,final.pos.z);

		ndist=math.distance(0,0,final.normal.x,final.normal.z);

		nbdist=math.distance(0,0,final.binormal.x,final.binormal.z);

		tdist=math.distance(0,0,final.tangent.x,final.tangent.z);

		if(final.pos.x!=0 || final.pos.z!=0){
			final.pos.x=(- cos((angleNew-90) * PI/180) * dist);	
			final.pos.z=(sin((angleNew-90) * PI/180) * dist);
		}

		if(final.normal.x!=0 || final.normal.z!=0){
			final.normal.x=(- cos((normalAngleNew-90) * PI/180) * ndist);
			final.normal.z=(sin((normalAngleNew-90) * PI/180) * ndist);
		}

		if(final.binormal.x!=0 || final.binormal.z!=0){
			final.binormal.x=(- cos((binormalAngleNew-90) * PI/180) * nbdist);
			final.binormal.z=(sin((binormalAngleNew-90) * PI/180) * nbdist);
		}

		if(final.tangent.x!=0 || final.tangent.z!=0){
			final.tangent.x=(- cos((tangentAngleNew-90) * PI/180) * tdist);
			final.tangent.z=(sin((tangentAngleNew-90) * PI/180) * tdist);
		}
					
	}

	
	if(rot.z!=0){	

		float dist;	
		float ndist;	
		float nbdist;	
		float tdist;	
		FloatVector2d v1;
		FloatVector2d v2;
		FloatVector2d nv1;
		FloatVector2d nv2;
		FloatVector2d bnv2;
		FloatVector2d tnv2;
		float normalAngleNew=0;
		float normalAngle=0;

		float binormalAngleNew=0;
		float binormalAngle=0;

		float tangentAngleNew=0;
		float tangentAngle=0;

		float angleNew=0;
		float angle=0;

		v1.x=0;
		v1.y=-1;	//must be -1 for some reason
		
		v2.x=final.pos.y;
		v2.y=final.pos.x;
			
		angle=math.vectorToAngle(v1,v2);
		angleNew=angle+rot.z;

		nv1.x=0;
		nv1.y=-1;	//must be -1 for some reason

		nv2.x=final.normal.y;
		nv2.y=final.normal.x;

		bnv2.x=final.binormal.y;
		bnv2.y=final.binormal.x;

		tnv2.x=final.tangent.y;
		tnv2.y=final.tangent.x;
			
		normalAngle=math.vectorToAngle(nv1,nv2);
		normalAngleNew=normalAngle+rot.z;

		binormalAngle=math.vectorToAngle(nv1,bnv2);
		binormalAngleNew=binormalAngle+rot.z;

		tangentAngle=math.vectorToAngle(nv1,tnv2);
		tangentAngleNew=tangentAngle+rot.z;

		dist=math.distance(0,0,final.pos.y,final.pos.x);

		ndist=math.distance(0,0,final.normal.y,final.normal.x);

		nbdist=math.distance(0,0,final.binormal.y,final.binormal.x);

		tdist=math.distance(0,0,final.tangent.y,final.tangent.x);

		if(final.pos.y!=0 || final.pos.x!=0){
			final.pos.y=(- cos((angleNew-90) * PI/180) * dist);	
			final.pos.x=(sin((angleNew-90) * PI/180) * dist);	
		}

		if(final.normal.y!=0 || final.normal.x!=0){
			final.normal.y=(- cos((normalAngleNew-90) * PI/180) * ndist);	
			final.normal.x=(sin((normalAngleNew-90) * PI/180) * ndist);	
		}

		if(final.binormal.y!=0 || final.binormal.x!=0){
			final.binormal.y=(- cos((binormalAngleNew-90) * PI/180) * nbdist);	
			final.binormal.x=(sin((binormalAngleNew-90) * PI/180) * nbdist);	
		}
			
		if(final.tangent.y!=0 || final.tangent.x!=0){
			final.tangent.y=(- cos((tangentAngleNew-90) * PI/180) * tdist);	
			final.tangent.x=(sin((tangentAngleNew-90) * PI/180) * tdist);	
		}
			
					
	}
	
	final.pos.x=final.pos.x+pos.x;
	final.pos.y=final.pos.y+pos.y;
	final.pos.z=final.pos.z+pos.z;

	return final;
}

Vertex& Object::getVertex(int id){
#ifdef SOFTWARE_TRANSFORMS
	return vertices[id];
#else

	if(rebuildTransformedVertexCache){
		while(transformedVertexCache.size()<vertices.size()){
			transformedVertexCache.pushBack();
		}

		for(int i=0; i<vertices.size(); i++){
			transformedVertexCache[i]=transformVertex(vertices[i],pos,rot,scale);
		}

		rebuildTransformedVertexCache=false;
	}

	return transformedVertexCache[id];

#endif
}

Vertex& Object::getVertexRaw(int id){
#ifdef SOFTWARE_TRANSFORMS
	return vertices[id];
#else
	return vertices[id];
#endif
}

Vertex& Object::setVertexRaw(int id){
	boxBuilt=false;
	dirty=true;
#ifdef SOFTWARE_TRANSFORMS
	return vertices[id];
#else
	return vertices[id];
#endif
}


int Object::getVertexCount(){
	return vertexCount;
}

void Object::setVertexCount(int i){
	vertexCount=i;
}

void Object::reserveVertices(int count){
	vertices.reserve(count);
}

void Object::addVertex(Vertex v){
	vertices.pushBack(v);
}

void Object::addVertex(){
	vertices.pushBack();
}

Array<Vertex>* Object::Vertices(){
	return &vertices;
}

void Object::become(Object* src){
	pos=src->pos;
	scale=src->scale;
	rot=src->rot;
	visible=src->visible;
	material=src->material;
	lightmap=src->lightmap;
	type=src->type;
	flags=src->flags;

	for(int i=0; i<src->vertices.size(); i++){
		vertices.pushBack(src->vertices[i]);
	}

	vertexCount=src->vertexCount;
}
