

#include "faststream.h"

using namespace std;

FastStream::FastStream(const char * file,std::ios_base::openmode mode){

	cached=false;

	//ifstream iin (file, ios::in|ios::binary);
  	//in=&iin;

	in=new ifstream;
	in->open(file, ios::in|ios::binary);

	//if(*in==NULL){
	//	int blah;
	//}

	cache();
}



void FastStream::seekg( std::streamoff off, std::ios_base::seekdir dir ){

	//in->seekg(off,dir);
	filepos=off;
}


void FastStream::seekg( std::streampos pos ){
	//in->seekg(pos);

	filepos=pos;
}

void FastStream::read(char* s, std::streamsize n ){

	//in->read(s,n);
	//s=&store[filepos];

	//&s=(char**)store;

	//char** lol=&s;
	//lol=(char**)store;

	memcpy(s,&store[filepos],n);
	filepos+=n;
}

/*void FastStream::read(char** s, std::streamsize n ){

	//in->read(s,n);
	//s=&store[filepos];

	//&s=(char**)store;

	//char** lol=&s;
	//lol=(char**)store;

	s=&store[filepos];
	//memcpy(s,&store[filepos],n);
	filepos+=n;
}*/

std::streampos  FastStream::tellg ( ){
	//return in->tellg();

	return filepos;
}




void FastStream::cache(){
	in->seekg(0, std::ios_base::beg);
	std::ifstream::pos_type begin_pos = in->tellg();
	in->seekg(0, std::ios_base::end);
	filesize=static_cast<int>(in->tellg() - begin_pos);


	in->seekg (0, ios::beg);
	store=new char[filesize];
	in->read(store,filesize);


	cached=true;
}

int FastStream::operator==(const char* right){
	//HACKKKKK 

	return 0;

	if(*in==NULL){
		return 1;
	}

	return 0;

}

FastStream::~FastStream(){


	in->close();
	delete[] store;
}

