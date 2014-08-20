#include "GlHeader.hpp"

#include <math.h>			// Math Library Header File
#include <stdio.h>			// Header File For Standard Input/Output

#include "StringFunctions.hpp"
#include "Log.hpp"
#include "HelperLibMath.hpp"
#include "Bmp.hpp"

#include "endian.hpp"

#include "leaks.h"

Bmp bmp;

// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.  
// See http://www.dcs.ed.ac.uk/~mxr/gfx/2d/BMP.txt for more info.

ImageData Bmp::load(Path filename){
	FILE *file;
	ImageData image;

#ifdef LINUX
	std::ifstream f;
	f.open(filename.getRelative().c_str(), std::ios_base::binary | std::ios_base::in);
	if (!f.good() || f.eof() || !f.is_open()) { return image; }
	f.seekg(0, std::ios_base::beg);
	std::ifstream::pos_type begin_pos = f.tellg();
	f.seekg(0, std::ios_base::end);
	int ss=static_cast<int>(f.tellg() - begin_pos);
	f.close();

	if ((file = fopen(filename.getAbsolute().c_str(), "rb"))==NULL)
    {
		logs().renderer.write("File Not Found : "+filename.getAbsolute());
		image.loadSuccess=false;
		return image;
    }

	return load(file,filename,ss);
#else
	// make sure the file is there.
    if ((file = fopen(filename.getAbsolute().c_str(), "rb"))==NULL)
    {
		logs().renderer.write("File Not Found : "+filename.getAbsolute());
		image.loadSuccess=false;
		return image;
    }

	fseek(file,0,SEEK_END);
	fpos_t pos;
	fgetpos(file,&pos);
	fseek(file,0,SEEK_SET);

	return load(file,filename,pos);

#endif
}

ImageData Bmp::load(FILE* file,Path filename,unsigned int totalfilesize){

	ImageData image;
  
    
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.

    unsigned short planes;          // number of planes in image (must be 1) 
    unsigned short bpp;             // number of bits per pixel (must be 24)
    unsigned int sizeX;
    unsigned int sizeY;
    unsigned char temp;                          // temporary color storage for bgr-rgb conversion.

	unsigned int currentCurPos=0;

    
    
	fseek(file, 2, SEEK_CUR);
	currentCurPos+=2;

	unsigned int totalsize=0;	//headersize:56+ width*height*bpp

	if ((i = fread(&totalsize, sizeof(totalsize), 1, file)) != 1) {
		logs().renderer.write("Error reading compression from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }

	//read in REAL file size - sometimes BMP lies
	//	fseek(file,0,SEEK_END
	//	fgetpos
	//end READ

	currentCurPos+=sizeof(totalsize);

	totalsize=endian(totalsize);
	
    // seek through the bmp header, up to the width/height:
	fseek(file, 4, SEEK_CUR);
	currentCurPos+=4;
	unsigned int headersize=0;	//headersize:56+ width*height*bpp

	if ((i = fread(&headersize, sizeof(headersize), 1, file)) != 1) {
		logs().renderer.write("Error reading compression from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }
	currentCurPos+=sizeof(headersize);

	headersize=endian(headersize);
	
	fseek(file, 4, SEEK_CUR);
	currentCurPos+=4;

    i = fread(&sizeX, 4, 1, file);
    
	currentCurPos+=4;

	sizeX=endian(sizeX);
    
    // read the width
    if (i != 1) {
		logs().renderer.write("Error reading width from"+filename.getRelative());
		image.loadSuccess=false;
		return image;
    }

    // read the height 
    i = fread(&sizeY, 4, 1, file);
    currentCurPos+=4;

	sizeY=endian(sizeY);
    
    if (i != 1) {
		logs().renderer.write("Error reading height from"+filename.getRelative());
		image.loadSuccess=false;
		return image;
    }

    
    // read the planes
    i=fread(&planes, 2, 1, file);
    currentCurPos+=2;

	planes=endian(planes);
    
    if (i != 1) {
		logs().renderer.write("Error reading planes from "+filename.getRelative());
		image.loadSuccess=false;
		return image;
    }
    
    if (planes != 1) {
		logs().renderer.write("Planes from "+filename.getRelative()+" is not 1, it is "+stringFunctions.toString(planes));
		image.loadSuccess=false;
		return image;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
		logs().renderer.write("Error reading bpp from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }
	currentCurPos+=2;

	unsigned int compression=0;

	if ((i = fread(&compression, sizeof(compression), 1, file)) != 1) {
		logs().renderer.write("Error reading compression from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }
    currentCurPos+=sizeof(compression);

	unsigned int csize=0;

	compression=endian(compression);
	
	if ((i = fread(&csize, sizeof(csize), 1, file)) != 1) {
		logs().renderer.write("Error reading compression from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }
    currentCurPos+=sizeof(csize);

	csize=endian(csize);


	bpp=endian(bpp);
    
    if (bpp == 24) {
		image.bytesPP=3;
		image.bitsPP=24;
	}else if (bpp ==32) {
		image.bytesPP=4; 
		image.bitsPP=32;
	}else{
		//printf("Bpp from %s is not 24: %u\n", filename, bpp);
                logs().renderer.write("Error reading bpp from '"+filename.getRelative()+"' bad bpp");
                
		image.loadSuccess=false;
		return image;
    }
	
	size = sizeX * sizeY * image.bytesPP;

    // seek past the rest of the bitmap header.
	fseek(file, 16, SEEK_CUR);
	currentCurPos+=16;

	unsigned char* hackdata=NULL;
	int offset=0;

	bool hackdataAlloc=false;

	if( (totalfilesize-headersize-size)!=0){
		float x=(totalfilesize-headersize-size);

		float offsetF=x/sizeY;
		offset=offsetF;
		hackdata=new unsigned char[size+offset*sizeY];
		hackdataAlloc=true;

		leaks.alloc("Bmp::Load::hackdata");
	}

    // read the data. 
    image.data=new unsigned char[size];
	leaks.alloc("ImageData::data");

    if (image.data == NULL) {
		logs().renderer.write("Error allocating memory for color-corrected image data");
		image.loadSuccess=false;
		image.error="Error allocating memory for color-corrected image data";

		if(hackdataAlloc){
			delete[] hackdata;
			leaks.rem("Bmp::Load::hackdata");
		}

		return image;	
    }

	if(offset!=0){
		//bitmaps, if not a power of two, termenate each row with a few bytes with value 0
		//why? d'no.  we can derive this from the filesize given in the file

		fseek(file, headersize-currentCurPos, SEEK_CUR);

		if ((i = fread(hackdata, size+offset*sizeY, 1, file)) != 1) {
			logs().renderer.write("Error reading image data from"+filename.getRelative());
			image.loadSuccess=false;

			if(hackdataAlloc){
				delete[] hackdata;
				leaks.rem("Bmp::Load::hackdata");
			}

			return image;
		}

		for(int h=0; h<sizeY; h++){

			memcpy(&image.data[h*sizeX*image.bytesPP],&hackdata[h*sizeX*image.bytesPP+h*offset],sizeX*image.bytesPP);
		}

		
		delete[] hackdata;
		hackdataAlloc=false;
		leaks.rem("Bmp::Load::hackdata");
	}else{
		//fseek(file, headersize, SEEK_SET);
		fseek(file, headersize-currentCurPos, SEEK_CUR);
		if ((i = fread(image.data, size, 1, file)) != 1) {
			logs().renderer.write("Error reading image data from"+filename.getRelative());
			image.loadSuccess=false;

			if(hackdataAlloc){
				delete[] hackdata;
				leaks.rem("Bmp::Load::hackdata");
			}

			return image;
		}
	}

	if(image.bytesPP==3){
		for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
			temp = image.data[i];
			image.data[i] = image.data[i+2];
			image.data[i+2] = temp;
			
			image.data[i]=endian(image.data[i]);
			image.data[i+1]=endian(image.data[i+1]);
			image.data[i+2]=endian(image.data[i+2]);
		}
	}else if(image.bytesPP==4){
		for (i=0;i<size;i+=4) { // reverse all of the colors. (bgr -> rgb)
			temp = image.data[i];
			image.data[i] = image.data[i+2];
			image.data[i+2] = temp;
			
			image.data[i]=endian(image.data[i]);
			image.data[i+1]=endian(image.data[i+1]);
			image.data[i+2]=endian(image.data[i+2]);
		}
	}
    
	

    image.width=int(sizeX);
    image.height=int(sizeY);

    image.loadSuccess=true;
    image.size=int(size);
    
	fclose(file);
    
	if(hackdataAlloc){
		delete[] hackdata;
		leaks.rem("Bmp::Load::hackdata");
	}

    // we're done.
    return image;
}

bool Bmp::save(Path filename, ImageData inputImage){
	ImageData image=inputImage;
	
	logs().renderer << "[Bmp::save] Saving Bmp \"" << filename.getRelative() << "\"" << endLine;
	logs().renderer.write("[Bmp::save] width: "+stringFunctions.toString(image.width));
	logs().renderer.write("[Bmp::save] height "+stringFunctions.toString(image.height));
    FILE *file;
    
    char temp;                          // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
    if ((file = fopen(filename.getAbsolute().c_str(), "wb"))==NULL)
    {
		logs().renderer.write("File Not Found : "+filename.getAbsolute());
		
		return false;
    }
    
	GLubyte		BMPheader[2]={66,77};

	if ((fwrite(&BMPheader, sizeof(BMPheader), 1, file)) != 1) {
		logs().renderer.write("[Bmp::save] Error writeing header to "+filename.getRelative());
		
		return false;
		
    }

	unsigned int totalsize=image.width*image.height*image.bytesPP+54;

	if ((fwrite(&totalsize, sizeof(totalsize), 1, file)) != 1) {
		logs().renderer.write("[Bmp::save] Error writeing header to "+filename.getRelative());
		
		return false;
		
    }
    
	
	totalsize=0;

	if ((fwrite(&totalsize, sizeof(totalsize), 1, file)) != 1) {
		logs().renderer.write("[Bmp::save] Error writeing header to "+filename.getRelative());
		
		return false;
		
    }

	//offset to data=54
	totalsize=54;

	if ((fwrite(&totalsize, sizeof(totalsize), 1, file)) != 1) {
		logs().renderer.write("[Bmp::save] Error writeing header to "+filename.getRelative());
		
		return false;
		
    }

	//	Size of InfoHeader =40 
	totalsize=40;

	if ((fwrite(&totalsize, sizeof(totalsize), 1, file)) != 1) {
		logs().renderer.write("[Bmp::save] Error writeing header to "+filename.getRelative());
		
		return false;
		
    }

    unsigned int width=image.width;
    unsigned int height=image.height;
    
    #ifdef MACOSX
    width=EndianSwap(width);
    height=EndianSwap(height);
    #endif
    
    // write the width
    if ((fwrite(&width, 4, 1, file)) != 1) {
		logs().renderer.write("Error writeing width to "+filename.getRelative());
		
		return false;
		
    }
    
	// write the height 
    if ((fwrite(&height, 4, 1, file)) != 1) {
		logs().renderer.write("Error writeing height to "+filename.getRelative());
		
		return false;
    }
  
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
   unsigned long size = image.height * image.width * image.bytesPP;

    // write the planes
    unsigned short int planes=1;
    
    if ((fwrite(&planes, 2, 1, file)) != 1) {
		logs().renderer.write("Error writeing planes to "+filename.getRelative());
		
		return false;
    }

    // write the bpp
    unsigned short bitsPP;
    
    if(image.bytesPP==1){
		bitsPP=24;
    }else{
    	bitsPP=24;
	}
	
	#ifdef MACOSX
		bitsPP=EndianSwap(bitsPP);
    #endif
    
    if ((fwrite(&bitsPP, 2, 1, file)) != 1) {
		logs().renderer.write("[Bmp save] Error writeing bpp to "+filename.getRelative());
		
		return false;
    }

	//compression
	unsigned int comp=0;
    
    if ((fwrite(&comp, 4, 1, file)) != 1) {
		logs().renderer.write("Error writeing planes to "+filename.getRelative());
		
		return false;
    }
    
	//imagesize
	comp=0;
    
    if ((fwrite(&comp, 4, 1, file)) != 1) {
		logs().renderer.write("Error writeing planes to "+filename.getRelative());
		
		return false;
    }
    
	//XpixelsPerM
	comp=0;
    
    if ((fwrite(&comp, 4, 1, file)) != 1) {
		logs().renderer.write("Error writeing planes to "+filename.getRelative());
		
		return false;
    }

	//YpixelsPerM
	comp=0;
    
    if ((fwrite(&comp, 4, 1, file)) != 1) {
		logs().renderer.write("Error writeing planes to "+filename.getRelative());
		
		return false;
    }

	//ColorsUsed
	comp=0;
    
    if ((fwrite(&comp, 4, 1, file)) != 1) {
		logs().renderer.write("Error writeing planes to "+filename.getRelative());
		
		return false;
    }

	//ColorsImportant
	comp=0;
    
    if ((fwrite(&comp, 4, 1, file)) != 1) {
		logs().renderer.write("Error writeing planes to "+filename.getRelative());
		
		return false;
    }
    
    // read the data. 
    
    if (image.data == NULL) {
		logs().renderer.write("[Bmp save] No image data to write!");
		
   		return false;	
    }

	if(image.bytesPP==3){
		for (unsigned int i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
			temp = image.data[i];
			image.data[i] = image.data[i+2];
			image.data[i+2] = temp;
    	}
    	
    	if ((fwrite(image.data, size, 1, file)) != 1) {
			logs().renderer.write("[Bmp save] Error writeing image data to "+filename.getRelative());
		
			return false;
    	}
    	
    	for (unsigned int i=0;i<size;i+=3) { // new we switch them back to RGB.  shoud figure out some way to not have to do this
			temp = image.data[i];
			image.data[i] = image.data[i+2];
			image.data[i+2] = temp;
		}
		
    }else if(image.bytesPP==1){
    	//we need to expand an alpha into 3 components so taht it can actually be read
    	unsigned char* expanded=new unsigned char[image.width*image.height*3];
		leaks.alloc("Bmp::save::expanded");

    	logs().renderer.write("[Bmp::save] 1 bytesPP");
    	logs().renderer.write(image.width*image.height*3);
    	
    	int m;
    	
    	for(m=0; m<(image.width*image.height); m++){
    		expanded[(m*3)]=image.data[m];
    		expanded[(m*3)+1]=image.data[m];
    		expanded[(m*3)+2]=image.data[m];
    	}
    	
    	logs().renderer << "m=" << m << endLine;
    	
    	if ((fwrite(expanded,image.width*image.height*3, 1, file)) != 1) {
			logs().renderer.write("[Bmp save] Error writeing image data to "+filename.getRelative());
			delete expanded;
			leaks.rem("Bmp::save::expanded");

			return false;
    	}
    	
    	delete expanded;
		leaks.rem("Bmp::save::expanded");
    }
    
    
    fclose(file);
    
    // we're done.
    return true;
}

ImageData Bmp::loadHeader(Path filename){
	FILE *file;
	ImageData image;

	 // make sure the file is there.
    if ((file = fopen(filename.getAbsolute().c_str(), "rb"))==NULL)
    {
		logs().renderer.write("File Not Found : "+filename.getAbsolute());
		image.loadSuccess=false;
		return image;
    }

	return loadHeader(file,filename);
}

ImageData Bmp::loadHeader(FILE* file,Path filename){

	ImageData image;
  
	unsigned long i;                    // standard counter.

    unsigned short planes;          // number of planes in image (must be 1) 
    unsigned short bpp;             // number of bits per pixel (must be 24)
    unsigned int sizeX;
    unsigned int sizeY;
    
	fseek(file, 2, SEEK_CUR);

	unsigned int totalsize=0;	//headersize:56+ width*height*bpp

	if ((i = fread(&totalsize, sizeof(totalsize), 1, file)) != 1) {
		logs().renderer.write("Error reading compression from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }

	fseek(file, 4, SEEK_CUR);

	unsigned int headersize=0;	//headersize:56+ width*height*bpp

	if ((i = fread(&headersize, sizeof(headersize), 1, file)) != 1) {
		logs().renderer.write("Error reading compression from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }

	fseek(file, 4, SEEK_CUR);

    i = fread(&sizeX, 4, 1, file);


    // read the width
    if (i != 1) {
		logs().renderer.write("Error reading width from"+filename.getRelative());
		image.loadSuccess=false;
		return image;
    }
 
 
	sizeX=endian(sizeX);
	
	i = fread(&sizeY, 4, 1, file);
    
    
    if (i != 1) {
		logs().renderer.write("Error reading height from"+filename.getRelative());
		image.loadSuccess=false;
		return image;
    }

	sizeY=endian(sizeY);
	
    i=fread(&planes, 2, 1, file);
    planes=endian(planes);

    
    if (i != 1) {
		logs().renderer.write("Error reading planes from "+filename.getRelative());
		image.loadSuccess=false;
		return image;
    }

    if (planes != 1) {
		logs().renderer.write("Planes from "+filename.getRelative()+" is not 1, it is "+stringFunctions.toString(planes));
		image.loadSuccess=false;
		return image;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
		logs().renderer.write("Error reading bpp from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }


	unsigned int compression=0;

	if ((i = fread(&compression, sizeof(compression), 1, file)) != 1) {
		logs().renderer.write("Error reading compression from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }
    
	unsigned int csize=0;

	if ((i = fread(&csize, sizeof(csize), 1, file)) != 1) {
		logs().renderer.write("Error reading compression from"+filename.getRelative());
		
		image.loadSuccess=false;
		return image;
    }
    
    bpp=endian(bpp);
	
    if (bpp == 24) {
		image.bytesPP=3;
		image.bitsPP=24;
	}else if (bpp ==32) {
		image.bytesPP=4; 
		image.bitsPP=32;
	}else{
                logs().renderer.write("Error reading bpp from '"+filename.getRelative()+"' bad bpp");
                
		image.loadSuccess=false;
		return image;
    }
	

	image.width=int(sizeX);
    image.height=int(sizeY);
    image.loadSuccess=true;
    image.size=int(sizeX)*int(sizeY)*int(image.bytesPP);

    fclose(file);
    
    // we're done.
    return image;

}

