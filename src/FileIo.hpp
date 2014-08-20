#ifndef FILE_IO_H
#define FILE_IO_H

//#include "common.h"
#include "HelperLibString.hpp"
#include "StringFunctions.hpp"
#include "script.hpp"
#include "os.h"

#ifdef _WIN32
	#include <string.h>
#endif

class ImageData{  // generic image file format
	public:

	unsigned char	*data;			// Image Data (Up To 32 Bits)  unsigned char == glubyte  (RGB(A) format)
									//should be layed out like this: start at bottom left, move right, 
									//when you get to the end of a line, wrap to the next above
									//it's exactly like the first quadrent in a cartesian plane
									//basically, store them like opengl uses them. (I need to check this stuff to make sure it's all right, I'm just guessing)

									/*USE THIS TO GENERATE A TEST IMAGE:
									
									o.data=new unsigned char[image.width*image.height*image.bytesPP];
									w=0;
									h=0;
									for(int i=0; i<(image.width*image.height); i++){
										image.data[i*image.bytesPP+0]=w;
										image.data[i*image.bytesPP+1]=h;
										image.data[i*image.bytesPP+2]=0;
										w++;
										if(w>=image.width){
											w=0;h++;
										}
									}*/

	unsigned int	bytesPP;		//bytes per pixel, assume 1=grayscale alpha, 3=RGB, 4=RGBA
	unsigned int	bitsPP;			//bits per pixel	
	int	width;								// Image Width
	int	height;								// Image Height

	bool loadSuccess;
	unsigned long size;  //it's big
	
	bool* dataLoaded;	//this is a pointer, so that we can copy this class and have this var stick to all the instances

	FloatVector2d offsetTL;	//when not using mipmaps, this is the offset value for the top left corner.  0 if mipmaps in use.
	FloatVector2d offsetTR;	//when not using mipmaps, this is the offset value for the top left corner.  0 if mipmaps in use.
	FloatVector2d offsetBL;	//when not using mipmaps, this is the offset value for the top left corner.  0 if mipmaps in use.
	FloatVector2d offsetBR;	//when not using mipmaps, this is the offset value for the top left corner.  0 if mipmaps in use.

	String file;
	String fileRelative;
	String name;

	std::string error;

	ImageData();
	~ImageData();
};

class FileLoader{  //generic file loader class
	public:
	
	virtual ImageData load(Path);
	virtual bool save (Path,ImageData);
};

extern Array<ImageData> loadedImages;				//this is a seperate hierarchy from level->textures

String getFileType(String input,char delim='.');
ImageData loadImage(Path file);					//This is for loading images that DO NOT get upload automatically to OpenGl (it's just file io)
ImageData loadImage(Path file, String name);		//This is for loading images that DO NOT get upload automatically to OpenGl (it's just file io)
bool saveImage(Path file,ImageData id);
bool fileExists(Path file);
bool checkSize(int size);						//check for power of 2
extern LuaType* luaImage;

extern int luaGetImageValue(lua_State *L);
extern int luaSetImageValue(lua_State *L);
extern int luaSaveImage(lua_State *L);

#endif

