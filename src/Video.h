#ifndef VIDEO_H
#define VIDEO_H

#ifdef _WIN32
	#include <windows.h>
	#include <vfw.h>	//need to put the right one (vfw_borland.h) in the devcpp/include dir
	#include <gl/gl.h>
	#include <gl/glu.h>
#elif LINUX
	#include <gl.h>
#else
	#include <GLUT/glut.h>
#endif


//#include <gl/glext.h>	
//#include "../engine/extensions.h"
#include "HelperLibString.hpp"
#include "VideoCapture.h"

//#include "player.h"

/*video class: handles video sources, avi or capture

this DOESN'T deal with the display of the image, it just gets the data into the
"data" pointer

need to use something like opengl to display it

*/

#define MAX_VFW_DEVICES		10
#define ERROR_SIZE			100

#ifdef _WIN32
extern HDRAWDIB hdd;													// Handle For Our Dib
extern HDC hdc;
extern unsigned int m_ValidDriverIndex[MAX_VFW_DEVICES];
extern USHORT m_TotalVideoDrivers;
   		
extern HWND m_hWndVideo;
#endif

class Video{
    public:
		bool listCaptureSources();

    	//void avaliableCaptureSources();
    	//short captureSource(int i);	//1 for default, 2 for next, etc
};
    
extern Video video;

//extern unsigned char* data;
    
enum Vtype{
	CAP,
	AVI,
	VIDEO_NONE
};
    
    


class VideoSource{
	//a SINGLE video source
	
	private:
		#ifdef _WIN32
    	Vtype type; 	
    	//bool getFrameCap(PBITMAPINFO *Bitmap, ULONG BitmapLength, ULONG *RetBitmapLength);
    	CVFWCapture cap;
		
		bool initResize();
		//DWORD lastTick;
		DWORD lastFrameMs;
		float					mpf;										// Will Hold Rough Milliseconds Per Frame
		
		HDRAWDIB hdd;
    	HDC hdc;
    	HBITMAP hBitmap;
    	BITMAPINFOHEADER	bmih;										// Header Information For DrawDibDraw Decoding
		BITMAPINFO * pbmi;		// BITMAPINFO structure
		#endif

		char* pdata;	

		void error(String error);
		void note(String note);
		
 	public:
 	    String file;
 	    
		//if fps is NOT set before sourceavi is called, then fps becomes the default fps gotten from the avi
		//so, should be something like 30 when you accsses it
 	    float fps;	
 	    
 	    bool move_lock;
 	    unsigned int texid;
 	    //float speed;											// Used For Animation
 	    bool autoPlay;	//should the video play at fps?  if set to false, it will get return the frame in var "frame"
 	    
     	long				totalFrames;									// number of frames 1=1 frame, 2=2 frames
     	
    	int					width;										// Video Width
    	int					height;										// Video Height
    	int widthp2;	//width/height rounded up to a power of two
    	int heightp2;
    	
    										// Pointer To Texture Data
    	unsigned char* data;
     	//unsigned char* udata;	//data in unsigned byte format
    	
    	int			frame;											// Frame Counter
    	float		floatFrame;
    	float			next;	
     	#ifdef _WIN32
     	AVISTREAMINFO		psi;										// Pointer To A Structure Containing Stream Info
    	PAVISTREAM			pavi;										// Handle To An Open Stream
    	PGETFRAME			pgf;										// Pointer To A GetFrame Object
    	#endif
		
    	int resizeX;	//-1 to not resize, a positive int to resize
    	int resizeY;	//-1 to not resize, a positive int to resize
    	
		bool powerOf2;	//resize to a power of 2 size? ie 128/256/512/1024

		#ifdef _WIN32
  		bool sourceAvi(LPCSTR szFile);
  		#else
		bool sourceAvi(String szFile);
		#endif
		bool sourceCapture(short driver);
  		bool getFrame(int frame);
  		bool getFrame();								//get next frame
  		bool preload();
  		VideoSource();
 		~VideoSource();        
};

#endif
