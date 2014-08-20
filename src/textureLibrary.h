/* TextureLibrary.h
 * Used for loading and saving lmt texture library files
 */

#ifndef TEXTURE_LIBRARY_H
#define TEXTURE_LIBRARY_H

#include "Containers.hpp"
#include "HelperLibString.hpp"
#include "FileIo.hpp"
#include "os.h"

class LMT{
	public:
		Array<String> files;
		Array<unsigned int> pos;
		Path file;
};

class TextureLibrary{
	private:
		Array<LMT> libraries;

	public:
		Array<String> getUnusedTextures();	//returns an array of textures we haven't used yet - for development use
		bool scriptInit();
		bool loadLibrary(Path file);
		bool libraryLoaded(Path file);	//return true if the library has already been loaded
		int load(Path file);				//load a texture from the libraries, return the gl index of it. -1 if failed
		int loadNoMip(Path file);
		ImageData info(int texid);			//probably better to use the one in class Texture
		ImageData loadHeader(Path file);
};

extern TextureLibrary textureLibrary;

#endif

