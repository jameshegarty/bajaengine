#include "Tga.hpp"
#include "HelperLibMath.hpp"
#include "Log.hpp"


using namespace std;

Tga tga;

ImageData Tga::load(Path filename){				// Loads A TGA File Into Memory
	FILE *file = fopen(filename.getAbsolute().c_str(), "rb");					// Open The TGA File

	ImageData texture;

	if(	file==NULL){

		texture.loadSuccess=false;
		texture.error="File doesn't exist";
		return texture;	
	}

	return load(file,filename);

}

ImageData Tga::load(FILE* file, Path filename){				// Loads A TGA File Into Memory
	ImageData texture; // temp texture storage
	unsigned char		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	unsigned char		TGAheaderC[12]={0,0,10,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	unsigned char		TGAcompare[12];						// Used To Compare TGA Header
	unsigned char		header[6];						// First 6 Useful Bytes From The Header
	unsigned int		bytesPerPixel;						// Holds Number Of Bytes Per Pixel Used In The TGA File
	unsigned int		imageSize;						// Used To Store The Image Size When Setting Aside Ram
	unsigned int		temp;							// Temporary Variable
	bool compressed=false;
	
	if(	fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare)){
		texture.loadSuccess=false;
		texture.error="TGAcompare";
		logs().renderer.write("TGAcompare error");
		return texture;	
	}

	//compare headers
	if(memcmp(TGAheader,TGAcompare,sizeof(TGAheader))==0){
		compressed=false;
	}else if(memcmp(TGAheaderC,TGAcompare,sizeof(TGAheaderC))==0){
		compressed=true;
	}else{
		logs().renderer.write("bad header, compression");
		texture.loadSuccess=false;
		texture.error="bad header";
		return texture;	
	}

	if(fread(header,1,sizeof(header),file)!=sizeof(header)){
		texture.loadSuccess=false;
		texture.error="header";
		return texture;	
	}
	
	texture.width  = header[1] * 256 + header[0];				// Determine The TGA Width	(highbyte*256+lowbyte)
	texture.height = header[3] * 256 + header[2];				// Determine The TGA Height	(highbyte*256+lowbyte)
	
 	if(	texture.width	<=0 ||						// Is The Width Less Than Or Equal To Zero
		texture.height	<=0 ||						// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))				// Is The TGA 24 or 32 Bit?
	{
		fclose(file);							// If Anything Failed, Close The File
		
		texture.loadSuccess=false;
		return texture;	
	}

	texture.bitsPP	= header[4];						// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture.bitsPP/8;					// Divide By 8 To Get The Bytes Per Pixel
	texture.bytesPP = bytesPerPixel;
	imageSize		= texture.width*texture.height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture.data=new unsigned char[imageSize];

	if(texture.data==NULL){
		fclose(file);							// Close The File
		
		texture.loadSuccess=false;
		texture.error="unable to allocate memory";
		return texture;	
	}

	if(!compressed){
		if(fread(texture.data, 1, imageSize, file)!=imageSize){

			delete[] texture.data;
			fclose(file);							// Close The File
			
			texture.loadSuccess=false;
			texture.error="error reading file";
			return texture;	
		}

		
		for(unsigned int i=0; i<int(imageSize); i+=bytesPerPixel){			// Loop Through The Image Data
											// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
			temp=texture.data[i];					// Temporarily Store The Value At Image Data 'i'
			texture.data[i] = texture.data[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
			texture.data[i + 2] = temp;				// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
		}

	}else{
		//read compressed
			
		unsigned int pixelcount	= texture.height * texture.width;							// Nuber of pixels in the image
		unsigned int currentpixel	= 0;												// Current pixel being read
		unsigned int currentbyte	= 0;												// Current byte 
		unsigned char * colorbuffer = (unsigned char *)malloc(texture.bytesPP);			// Storage for 1 pixel

		do
		{
			unsigned char chunkheader = 0;											// Storage for "chunk" header

			if(fread(&chunkheader, sizeof(unsigned char), 1, file) == 0)				// Read in the 1 byte header
			{
				if(file != NULL)												// If file is open
				{
					fclose(file);												// Close file
				}
				if(texture.data != NULL)									// If there is stored image data
				{
					free(texture.data);									// Delete image data
				}
				texture.loadSuccess=false;
				texture.error="TGA error:Could not read RLE header";
				return texture;													// Return failed
			}

			if(chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
			{																	// that follow the header
				chunkheader++;													// add 1 to get number of following color values
				for(short counter = 0; counter < chunkheader; counter++)		// Read RAW color values
				{
					if(fread(colorbuffer, 1, texture.bytesPP, file) != texture.bytesPP) // Try to read 1 pixel
					{

						if(file != NULL)													// See if file is open
						{
							fclose(file);													// If so, close file
						}

						if(colorbuffer != NULL)												// See if colorbuffer has data in it
						{
							free(colorbuffer);												// If so, delete it
						}

						if(texture.data != NULL)										// See if there is stored Image data
						{
							free(texture.data);										// If so, delete it too
						}

						texture.loadSuccess=false;
						texture.error="Could not read image data";
						return texture;														// Return failed
					}
					
					//colorbuffer
					texture.data[currentbyte		] = colorbuffer[2];				    // Flip R and B vcolor values around in the process 
					texture.data[currentbyte + 1	] = colorbuffer[1];
					texture.data[currentbyte + 2	] = colorbuffer[0];

					if(texture.bytesPP == 4)												// if its a 32 bpp image
					{
						texture.data[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
					}
					
					currentbyte += texture.bytesPP;										// Increase thecurrent byte by the number of bytes per pixel
					currentpixel++;															// Increase current pixel by 1

					if(currentpixel > pixelcount)											// Make sure we havent read too many pixels
					{
						if(file != NULL)													// If there is a file open
						{
							fclose(file);													// Close file
						}	

						if(colorbuffer != NULL)												// If there is data in colorbuffer
						{
							free(colorbuffer);												// Delete it
						}

						if(texture.data != NULL)										// If there is Image data
						{
							free(texture.data);										// delete it
						}

						texture.error="Too many pixels read";
						texture.loadSuccess=false;
						return texture;														// Return failed
					}
				}
			}
			else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
			{
				chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
				if(fread(colorbuffer, 1, texture.bytesPP, file) != texture.bytesPP)		// Attempt to read following color values
				{	
	
					if(file != NULL)														// If thereis a file open
					{
						fclose(file);														// Close it
					}

					if(colorbuffer != NULL)													// If there is data in the colorbuffer
					{
						free(colorbuffer);													// delete it
					}

					if(texture.data != NULL)											// If thereis image data
					{
						free(texture.data);											// delete it
					}

					texture.loadSuccess=false;
					texture.error="Could not read from file";
					return texture;															// return failed
				}

				for(short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
				{																			// by the header
					texture.data[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
					texture.data[currentbyte + 1	] = colorbuffer[1];
					texture.data[currentbyte + 2	] = colorbuffer[0];

					if(texture.bytesPP == 4)												// If TGA images is 32 bpp
					{
						texture.data[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
					}
					
					currentbyte += texture.bytesPP;										// Increase current byte by the number of bytes per pixel
					currentpixel++;															// Increase pixel count by 1

					if(currentpixel > pixelcount)											// Make sure we havent written too many pixels
					{

						if(file != NULL)													// If there is a file open
						{
							fclose(file);													// Close file
						}	

						if(colorbuffer != NULL)												// If there is data in colorbuffer
						{
							free(colorbuffer);												// Delete it
						}

						if(texture.data != NULL)										// If there is Image data
						{
							free(texture.data);										// delete it
						}

						texture.error="Too many pixels read";
						texture.loadSuccess=false;
						return texture;														// Return failed
					}
				}
			}
		}

		while(currentpixel < pixelcount);

	}




	fclose (file);								// Close The File
	
	texture.size=imageSize;
	texture.loadSuccess=true;
	return texture;
}


bool Tga::save(Path filename,ImageData texture){				// Loads A TGA File Into Memory

	unsigned char		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	unsigned char		header[6];						// First 6 Useful Bytes From The Header
	unsigned int		temp;							// Temporary Variable

	FILE *file = fopen(filename.getAbsolute().c_str(), "wb");					// Open The TGA File

	if(	file==NULL){

		texture.loadSuccess=false;
		texture.error="could not open file to write";
		return false;	
	}

	fwrite(TGAheader,1,sizeof(TGAheader),file);

	header[1]=math.floor(texture.width/256);
	header[0]=(texture.width % 256);
	

	header[3]=math.floor(texture.height/256);
	header[2]=(texture.height % 256);
	
	header[4]=texture.bitsPP;

	header[5]=0;

	fwrite(header,1,sizeof(header),file);

	for(unsigned int i=0; i<int(texture.size); i+=texture.bytesPP){			// Loop Through The Image Data
											// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
			temp=texture.data[i];					// Temporarily Store The Value At Image Data 'i'
			texture.data[i] = texture.data[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
			texture.data[i + 2] = temp;				// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
		}

	fwrite(texture.data, 1, texture.size, file);

	for(unsigned int i=0; i<int(texture.size); i+=texture.bytesPP){			// Loop Through The Image Data
											// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
			temp=texture.data[i];					// Temporarily Store The Value At Image Data 'i'
			texture.data[i] = texture.data[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
			texture.data[i + 2] = temp;				// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
		}


	fclose (file);								// Close The File
	
	texture.loadSuccess=true;
	return true;
}

ImageData Tga::loadHeader(Path filename){				// Loads A TGA File Into Memory
	FILE *file = fopen(filename.getAbsolute().c_str(), "rb");					// Open The TGA File

	ImageData texture;

	if(	file==NULL){

		texture.loadSuccess=false;
		texture.error="File doesn't exist";
		return texture;	
	}

	return loadHeader(file,filename);
}

ImageData Tga::loadHeader(FILE* file, Path filename){				// Loads A TGA File Into Memory


	ImageData texture; // temp texture storage
	unsigned char		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	unsigned char		TGAheaderC[12]={0,0,10,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	unsigned char		TGAcompare[12];						// Used To Compare TGA Header
	unsigned char		header[6];						// First 6 Useful Bytes From The Header
	unsigned int		bytesPerPixel;						// Holds Number Of Bytes Per Pixel Used In The TGA File
	unsigned int		imageSize;						// Used To Store The Image Size When Setting Aside Ram
	bool compressed=false;

	if(	fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare)){

		texture.loadSuccess=false;
		texture.error="TGAcompare";
		return texture;	
	}

	//compare headers
	if(memcmp(TGAheader,TGAcompare,sizeof(TGAheader))==0){
		compressed=false;
	}else if(memcmp(TGAheaderC,TGAcompare,sizeof(TGAheaderC))==0){
		compressed=true;
	}else{

		texture.loadSuccess=false;
		texture.error="bad header";
		return texture;	
	}

	if(fread(header,1,sizeof(header),file)!=sizeof(header)){
		texture.loadSuccess=false;
		texture.error="header";
		return texture;	
	}

	
	
	texture.width  = header[1] * 256 + header[0];				// Determine The TGA Width	(highbyte*256+lowbyte)
	texture.height = header[3] * 256 + header[2];				// Determine The TGA Height	(highbyte*256+lowbyte)

	
 	if(	texture.width	<=0 ||						// Is The Width Less Than Or Equal To Zero
		texture.height	<=0 ||						// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))				// Is The TGA 24 or 32 Bit?
	{

		fclose(file);							// If Anything Failed, Close The File
		
		texture.loadSuccess=false;
		return texture;	
	}

	texture.bitsPP	= header[4];						// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture.bitsPP/8;					// Divide By 8 To Get The Bytes Per Pixel
	texture.bytesPP = bytesPerPixel;
	imageSize		= texture.width*texture.height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture.size=imageSize;
	texture.loadSuccess=true;

	fclose(file);

	return texture;
}
