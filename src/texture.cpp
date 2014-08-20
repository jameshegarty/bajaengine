#include "texture.hpp"

#include "Bmp.hpp"
#include "Tga.hpp"
#include "jpg.hpp"

#ifdef _WIN32
#include <windows.h>	// Header File For Windows
#endif

#include "GlHeader.hpp"

#include "Helperlib/Log.hpp"

#include "Gl/Extensions.h"

#include "hardware.hpp"
#include "conf.h"
#include "level.hpp"

#include "leaks.h"

Texture texture;

/* void updateTextureLevel()
 * The client can set the maximum texture level (ie, choose a lower mip map as the base)
 * in order to reduce video memory usage.  This function reloads all images at
 * the lower base level
 */
void updateTextureLevel(){
	
	if(conf->textureLevelChanged()){
		
		logs().renderer.write("SET TEXTURE LEVEL"+String(conf->textureLevel));
		
		glGetError();
		
		for(int l=0; l<loadedLevels.size(); l++){
			for(int i=0; i<loadedLevels[l]->textureCount; i++){
				
				if(hardware.compatibility.slowTextureLevel){
					if(loadedLevels[l]->textures[i].loadedTextureLevel!=conf->textureLevel){
						loadedLevels[l]->textures[i].reload();
					}
					
				}else{
					glBindTexture(GL_TEXTURE_2D,loadedLevels[l]->textures[i].glName());
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, conf->textureLevel);
					
					GLenum en=glGetError();
					
					if(en!=GL_NO_ERROR){
						logs().renderer.write("TEX LEVEL ERROR1");
						
					}
				}
			}
			
			for(int i=0; i<loadedLevels[l]->lightmapCount; i++){
				if(hardware.compatibility.slowTextureLevel){
					if(loadedLevels[l]->lightmaps[i].loadedTextureLevel!=conf->textureLevel){
						loadedLevels[l]->lightmaps[i].reload();
					}
				}else{
					glBindTexture(GL_TEXTURE_2D,loadedLevels[l]->lightmaps[i].glName());
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, conf->textureLevel);
					
					GLenum en=glGetError();
					
					if(en!=GL_NO_ERROR){
						logs().renderer.write("TEX LEVEL ERROR2");
						
					}
				}
			}
		}
		
		
		
	}
	
	conf->resetTextureLevel();
}

bool powerOf2(int i){
	if(i==16){
		return true;
	}else if(i==32){
		return true;
	}else if(i==64){
		return true;
	}else if(i==128){
		return true;
	}else if(i==256){
		return true;
	}else if(i==512){
		return true;
	}else if(i==1024){
		return true;
	}else if(i==2048){
		return true;
	}
	
	return false;
}


int targetPowerOf2(int i){
	
	if(powerOf2(i)){
		return i;
	}
	
	if(i<16){
		return 16;
	}else if(i>16 && i<32){
		return 32;
	}else if(i>32 && i<64){
		return 64;
	}else if(i>64 && i<128){
		return 128;
	}else if(i>128 && i<256){
		return 256;
	}else if(i>256 && i<512){
		return 512;
	}else if(i>512 && i<1024){
		return 1024;
	}else if(i>1024 && i<2048){
		return 2048;
	}else if(i>2048 && i<4096){
		return 4096;
	}
	
	return 0;
}


void pad(ImageData* i){
	
	if(powerOf2(i->width) && powerOf2(i->height)){
		return;
	}
	
	if(i->width>4096  || i->height>4096){
		return;
	}
	
	int tX=targetPowerOf2(i->width);
	int tY=targetPowerOf2(i->height);
	
	unsigned char* nd=new unsigned char[tX*tY*i->bytesPP];
	leaks.alloc("pad::nd");
	
	long rowC=0;
	int y;
	
	for(y=0; y<tY; y++){
		int x=0;
		
		if(y<i->height){
			
			for(x=0; x< (i->width*i->bytesPP); x+=i->bytesPP){
				
				nd[x+rowC]=i->data[x+y*i->bytesPP*i->width];
				nd[x+rowC+1]=i->data[x+y*i->bytesPP*i->width+1];
				nd[x+rowC+2]=i->data[x+y*i->bytesPP*i->width+2];
				
				if(i->bytesPP==4){
					nd[x+rowC+3]=i->data[x+y*i->bytesPP*i->width+3];
				}
			}
		}
		
		for(; x< (tX*i->bytesPP); x++){
			nd[x+rowC]=0;
		}
		
		
		rowC+=tX*i->bytesPP;
	}
	
	delete[] i->data;
	i->data=nd;
	leaks.rem("pad::nd");
}


#define SMOOTH_FILTER

bool rescale(ImageData* image){
	
	if(conf->textureLevel==1){
		return true;
	}
	
	int rescale;
	
	if(conf->textureLevel==2){
		rescale=2;
		
	}else if(conf->textureLevel==3){
		rescale=4;
		
	}else if(conf->textureLevel==4){
		rescale=8;
	}else if(conf->textureLevel==5){
		rescale=16;
	}else if(conf->textureLevel==6){
		rescale=32;
	}else{
		return false;
	}
	
	
	if(!powerOf2(image->width) || !powerOf2(image->height)){
		return false;
	}
	
	int nX,nY;
	nX=image->width/rescale;
	nY=image->height/rescale;
	
	unsigned char* nData=new unsigned char[nX*nY*image->bytesPP];
	
#ifdef SMOOTH_FILTER
	FloatVector3d m;
	FloatVector3d t;
	FloatVector3d r;
	FloatVector3d l;
	FloatVector3d b;
	
	FloatVector3d tr;
	FloatVector3d tl;
	FloatVector3d br;
	FloatVector3d bl;
#endif
	
	if(image->bytesPP==3){
		
#ifdef SMOOTH_FILTER
		int ec=0;
		int oX=0;
		int row=0;
		
		for(int cX=0; cX< (nX*nY*image->bytesPP); cX+=3){
			oX+=3;
			
			m.x=image->data[(oX*rescale)];
			m.y=image->data[(oX*rescale)+1];
			m.z=image->data[(oX*rescale)+2];
			
			if(ec==0){
				l.x=image->data[(oX*rescale)];
				l.y=image->data[(oX*rescale)+1];
				l.z=image->data[(oX*rescale)+2];
			}else{
				l.x=image->data[(oX*rescale)-3];
				l.y=image->data[(oX*rescale)-2];
				l.z=image->data[(oX*rescale)-1];
			}
			
			if(ec>=nX){
				r.x=image->data[(oX*rescale)];
				r.y=image->data[(oX*rescale)+1];
				r.z=image->data[(oX*rescale)+2];
			}else{
				r.x=image->data[(oX*rescale)+3];
				r.y=image->data[(oX*rescale)+4];
				r.z=image->data[(oX*rescale)+5];
			}
			
			if(row==0){
				t.x=image->data[(oX*rescale)];
				t.y=image->data[(oX*rescale)+1];
				t.z=image->data[(oX*rescale)+2];
			}else{
				t.x=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))];
				t.y=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))+1];
				t.z=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))+2];
			}
			
			if(row==nY){
				b.x=image->data[(oX*rescale)];
				b.y=image->data[(oX*rescale)+1];
				b.z=image->data[(oX*rescale)+2];
			}else{
				b.x=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))];
				b.y=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))+1];
				b.z=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))+2];
			}
			
			if(row==0 || ec>=nX){
				tr.x=image->data[(oX*rescale)];
				tr.y=image->data[(oX*rescale)+1];
				tr.z=image->data[(oX*rescale)+2];
			}else{
				tr.x=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))+3];
				tr.y=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))+4];
				tr.z=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))+5];
			}
			
			if(row==0 || ec==0){
				tl.x=image->data[(oX*rescale)];
				tl.y=image->data[(oX*rescale)+1];
				tl.z=image->data[(oX*rescale)+2];
			}else{
				tl.x=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))-3];
				tl.y=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))-2];
				tl.z=image->data[(oX*rescale)-(nX*image->bytesPP*(rescale/2))-1];
			}
			
			if(row==nY || ec>=nX){
				br.x=image->data[(oX*rescale)];
				br.y=image->data[(oX*rescale)+1];
				br.z=image->data[(oX*rescale)+2];
			}else{
				br.x=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))+3];
				br.y=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))+4];
				br.z=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))+5];
			}
			
			if(row==nY || ec==0){
				bl.x=image->data[(oX*rescale)];
				bl.y=image->data[(oX*rescale)+1];
				bl.z=image->data[(oX*rescale)+2];
			}else{
				bl.x=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))+3];
				bl.y=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))+4];
				bl.z=image->data[(oX*rescale)+(nX*image->bytesPP*(rescale/2))+5];
			}
			
			nData[cX]=m.x;
			nData[(cX+1)]=m.y;
			nData[(cX+2)]=m.z;
			
			ec++;
			
			//skip every other row
			if(ec>=nX){
				oX+=nX*image->bytesPP*(rescale-1);
				ec=0;
				row++;
			}
		}
#else
		int ec=0;
		int oX=0;
		
		for(int cX=0; cX< (nX*nY*image->bytesPP); cX+=3){
			oX+=3;
			
			nData[cX]=image->data[(oX*rescale)];
			nData[(cX+1)]=image->data[(oX*rescale)+1];
			nData[(cX+2)]=image->data[(oX*rescale)+2];
			
			ec++;
			
			//skip every other row
			if(ec>=nX){
				oX+=nX*image->bytesPP*(rescale-1);
				ec=0;
			}
		}
#endif
		
	}else if(image->bytesPP==4){
		int ec=0;
		int oX=0;
		
		for(int cX=0; cX< (nX*nY*image->bytesPP); cX+=4){
			oX+=4;
			
			nData[cX]=image->data[(oX*rescale)];
			nData[(cX+1)]=image->data[(oX*rescale)+1];
			nData[(cX+2)]=image->data[(oX*rescale)+2];
			nData[(cX+3)]=image->data[(oX*rescale)+3];
			
			ec++;
			
			//skip every other row
			if(ec>=nX){
				oX+=nX*image->bytesPP*(rescale-1);
				ec=0;
			}
		}
	}else{
		return false;
	}
	
	
	delete[] image->data;
	image->data=nData;
	image->width=nX;
	image->height=nY;
	
	return true;
}

int Texture::load(Path file){
	
	if(!file.set()){
		return -1;
	}
	
	if(file.getFilename().size()<5){
		return -1;
	}
	
	
	ImageData image;
	
	if(file.getExtension()=="bmp"){
		image=bmp.load(file);
	}else if(file.getExtension()=="tga"){
		image=tga.load(file);
	}else if(file.getExtension()==".jpg"){
		image=jpg.load(file);
	}else{
		image.error="unknown file type '"+file.getFilename().substr(file.getFilename().size()-4,file.getFilename().size()-1)+"'";
		
		infostore[-1]=image;
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
		
		
		hardware.textureMemoryUsed+=image.width*image.height*channels;
		
		delete[] image.data;
		leaks.rem("ImageData::data");
		
		infostore[gid]=image;
		
		return gid;
	}else{
		infostore[-1]=image;
		return -1;
	}
	
}

ImageData Texture::loadHeader(Path file){
	ImageData image;
	
	if(file.getExtension()=="bmp"){
		image=bmp.loadHeader(file);
	}else if(file.getExtension()=="tga"){
		image=tga.loadHeader(file);
	}else if(file.getExtension()=="jpg"){
		image=jpg.loadHeader(file);
	}else{
		image.error="unknown file type '"+file.getExtension()+"'";
		image.loadSuccess=false;
	}
	
	return image;
}

int Texture::loadNoMip(Path file){
	
	if(!file.set()){
		return -1;
	}
	
	if(file.getFilename().size()<5){
		return -1;
	}
	
	
	ImageData image;
	
	if(file.getExtension()=="bmp"){
		image=bmp.load(file);
	}else if(file.getExtension()=="tga"){
		
		image=tga.load(file);
	}else if(file.getExtension()=="jpg"){
		image=jpg.load(file);
	}else{
		image.error="unknown file type '"+file.getExtension()+"'";
		
		infostore[-1]=image;
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
		
		hardware.textureMemoryUsed+=image.width*image.height*channels;
		
		delete[] image.data;
		leaks.rem("ImageData::data");
		
		infostore[gid]=image;
		
		return gid;
	}else{
		infostore[-1]=image;
		return -1;
	}
	
}

bool Texture::load(TextureImage* file){
	GLint channels;
	GLenum type;
	GLenum target;
	
	if (file->bytesPP==3){
		type=GL_RGB;
		channels=3;
	}else if(file->bytesPP==4){
		type=GL_RGBA;
		channels=4;
	}else{
		//not able to get name
		logs().main.write(String("bad bytesPP '")+file->bytesPP+"' on texture.");
		return false;
	}
	
	if(!file->glNameSet()){
		GLuint gid=0;
		glGenTextures(1, &gid);
		file->glName(gid);
		
		glBindTexture(GL_TEXTURE_2D,gid);
	}else{
		glBindTexture(GL_TEXTURE_2D,file->glName());
	}
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	target=GL_TEXTURE_2D;
	
	if(file->flags & TEXTURE_CUBE_FRONT){
		
		target=GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
	}else if(file->flags & TEXTURE_CUBE_BACK){
		
		target=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
	}else if(file->flags & TEXTURE_CUBE_RIGHT){
		
		target=GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
	}else if(file->flags & TEXTURE_CUBE_LEFT){
		
		target=GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
	}else if(file->flags & TEXTURE_CUBE_TOP){
		
		target=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
	}else if(file->flags & TEXTURE_CUBE_BOTTOM){
		
		target=GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
	}
	
	
#ifndef NO_MIP
	
	int r;
	
	if(conf->textureLevel!=0 && hardware.compatibility.slowTextureLevel){
		
		int scalefac=1;
		
		for(int i=0; i<conf->textureLevel; i++){
			scalefac*=2;
		}
		
		int nx=file->x/scalefac;
		int ny=file->y/scalefac;
		
		unsigned char* newdata=new unsigned char[nx*ny*channels];
		
		gluScaleImage(type,file->x,file->y,GL_UNSIGNED_BYTE,file->data,nx,ny,GL_UNSIGNED_BYTE,newdata);
		
		file->loadedTextureLevel=conf->textureLevel;
		
		r=gluBuild2DMipmaps(target, channels, nx, ny, type , GL_UNSIGNED_BYTE, newdata);
		
		delete[] newdata;
	}else{
		r=gluBuild2DMipmaps(target, channels, file->x, file->y, type , GL_UNSIGNED_BYTE, file->data);
	}
#else
	glTexImage2D(target, 0, type, file->x,  file->y, 0, type , GL_UNSIGNED_BYTE, file->data);
	int r=0;
#endif
	
	hardware.textureMemoryUsed+=file->x*file->y*file->bytesPP;
	
	//we'll delete later so this isn't necessary
	//delete[] file->data;
	
	if(r==0){
		return true;
	}
	
	return false;
	
	
}


/* unsigned int EmptyTexture(int x, int y)
 * This just creates and empty texture of a given size and
 * uploads it to OpenGL and returns the opengl texture id
 */
unsigned int EmptyTexture(int x, int y){
	GLuint txtnumber;											// Texture ID
	
	// Create Storage Space For Texture Data
	unsigned char* data;
	data=NULL;
	data=new unsigned char[x*y*4];
	leaks.alloc("EmptyTexture::data");
	
	//zero memory
	for(int i=0; i<(x*y*4); i++){
		data[i]=0;
	}
	
	glGenTextures(1, &txtnumber);								// Create 1 Texture
	glBindTexture(GL_TEXTURE_2D, txtnumber);					// Bind The Texture
	glTexImage2D(GL_TEXTURE_2D, 0, 4, x, y, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, data);						// Build Texture Using Information In data
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	delete [] data;												// Release data
	leaks.rem("EmptyTexture::data");
	
	return txtnumber;											// Return The Texture ID
}

unsigned int EmptyTexture(String name,unsigned int x, unsigned int y, unsigned int bytesPP){
	GLuint txtnumber;											// Texture ID
	
	// Create Storage Space For Texture Data
	
	unsigned char* data;
	data=NULL;
	data=new unsigned char[x*y*bytesPP];
	
	//zero memory
	for(int i=0; i<(x*y*bytesPP); i++){
		data[i]=0;
	}
	
	
	glGenTextures(1, &txtnumber);								// Create 1 Texture
	glBindTexture(GL_TEXTURE_2D, txtnumber);					// Bind The Texture
	glTexImage2D(GL_TEXTURE_2D, 0, 4, x, y, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, data);						// Build Texture Using Information In data
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	LuaType* img=script().add(name,luaImage);
	script().add("name",name,img);
	LuaType* size=script().add("size",img);
	script().add("x",x,size);
	script().add("y",y,size);
	script().add("bytesPerPixel",bytesPP,img);
	script().add("getValue",luaGetImageValue,img);
	script().add("setValue",luaSetImageValue,img);
	script().add("save",luaSaveImage,img);
	
	ImageData im;
	im.bytesPP=bytesPP;
	im.width=x;
	im.height=y;
	im.data=data;
	im.name=name;
	im.dataLoaded=new bool;
	*im.dataLoaded=true;
	leaks.alloc("ImageData::dataLoaded");
	leaks.alloc("ImageData::data");
	
	loadedImages.pushBack(im);
	
	return txtnumber;											// Return The Texture ID
}

ImageData Texture::info(int texid){
	return infostore[texid];
}
