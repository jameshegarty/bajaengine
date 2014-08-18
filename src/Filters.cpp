#include "Filters.h"
#include "HelperLibMath.hpp"

int gWidth, gHeight;

typedef struct _pixel
{
   unsigned char red;
   unsigned char blue;
   unsigned char green;
} pixel;


void 
WritePixel (pixel *image, pixel *pix, int x, int y)
{
   *(image+gWidth*y+x) = *pix;
}


void 
ReadPixel (pixel *image, pixel *pix, int x, int y)
{
   *pix = *(image+gWidth*y+x);
}


unsigned char PackFloatInByte(float in){
   return (unsigned char) ((in+1.0f) / 2.0f * 255.0f);
}

#include <math.h>

ImageData sobel(ImageData image){
	float dX, dY, nX, nY, nZ, oolen;
	FloatVector3d final;
	
	FloatVector3d vN;		
	ImageData finalImage=image;
	
	
	finalImage.data=new unsigned char[finalImage.size];
	
	pixel pix, *srcImage, *dstImage;
	
	srcImage=(pixel*)image.data;
	dstImage=(pixel*)finalImage.data;
	
	gHeight=finalImage.height;
	gWidth=finalImage.width;
	
	//from ati normal map thing
	for(int y = 0; y < gHeight; y++){
		for(int x = 0; x < gWidth; x++){
            // Do Y Sobel filter
            ReadPixel(srcImage, &pix, (x-1+gWidth) % gWidth, (y+1) % gHeight);
            dY  = ((float) pix.red) / 255.0f * -1.0f;
            
            ReadPixel(srcImage, &pix,   x   % gWidth, (y+1) % gHeight);
            dY += ((float) pix.red) / 255.0f * -2.0f;
            
            ReadPixel(srcImage, &pix, (x+1) % gWidth, (y+1) % gHeight);
            dY += ((float) pix.red) / 255.0f * -1.0f;
            
            ReadPixel(srcImage, &pix, (x-1+gWidth) % gWidth, (y-1+gHeight) % gHeight);
            dY += ((float) pix.red) / 255.0f *  1.0f;
            
            ReadPixel(srcImage, &pix,   x   % gWidth, (y-1+gHeight) % gHeight);
            dY += ((float) pix.red) / 255.0f *  2.0f;
            
            ReadPixel(srcImage, &pix, (x+1) % gWidth, (y-1+gHeight) % gHeight);
            dY += ((float) pix.red) / 255.0f *  1.0f;
            
            // Do X Sobel filter
            ReadPixel(srcImage, &pix, (x-1+gWidth) % gWidth, (y-1+gHeight) % gHeight);
            dX  = ((float) pix.red) / 255.0f * -1.0f;
            
            ReadPixel(srcImage, &pix, (x-1+gWidth) % gWidth,   y   % gHeight);
            dX += ((float) pix.red) / 255.0f * -2.0f;
            
            ReadPixel(srcImage, &pix, (x-1+gWidth) % gWidth, (y+1) % gHeight);
            dX += ((float) pix.red) / 255.0f * -1.0f;
            
            ReadPixel(srcImage, &pix, (x+1) % gWidth, (y-1+gHeight) % gHeight);
            dX += ((float) pix.red) / 255.0f *  1.0f;
            
            ReadPixel(srcImage, &pix, (x+1) % gWidth,   y   % gHeight);
            dX += ((float) pix.red) / 255.0f *  2.0f;
            
            ReadPixel(srcImage, &pix, (x+1) % gWidth, (y+1) % gHeight);
            dX += ((float) pix.red) / 255.0f *  1.0f;
            
            
            // Cross Product of components of gradient reduces to
            nX = -dX;
            nY = dY;
            nZ = 1;
            
            // Normalize
            oolen = 1.0f/((float) sqrt(nX*nX + nY*nY + nZ*nZ));
            nX *= oolen;
            nY *= oolen;
            nZ *= oolen;
            
            pix.red   = (unsigned char) PackFloatInByte(nX);
            pix.green = (unsigned char) PackFloatInByte(nZ);
            pix.blue  = (unsigned char) PackFloatInByte(nY);
            
            WritePixel(dstImage, &pix, x, y);
		}
	}
	
	delete[] image.data;

	return finalImage;
}

