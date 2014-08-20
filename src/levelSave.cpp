#include "level.hpp"
#include "script.hpp"

#include <iostream>
#include <fstream>

#include "GlHeader.hpp"

#include "Helperlib/Log.hpp"
#include "Helperlib/HelperLibMath.hpp"

#include "FileIo.hpp"
#include "Bmp.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "light.h"
#include "Filters.h"
#include "conf.h"

#include "foam.hpp"
#include "draw.hpp"

#include "os.h"
#include "timeing.hpp"

#include "script.hpp"

#include "endian.hpp"

#include "rsa.h"
#include "SceneGraph.hpp"

using namespace std;

unsigned long getTotalObjCnt(SceneGraphNode* node){

	unsigned long cnt=0;

	for(int i=0; i<node->children.size(); i++){

		cnt+=1;
		cnt+=getTotalObjCnt(&node->children[i]);
	}

	return cnt;
}

unsigned long getTexturesSize(){
	unsigned long total=0;

	total+=4*level->textureCount;

	for(int i=0; i<level->textureCount; i++){
		total+=level->textures[i].size();
	}

	return total;
}


unsigned long getMaterialsSize(){
	unsigned long total=0;

	total+=4*level->materials.size();

	for(int i=0; i<level->materials.size(); i++){
		total+=level->materials[i].size();
	}

	return total;
}

unsigned long getLightmapsSize(){
	unsigned long total=0;

	total+=4*level->lightmapCount;

	for(int i=0; i<level->lightmapCount; i++){
		total+=level->lightmaps[i].size();
	}

	return total;
}


String relative(String f, String lmlFile){

	f.replace("/","\\");
	Array<String> source=f.explode("\\");

	lmlFile.replace("/","\\");
	Array<String> target=lmlFile.explode("\\");

	if(source.size()<1 || target.size()<1){return f;}

	if(source[0]!=target[0]){return f;}

	int levels=0;
	int i=1;
	
	while(source.size()>i && target.size()>1){
		if(source[levels]==target[levels]){
			levels++;
		}else{
			break;
		}

		i++;
	}

	i=levels;
	while(i>0){
		source.erase(0);
		target.erase(0);
		i--;
	}

	String final;

	for(int i=0; i<target.size()-1; i++){
		final+="..\\";
	}

	for(int i=0; i<source.size(); i++){
		final+=source[i]+"\\";
	}

	final=final.substrIndex(0,final.size()-1);	//remove trailing /

	return final;
}


void writeObjects(SceneGraphNode* node, ofstream* out, unsigned int level=0){

	

	if(node->parent!=NULL){
		if(node->object.type==OBJECT){
			node->object.object->sceneGraphLevel=level;
			node->object.object->write(*out);
		}else if(node->object.type==NULL3D){
			node->object.null->sceneGraphLevel=level;
			node->object.null->write(*out);
		}else if(node->object.type==IK_EFFECTOR){
			node->object.ikEffector->sceneGraphLevel=level;
			node->object.ikEffector->write(*out);
		}else if(node->object.type==IK_ROOT){
			node->object.ikRoot->sceneGraphLevel=level;
			node->object.ikRoot->write(*out);
		}else if(node->object.type==IK_JOINT){
			node->object.ikJoint->sceneGraphLevel=level;
			node->object.ikJoint->write(*out);
		}else if(node->object.type==LIGHT){
			node->object.light->sceneGraphLevel=level;
			node->object.light->write(*out);
		}else if(node->object.type==CAMERA){
			node->object.camera->sceneGraphLevel=level;
			node->object.camera->write(*out);
		}else{

		}	
	}

	level++;

	for(int i=0; i<node->children.size(); i++){
		writeObjects(&node->children[i],out,level);
	}

	return;
}

unsigned long writeObjectsHeader(SceneGraphNode* node, ofstream* out, unsigned long fpos){

	

	if(node->parent!=NULL){
		out->write((char*)&fpos,4);

		if(node->object.type==OBJECT){
			unsigned long us=node->object.object->size();
			fpos+=us;
		}else if(node->object.type==LIGHT){
			unsigned long us=node->object.light->size();
			fpos+=us;
		}else if(node->object.type==NULL3D){
			unsigned long us=node->object.null->size();
			fpos+=us;
		}else if(node->object.type==IK_EFFECTOR){
			
		}else if(node->object.type==IK_ROOT){
			
		}else if(node->object.type==IK_JOINT){
			
		}else if(node->object.type==CAMERA){

		}else{

		}	

		
	}else{

	}

	
	for(int i=0; i<node->children.size(); i++){
		unsigned long lol=writeObjectsHeader(&node->children[i],out,fpos);
		fpos=lol;
	}
 
	return fpos;
}

bool Level::savePrep(Path outfile, bool developer){
	//prep textures
	for(int i=0; i<textureCount; i++){

		if(!developer && textures[i].ref){
			textures[i].ref=false;
		}

		if(!developer && textures[i].file!=""){
			ImageData imagedata=loadImage(textures[i].file);
			textures[i].data=imagedata.data;
		}

		if(textures[i].ref){
			textures[i].fileRelative=relative(textures[i].file,outfile.getRelative());
		}

		
		if(developer && !textures[i].ref && textures[i].file!=""){
			textures[i].ref=true;
		}

		if(!textures[i].ref && textures[i].file=="" && !*textures[i].dataLoaded){
			//we need to load the level file + reload the data. eek.
		}
	}

	//prep lightmaps
	for(int i=0; i<lightmapCount; i++){

		if(!developer && lightmaps[i].ref){
			lightmaps[i].ref=false;
		}else if(developer){
			lightmaps[i].ref=true;
		}

		if(!developer && lightmaps[i].file!=""){
			ImageData imagedata=loadImage(lightmaps[i].file);
			lightmaps[i].data=imagedata.data;
		}

		if(lightmaps[i].ref){
			lightmaps[i].fileRelative=relative(lightmaps[i].file,outfile.getRelative());
		}
	}

	for(int i=0; i<objects.size(); i++){
		if(objects[i]->visible){
			objects[i]->flags|=OBJECT_VISIBLE;
		}else{
			objects[i]->flags&=~OBJECT_VISIBLE;
		}
	}

	return true;
}

bool Level::save(Path outfile, bool developer){
	update();	//make sure everything is up to date before we write

	savePrep(outfile,developer);

	ofstream out(outfile.getRelative().c_str(), ios::out|ios::binary);
  	
	out.write("LML2",4);

	unsigned long filePos=0;

	filePos=4;

	char* key=new char[256];
	for(int i=0; i<256; i++){key[i]=0;}
	out.write(key,256);
	filePos+=256;

	out.write((char*)&textureCount,4);
	filePos+=4;
	
	out.write((char*)&materialCount,4);
	filePos+=4;

	out.write((char*)&lightmapCount,4);
	filePos+=4;

	unsigned long objcnt=getTotalObjCnt(&sceneGraph.root);

	out.write((char*)&objcnt,4);
	filePos+=4;

	unsigned long tfilepos=filePos+16;

	out.write((char*)&tfilepos,4);	//start pos of textures
	filePos+=4;

	unsigned int texturesize=getTexturesSize();
	tfilepos+=texturesize;
	out.write((char*)&tfilepos,4);	//start pos of materials
	filePos+=4;

	unsigned int materialsize=getMaterialsSize();
	tfilepos+=materialsize;
	out.write((char*)&tfilepos,4);	//start pos of lightmaps
	filePos+=4;

	unsigned int lightmapsize=getLightmapsSize();
	tfilepos+=lightmapsize;
	out.write((char*)&tfilepos,4);	//start pos of objects
	filePos+=4;
	
	unsigned long texpos=filePos+textureCount*4;

	for(int i=0; i<textureCount; i++){
		out.write((char*)&texpos,4);
		texpos+=textures[i].size();
		filePos+=4;
	}
	
	for(int i=0; i<textureCount; i++){
		textures[i].write(out);
		unsigned long l=textures[i].size();
		filePos+=l;
	}

	unsigned long matpos=filePos+materialCount*4;

	for(int i=0; i<materials.size(); i++){
		out.write((char*)&matpos,4);
		matpos+=materials[i].size();
		filePos+=4;
	}

	for(int i=0; i<materials.size(); i++){
		materials[i].write(out);
		filePos+=materials[i].size();
	}

	unsigned long lightpos=filePos+lightmapCount*4;

	for(int i=0; i<lightmapCount; i++){
		out.write((char*)&lightpos,4);
		lightpos+=lightmaps[i].size();
		filePos+=4;
	}

	for(int i=0; i<lightmapCount; i++){
		lightmaps[i].write(out);
		filePos+=lightmaps[i].size();
	}


	writeObjectsHeader(&sceneGraph.root,&out,filePos+objcnt*4);
	
	writeObjects(&sceneGraph.root,&out);

	return true;
}
