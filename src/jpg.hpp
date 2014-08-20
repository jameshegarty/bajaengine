#ifndef JPG_H
#define JPG_H

#include "FileIo.hpp"

class Jpg : public FileLoader{
	private:
	
	public:
		ImageData loadHeader(FILE* file,Path filename);
		ImageData load(FILE* file,Path filename);
		
		ImageData loadHeader(Path filename);
		ImageData load(Path filename);
		
		bool save(Path, ImageData);
};

extern Jpg jpg;

#endif
