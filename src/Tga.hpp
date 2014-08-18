#ifndef TGA_H
#define TGA_H

#include "FileIo.hpp"

typedef struct TGAHeader{
		unsigned char Header[12];			// File Header To Determine File Type
} TGAHeader;


class Tga : public FileLoader{
	private:
		TGAHeader tgaheader;				// Used To Store Our File Header
		unsigned char uTGAcompare[12];// Uncompressed TGA Header
	  	unsigned char cTGAcompare[12];// Compressed TGA Header
	
	public:
		ImageData loadHeader(FILE* file,Path filename);
		ImageData load(FILE* file,Path filename);

		ImageData loadHeader(Path filename);
		ImageData load(Path filename);
		bool save(Path filename,ImageData texture);
};

extern Tga tga;
	
	
#endif
