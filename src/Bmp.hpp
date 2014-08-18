#ifndef BMP_H
#define BMP_H

#include "FileIo.hpp"
	
class Bmp : public FileLoader{
	private:
	
	public:
		ImageData load(FILE* file,Path filename, unsigned int totalfilesize);
		ImageData loadHeader(FILE* file,Path filename);
		ImageData loadHeader(Path filename);
		ImageData load(Path filename);
		
		bool save(Path, ImageData);
};

extern Bmp bmp;

#endif
