#include "common.h"
#include "engine.h"
#include "HelperLibMath.hpp"
#include "Log.hpp"
#include "level.hpp"
#include "Audio.h"
#include "timeing.hpp"
#include "hardware.hpp"

#include <al.h>
#include <alc.h>

#include "script.hpp"

#include "os.h"

#include "GlHeader.hpp"

#ifndef PADOFF_VAR
	#define PADOFF_VAR
#endif

typedef struct                                  /* WAV Fmt-header */
{
  unsigned short Format			PADOFF_VAR;
  unsigned short Channels		PADOFF_VAR;
  unsigned int   SamplesPerSec	PADOFF_VAR;
  unsigned int   BytesPerSec		PADOFF_VAR;
  unsigned short BlockAlign		PADOFF_VAR;
  unsigned short BitsPerSample	PADOFF_VAR;
} WAVFmtHdr_Struct;

typedef struct                                  /* WAV Chunk-header */
{
  unsigned char  Id[4]			PADOFF_VAR;
  unsigned int   Size			PADOFF_VAR;
} WAVChunkHdr_Struct;

typedef struct									/* WAV FmtEx-header */
{
  unsigned short Size				PADOFF_VAR;
  unsigned short SamplesPerBlock	PADOFF_VAR;
} WAVFmtExHdr_Struct;

typedef struct                                  /* WAV File-header */
{
  unsigned char  Id[4]			PADOFF_VAR;
  int  Size				PADOFF_VAR;
  unsigned char  Type[4]			PADOFF_VAR;
} WAVFileHdr_Struct;

typedef struct                                  /* WAV Smpl-header */
{
  ALuint   Manufacturer		PADOFF_VAR;
  ALuint   Product			PADOFF_VAR;
  ALuint   SamplePeriod		PADOFF_VAR;
  ALuint   Note				PADOFF_VAR;
  ALuint   FineTune			PADOFF_VAR;
  ALuint   SMPTEFormat		PADOFF_VAR;
  ALuint   SMPTEOffest		PADOFF_VAR;
  ALuint   Loops			PADOFF_VAR;
  ALuint   SamplerData		PADOFF_VAR;
  struct
  {
    ALuint Identifier		PADOFF_VAR;
    ALuint Type				PADOFF_VAR;
    ALuint Start			PADOFF_VAR;
    ALuint End				PADOFF_VAR;
    ALuint Fraction			PADOFF_VAR;
    ALuint Count			PADOFF_VAR;
  }      Loop[1]			PADOFF_VAR;
} WAVSmplHdr_Struct;

void alutLoadWAVFile(ALbyte *file,ALenum *format,ALvoid **data,ALsizei *size,ALsizei *freq, ALboolean *loop)
{
	WAVChunkHdr_Struct ChunkHdr;
	WAVFmtExHdr_Struct FmtExHdr;
	WAVFileHdr_Struct FileHdr;
	WAVSmplHdr_Struct SmplHdr;
	WAVFmtHdr_Struct FmtHdr;
	FILE *Stream;
	
	*format=AL_FORMAT_MONO16;
	*data=NULL;
	*size=0;
	*freq=22050;
	*loop=AL_FALSE;
	if (file)
	{
		Stream=fopen((char*)file,"rb");
		if (Stream)
		{
			fread(&FileHdr,1,sizeof(WAVFileHdr_Struct),Stream);
			FileHdr.Size=((FileHdr.Size+1)&~1)-4;
			while ((FileHdr.Size!=0)&&(fread(&ChunkHdr,1,sizeof(WAVChunkHdr_Struct),Stream)))
			{
				if (!memcmp(ChunkHdr.Id,"fmt ",4))
				{
					fread(&FmtHdr,1,sizeof(WAVFmtHdr_Struct),Stream);
					if (FmtHdr.Format==0x0001)
					{
						*format=(FmtHdr.Channels==1?
								(FmtHdr.BitsPerSample==8?AL_FORMAT_MONO8:AL_FORMAT_MONO16):
								(FmtHdr.BitsPerSample==8?AL_FORMAT_STEREO8:AL_FORMAT_STEREO16));
						*freq=FmtHdr.SamplesPerSec;
						fseek(Stream,ChunkHdr.Size-sizeof(WAVFmtHdr_Struct),SEEK_CUR);
					} 
					else
					{
						fread(&FmtExHdr,1,sizeof(WAVFmtExHdr_Struct),Stream);
						fseek(Stream,ChunkHdr.Size-sizeof(WAVFmtHdr_Struct)-sizeof(WAVFmtExHdr_Struct),SEEK_CUR);
					}
				}
				else if (!memcmp(ChunkHdr.Id,"data",4))
				{
					if (FmtHdr.Format==0x0001)
					{
						*size=ChunkHdr.Size;
						*data=malloc(ChunkHdr.Size+31);
						if (*data) fread(*data,FmtHdr.BlockAlign,ChunkHdr.Size/FmtHdr.BlockAlign,Stream);
						memset(((char *)*data)+ChunkHdr.Size,0,31);
					}
					else if (FmtHdr.Format==0x0011)
					{
						//IMA ADPCM
					}
					else if (FmtHdr.Format==0x0055)
					{
						//MP3 WAVE
					}
				}
				else if (!memcmp(ChunkHdr.Id,"smpl",4))
				{
					fread(&SmplHdr,1,sizeof(WAVSmplHdr_Struct),Stream);
					*loop = (SmplHdr.Loops ? AL_TRUE : AL_FALSE);
					fseek(Stream,ChunkHdr.Size-sizeof(WAVSmplHdr_Struct),SEEK_CUR);
				}
				else fseek(Stream,ChunkHdr.Size,SEEK_CUR);
				fseek(Stream,ChunkHdr.Size&1,SEEK_CUR);
				FileHdr.Size-=(((ChunkHdr.Size+1)&~1)+8);
			}
			fclose(Stream);
		}
		
	}
}


float wavLen(ALbyte *file)
{

	ALenum format;
	ALvoid *data;
	ALsizei size;
	ALsizei freq;
	ALboolean loop;

	WAVChunkHdr_Struct ChunkHdr;
	WAVFmtExHdr_Struct FmtExHdr;
	WAVFileHdr_Struct FileHdr;
	WAVSmplHdr_Struct SmplHdr;
	WAVFmtHdr_Struct FmtHdr;
	FILE *Stream;
	
	format=AL_FORMAT_MONO16;
	data=NULL;
	size=0;
	freq=22050;
	loop=AL_FALSE;
	if (file)
	{
		Stream=fopen((char*)file,"rb");
		if (Stream)
		{
			fread(&FileHdr,1,sizeof(WAVFileHdr_Struct),Stream);
			FileHdr.Size=((FileHdr.Size+1)&~1)-4;
			while ((FileHdr.Size!=0)&&(fread(&ChunkHdr,1,sizeof(WAVChunkHdr_Struct),Stream)))
			{
				if (!memcmp(ChunkHdr.Id,"fmt ",4))
				{
					fread(&FmtHdr,1,sizeof(WAVFmtHdr_Struct),Stream);
					if (FmtHdr.Format==0x0001)
					{
						format=(FmtHdr.Channels==1?
								(FmtHdr.BitsPerSample==8?AL_FORMAT_MONO8:AL_FORMAT_MONO16):
								(FmtHdr.BitsPerSample==8?AL_FORMAT_STEREO8:AL_FORMAT_STEREO16));
						freq=FmtHdr.SamplesPerSec;
						fseek(Stream,ChunkHdr.Size-sizeof(WAVFmtHdr_Struct),SEEK_CUR);
					} 
					else
					{
						fread(&FmtExHdr,1,sizeof(WAVFmtExHdr_Struct),Stream);
						fseek(Stream,ChunkHdr.Size-sizeof(WAVFmtHdr_Struct)-sizeof(WAVFmtExHdr_Struct),SEEK_CUR);
					}
				}
				else if (!memcmp(ChunkHdr.Id,"data",4))
				{
					if (FmtHdr.Format==0x0001)
					{
						size=ChunkHdr.Size;
						data=malloc(ChunkHdr.Size+31);
						if (data) fread(data,FmtHdr.BlockAlign,ChunkHdr.Size/FmtHdr.BlockAlign,Stream);
						memset(((char *)data)+ChunkHdr.Size,0,31);
					}
					else if (FmtHdr.Format==0x0011)
					{
						//IMA ADPCM
					}
					else if (FmtHdr.Format==0x0055)
					{
						//MP3 WAVE
					}
				}
				else if (!memcmp(ChunkHdr.Id,"smpl",4))
				{
					fread(&SmplHdr,1,sizeof(WAVSmplHdr_Struct),Stream);
					loop = (SmplHdr.Loops ? AL_TRUE : AL_FALSE);
					fseek(Stream,ChunkHdr.Size-sizeof(WAVSmplHdr_Struct),SEEK_CUR);
				}
				else fseek(Stream,ChunkHdr.Size,SEEK_CUR);
				fseek(Stream,ChunkHdr.Size&1,SEEK_CUR);
				FileHdr.Size-=(((ChunkHdr.Size+1)&~1)+8);
			}
			fclose(Stream);
		}
		
	}

	return float(size)/float(freq);
}

