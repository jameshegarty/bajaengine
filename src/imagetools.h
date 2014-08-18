#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

#include "HelperLibString.hpp"
#include "os.h"

class ImageTools{
	public:
		bool thumbnail(Path in,String out,int size);
		bool bmpAlphaToTga(FloatVector3d color, String bmpIn,String tgaOut);
		bool bmpColorAlphaToTga(String bmpColorIn ,String bmpAlphaIn, String tgaOut);
};

ImageTools& imageTools();

bool imageToolsScriptInit();

#endif