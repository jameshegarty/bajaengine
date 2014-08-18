#ifndef FASTSTREAM_H
#define FASTSTREAM_H

/* LOLOLOL

this sucks, it actually makes stuff a lot slower.  don't use it, idiot.

*/

#include <iostream>
#include <fstream>


class FastStream{
	private:
		std::ifstream* in;
		int filesize;
		char* store;
		bool cached;
		int filepos;

	public:
		void seekg( std::streamoff off, std::ios_base::seekdir dir );
		void seekg( std::streampos pos );
		void read(char* s, std::streamsize n ); 
		//void read(char** s, std::streamsize n ); 
		std::streampos  tellg ( );
		void cache();

		int operator==(const char* right);
		//istream&  seekg ( streampos pos );
		//istream&  seekg ( streamoff off, ios_base::seekdir dir );
		//istream&  read (char* s, streamsize n ); 
		FastStream(const char * filename,std::ios_base::openmode mode);
		~FastStream();
};



#endif
