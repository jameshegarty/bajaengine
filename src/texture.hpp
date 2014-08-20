/* Texture.hpp
 * This class just contains some helper functions for loading raw image files into
 * OpenGL texture memory.
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "objects.hpp"
#include "Containers.hpp"
#include "FileIo.hpp"
#include "textureLibrary.h"
#include "os.h"


bool powerOf2(int i);
int targetPowerOf2(int i);
void pad(ImageData* i);

class Texture{
	friend class TextureLibrary;

	private:
		Map<int,ImageData> infostore;

	public:
		int load(Path file);			//add a texture, return the gl index of it. -1 if failed
		int loadNoMip(Path file);
		int loadAlpha(Path file);		//add a texture - treat as alpha, return the gl index of it. -1 if failed
		bool load(TextureImage* input);	//upload to opengl
		ImageData info(int texid);
		ImageData loadHeader(Path file);
};

unsigned int EmptyTexture(int x, int y);
unsigned int EmptyTexture(String name,unsigned int x, unsigned int y, unsigned int bytesPP);

void updateTextureLevel();

extern Texture texture;

#endif
