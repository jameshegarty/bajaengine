#include "textureLibrary.h"
#include "script.hpp"
#include "FileIo.hpp"
#include "Bmp.hpp"
#include "Tga.hpp"
#include "jpg.hpp"
#include "texture.hpp"
#include "hardware.hpp"
#include "level.hpp"
#include "endian.hpp"

#include "GlHeader.hpp"

#include <iostream>
#include <fstream>

#include "os.h"

#include "rsa.h"

#include "leaks.h"

TextureLibrary textureLibrary;

using namespace std; 

/* int luaTextureLibraryLoad(lua_State *L)
 * helper for LUA function textureLibrary.load()
 */
int luaTextureLibraryLoad(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error: loadLibrary takes one argument");
		return 0;
	}

	if(args[0].type==SCRIPT_STRING_STORED){

		textureLibrary.loadLibrary(args[0].value.ss);
	}else{
		console().write("Error: loadLibrary bad arguments");
		return 0;
	}

	return 0;
}

int luaTextureLoad(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1 && args.size()!=2){
		console().write("Error: load takes one argument");
		script().result(L,false);
		return 1;
	}

	if(args[0].type==SCRIPT_STRING_STORED){

		if(args.size()==2){
			if(args[1].type==SCRIPT_STRING_STORED){
				ImageData im=loadImage(args[0].value.ss,args[1].value.ss);
			
				if(im.loadSuccess){
					script().result(L,true);
				}else{	
					script().result(L,false);
				}

				return 1;
			}else{
				console().write("Error: load bad arguments");
				script().result(L,false);
				return 1;
			}
		}else{
			ImageData im=loadImage(args[0].value.ss);

			if(im.loadSuccess){
				script().result(L,true);
			}else{	
				script().result(L,false);
			}

			return 1;
		}
	}else{
		console().write("Error: load bad arguments");
		script().result(L,false);
		return 1;
	}
}


int luaTextureAdd(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=4){
		console().write("Error: add takes four arguments");
		return 0;
	}

	String name;
	unsigned int x;
	unsigned int y;
	unsigned int bytesPP;

	if(args[0].type==SCRIPT_STRING_STORED){
		name=args[0].value.ss;
		
	}else{
		console().write("Error: add bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){
		x=args[1].value.ds;
	}else{
		console().write("Error: add bad arguments");
		return 0;
	}

	if(args[2].type==SCRIPT_DOUBLE_STORED){
		y=args[2].value.ds;
	}else{
		console().write("Error: add bad arguments");
		return 0;
	}

	if(args[3].type==SCRIPT_DOUBLE_STORED){
		bytesPP=args[3].value.ds;
	}else{
		console().write("Error: add bad arguments");
		return 0;
	}

	EmptyTexture(name,x,y,bytesPP);

	return 0;
}



int luaTextureDelete(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error: add takes four arguments");
		return 0;
	}

	String name;
	

	if(args[0].type==SCRIPT_STRING_STORED){
		name=args[0].value.ss;
		
	}else{
		console().write("Error: add bad arguments");
		return 0;
	}

	int i;
	for(i=0; i<level->textures.size(); i++){
		if(level->textures[i].name==name){
			break;
		}
	}

	int l;
	for(l=0; l<level->lightmaps.size(); l++){
		if(level->lightmaps[l].name==name){
			break;
		}
	}

	int j=0;

	for(j=0; j<loadedImages.size(); j++){
		if(loadedImages[j].name==name){
			break;
		}
	}

	if(i!=level->textures.size() || j!=loadedImages.size() || l!=level->lightmaps.size()){

		if(j!=loadedImages.size()){

			if(*loadedImages[j].dataLoaded){
				delete[] loadedImages[j].data;
				leaks.rem("ImageData::data");
			}

			loadedImages.erase(j);
		}

		if(i!=level->textures.size()){
			level->textures.erase(i);
			level->textureCount--;
		}
		if(l!=level->lightmaps.size()){
			level->lightmaps.erase(l);
			level->lightmapCount--;
		}

		script().run("image[\""+name+"\"]=nil");

		
	}else{
		console().write("error, texture '"+name+"' not found to delete");
		return 0;
	}

	return 0;
}

/* bool TextureLibrary::loadLibrary(Path file)
 * Load the texture library (LMT) at given path
 */
bool TextureLibrary::loadLibrary(Path file){

	#ifdef RSA
		String r=rsa_verify_file(os().path(file).c_str());

		if(r!=""){
			os().error("loadLibrary Error: "+r);
			console().write("KEY ERROR: "+r);
			return false;
		}

	#endif

	ifstream inf (file.getAbsolute().c_str(),ios::in|ios::binary);

	if(inf==NULL || !inf.is_open()){
		console().write("error, library file "+file.getAbsolute()+" not found");
		return false;
	}

	inf.seekg (0, ios::beg);

	char head[4];

	inf.read(head,4);

	if(head[0]!='L' ||
		head[1]!='M' ||
		head[2]!='T' ||
		head[3]!='1'){

		console().write("error, bad file "+file.getRelative()+" ");

		return false;
	}

	//the first 128 bits are the sig, the next 128 are NULL
	inf.seekg(256+4,ios::beg);

	unsigned int tcount;

	inf.read((char*)&tcount,sizeof(unsigned int));
	tcount=endian(tcount);
	
	libraries.pushBack();

	libraries[libraries.size()-1].file=file.getRelative();

	for(int i=0; i<tcount; i++){
		libraries[libraries.size()-1].files.pushBack();
		libraries[libraries.size()-1].files[i].read(&inf);

		libraries[libraries.size()-1].pos.pushBack();
		inf.read((char*)&libraries[libraries.size()-1].pos[i],sizeof(unsigned int));
		libraries[libraries.size()-1].pos[i]=endian(libraries[libraries.size()-1].pos[i]);
	}

	inf.close();

	return true;
}

/* int TextureLibrary::load(Path texturefile)
 * Load the texture file at the given path FROM the loaded library
 */
int TextureLibrary::load(Path texturefile){
	Path libraryfile;
	String file;
	unsigned int pos;
	int kpos=0;
	int lid=0;

	for(int i=0; i<libraries.size(); i++){
		for(int k=0; k<libraries[i].files.size(); k++){
			if(libraries[i].files[k]==texturefile.getRelative()){
				libraryfile=libraries[i].file;
				file=libraries[i].files[k];
				pos=libraries[i].pos[k];
				kpos=k;
				lid=i;
			}

		}

	}


	if(!libraryfile.set()){
		return -1;
	}

	if(kpos==0){
		pos=4;
		pos+=sizeof(unsigned int);
		pos+=256;	//sig
		for(int i=0; i<libraries[lid].files.size(); i++){
			pos+=sizeof(unsigned int);
			pos+=libraries[lid].files[i].filesize();
		}

	}else{
		pos=4;
		pos+=sizeof(unsigned int);
		pos+=256;	//sig
		for(int i=0; i<libraries[lid].files.size(); i++){
			pos+=sizeof(unsigned int);
			pos+=libraries[lid].files[i].filesize();
		}
		pos+=libraries[lid].pos[kpos-1];
	}

	FILE* f;

	if(file.size()<5){
		return -1;
	}


	ImageData image;


	if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".bmp"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		fseek(f,pos,SEEK_SET);
		unsigned int len=libraries[lid].pos[kpos]-libraries[lid].pos[kpos-1];
		image=bmp.load(f,file,len);
	}else if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".tga"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		fseek(f,pos,SEEK_SET);
		image=tga.load(f,file);
	}else if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".jpg"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		fseek(f,pos,SEEK_SET);
		image=jpg.load(f,file);
	}else{
		image.error="unknown file type '"+file.substr(file.size()-4,file.size()-1)+"'";

		texture.infostore[-1]=image;
		return -1;
	}

	if(image.loadSuccess){

			image.offsetBL.x=0;
			image.offsetBL.y=0;
			image.offsetBR.x=0;
			image.offsetBR.y=0;
			image.offsetTL.x=0;
			image.offsetTL.y=0;
			image.offsetTR.x=0;
			image.offsetTR.y=0;

			GLuint gid=0;

			glGenTextures(1, &gid);					// Generate OpenGL texture IDs

			GLint channels;
			GLenum type;

			if (image.bitsPP==24){
 				type=GL_RGB;	//6407
 				channels=3;
   			}else{
  	   			type=GL_RGBA;	//6408
  	   			channels=4;
   			}


			glBindTexture(GL_TEXTURE_2D,gid);				// Bind Our Texture

			#ifdef MACOSX
        		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
           		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        		logs().renderer.write(stringFunctions.toString(input->width)+" - "+stringFunctions.toString(input->height));
        		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type , GL_UNSIGNED_BYTE, input->data);
			#else
        		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtered
        		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				#ifdef NO_MIP
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
        			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					float nw=targetPowerOf2(image.width);
					float nh=targetPowerOf2(image.height);

					
					pad(&image);

					image.offsetTL.y=-1+(image.height/nh);
					image.offsetTR.y=-1+(image.height/nh);

					glTexImage2D(GL_TEXTURE_2D, 0, type, nw, nh, 0, type , GL_UNSIGNED_BYTE, image.data);

				#else
        			gluBuild2DMipmaps(GL_TEXTURE_2D, channels, image.width, image.height, type , GL_UNSIGNED_BYTE, image.data);
				#endif

			#endif

			hardware.textureMemoryUsed+=image.width*image.height*channels;

			delete[] image.data;

			leaks.rem("ImageData::data");

			texture.infostore[gid]=image;

			return gid;
		}else{

			texture.infostore[-1]=image;
			return -1;
		}

	return -1;
}

/* ImageData TextureLibrary::loadHeader(Path texturefile)
 * Load the header data (image size + name) from the texture library (LMT)
 * file, but don't actually load the image data - it will be flagged as 
 * not yet loaded and loaded later - to decrese initial load time.
 */
ImageData TextureLibrary::loadHeader(Path texturefile){
	Path libraryfile;
	String file;
	unsigned int pos;
	int kpos=0;
	int lid=0;

	ImageData image;
	image.loadSuccess=false;

	for(int i=0; i<libraries.size(); i++){
		for(int k=0; k<libraries[i].files.size(); k++){
			if(libraries[i].files[k]==texturefile.getRelative()){
				libraryfile=libraries[i].file;
				file=libraries[i].files[k];
				pos=libraries[i].pos[k];
				kpos=k;
				lid=i;
			}

		}

	}

	if(!libraryfile.set()){
		return image;
	}

	if(kpos==0){
		pos=4;
		pos+=sizeof(unsigned int);
		pos+=256;	//sig
		for(int i=0; i<libraries[lid].files.size(); i++){
			pos+=sizeof(unsigned int);
			pos+=libraries[lid].files[i].filesize();
		}

	}else{
		pos=4;
		pos+=sizeof(unsigned int);
		pos+=256;	//sig
		for(int i=0; i<libraries[lid].files.size(); i++){
			pos+=sizeof(unsigned int);
			pos+=libraries[lid].files[i].filesize();
		}
		pos+=libraries[lid].pos[kpos-1];
	}

	FILE* f=NULL;

	if(file.size()<5){
		return image;
	}

	if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".bmp"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		
		fseek(f,pos,SEEK_SET);
		image=bmp.loadHeader(f,file);
	}else if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".tga"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		fseek(f,pos,SEEK_SET);
		image=tga.loadHeader(f,file);
	}else if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".jpg"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		fseek(f,pos,SEEK_SET);
		image=jpg.loadHeader(f,file);
	}else{
		image.error="unknown file type '"+file.substr(file.size()-4,file.size()-1)+"'";

		texture.infostore[-1]=image;
		return image;
	}

	return image;
}


int TextureLibrary::loadNoMip(Path texturefile){
	Path libraryfile;
	String file;
	unsigned int pos;
	int kpos=0;
	int lid=0;

	for(int i=0; i<libraries.size(); i++){
		for(int k=0; k<libraries[i].files.size(); k++){
			if(libraries[i].files[k]==texturefile.getRelative()){
				libraryfile=libraries[i].file;
				file=libraries[i].files[k];
				pos=libraries[i].pos[k];
				kpos=k;
				lid=i;
			}

		}

	}


	if(!libraryfile.set()){
		return -1;
	}

	if(kpos==0){
		pos=4;
		pos+=sizeof(unsigned int);
		pos+=256;	//sig
		for(int i=0; i<libraries[lid].files.size(); i++){
			pos+=sizeof(unsigned int);
			pos+=libraries[lid].files[i].filesize();
		}

	}else{
		pos=4;
		pos+=sizeof(unsigned int);
		pos+=256;	//sig
		for(int i=0; i<libraries[lid].files.size(); i++){
			pos+=sizeof(unsigned int);
			pos+=libraries[lid].files[i].filesize();
		}
		pos+=libraries[lid].pos[kpos-1];
	}

	FILE* f;

	if(file.size()<5){
		return -1;
	}

	

	ImageData image;

	if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".bmp"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		fseek(f,pos,SEEK_SET);

		unsigned int len=libraries[lid].pos[kpos]-libraries[lid].pos[kpos-1];
		image=bmp.load(f,file,len);
	}else if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".tga"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		fseek(f,pos,SEEK_SET);
		image=tga.load(f,file);
	}else if(stringFunctions.toLower(file.substr(file.size()-4,file.size()-1))==".jpg"){
		f = fopen(libraryfile.getAbsolute().c_str(), "rb");
		fseek(f,pos,SEEK_SET);
		image=jpg.load(f,file);
	}else{
		image.error="unknown file type '"+file.substr(file.size()-4,file.size()-1)+"'";
		texture.infostore[-1]=image;
		return -1;
	}

	if(image.loadSuccess){
			image.offsetBL.x=0;
			image.offsetBL.y=0;
			image.offsetBR.x=0;
			image.offsetBR.y=0;
			image.offsetTL.x=0;
			image.offsetTL.y=0;
			image.offsetTR.x=0;
			image.offsetTR.y=0;

			GLuint gid=0;

			glGenTextures(1, &gid);					// Generate OpenGL texture IDs

			GLint channels;
			GLenum type;

			if (image.bitsPP==24){
 				type=GL_RGB;	//6407
 				channels=3;
   			}else{
  	   			type=GL_RGBA;	//6408
  	   			channels=4;
   			}


			glBindTexture(GL_TEXTURE_2D,gid);				// Bind Our Texture

			#ifdef MACOSX
        		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
           		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        		logs().renderer.write(stringFunctions.toString(input->width)+" - "+stringFunctions.toString(input->height));
        		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type , GL_UNSIGNED_BYTE, input->data);
			#else
        		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtered
        		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
    			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				float nw=targetPowerOf2(image.width);
				float nh=targetPowerOf2(image.height);

				
				pad(&image);

				image.offsetTL.y=-1+(image.height/nh);
				image.offsetTR.y=-1+(image.height/nh);

				image.offsetTR.x=-1+(image.width/nw);
				image.offsetBR.x=-1+(image.width/nw);

				glTexImage2D(GL_TEXTURE_2D, 0, type, nw, nh, 0, type , GL_UNSIGNED_BYTE, image.data);
			#endif

			hardware.textureMemoryUsed+=image.width*image.height*channels;

			delete[] image.data;
			leaks.rem("ImageData::data");

			texture.infostore[gid]=image;

			return gid;
		}else{
			texture.infostore[-1]=image;
			return -1;
		}
}

bool TextureLibrary::scriptInit(){

	LuaType* luaa=script().add("texture");
	
	script().add("loadLibrary",&luaTextureLibraryLoad,luaa);
	script().add("load",&luaTextureLoad,luaa);
	script().add("add",&luaTextureAdd,luaa);
	script().add("delete",&luaTextureDelete,luaa);

	luaImage=script().add("image");

	return true;
}

bool TextureLibrary::libraryLoaded(Path file){

	for(int i=0; i<libraries.size(); i++){
		if(libraries[i].file==file){
			return true;
		}
	}

	return false;
}

ImageData TextureLibrary::info(int texid){
	return texture.info(texid);
}