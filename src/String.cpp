#include "HelperLibString.hpp"
#include <sstream>
#include <algorithm>
#include "endian.hpp"

int operator!=(const std::string& left, const String& right){
	if(right.s!=left){return true;}
	return false;
}

int operator!=(const String& left, const String& right){
	if(right.s!=left.s){return true;}
	return false;
}


int operator<(const String& left, const String& right){

	return left.s<right.s;
}

int operator==(const std::string& left, const String& right){
	if(right.s==left){return true;}
	return false;
}

int operator==(const String& left, const char* right){
	if(left.s==right){return true;}
	return false;
}

std::ostream& operator<<(std::ostream& left, const String& right){
	left << right.s;
	return left;
}

String String::substr(int start, int length){
	//returns a subset of this string
			
	String t;
	t=s.substr(start,length);
	return t;
}

String operator+(const String& left, const int& right){

	std::ostringstream s;
	s << right;
	
	String t=left.s+s.str();
	
	return t;
}

String operator+(const String& left, const double& right){

	std::ostringstream s;
	s << right;
	
	String t=left.s+s.str();
	
	
	return t;

}

#ifdef _WIN32
String operator+(const String& left, const DWORD& right){
	
	std::ostringstream s;
	s << right;
	
	String t=left.s+s.str();
	
	return t;
}
#endif

String operator+(const String& left, const float& right){

	std::ostringstream s;
	s << right;
	
	String t=left.s+s.str();
	
	return t;
}

String operator+(const String& left, const FloatVector3d& right){

	String t=left.s;
	
	std::ostringstream s;
	
	s << right.x;
	t+="[x]";
	t+=s.str();
	
	s << right.y;
	t+="[y]";
	t+=s.str();
	
	s << right.z;
	t+="[z]";
	t+=s.str();
	
	return t;
}

String operator+(const String& left, const String& right){
	String t=left.s+right.s;
	return t;
}

void operator+=(String& left, const String& right){
	left.s+=right.s;
}


void operator+=(String& left, const char& right){
	left.s+=right;
}


void String::replace(const char* find, const char* replace){
	std::string f=find;
	std::string r=replace;
	std::string::size_type i=s.find(f);
	
	while(i!=std::string::npos){
		s.replace(i,f.size(),r);
		i=s.find(f);
	}
}

bool String::contains(const char* find){
	std::string::size_type pos = s.find (find,0);
    if (pos != std::string::npos){
        return true;
    }else{
        return false;
    }
}

int String::filesize(){	//size when serilaized, in bytes

	if(s.size()>0){
		return (int)s.size()+(int)sizeof(unsigned int)+1;	
	}else{
		return (int)sizeof(unsigned int);
	}
	
}

bool String::write(std::ofstream* out){
	unsigned long pos=out->tellp();

	unsigned int si=size();

	out->write((char*)&si,sizeof(unsigned int));

	if(si>0){
		si++;	//NULL
		out->write(cStr(),si);
		out->seekp(pos+sizeof(unsigned int)+si);
	}else{
		out->seekp(pos+sizeof(unsigned int));
	}

	return true;
}


bool String::read(std::ifstream* in){

	unsigned long pos=in->tellg();

	unsigned int si=0;

	in->read ((char*)&si,sizeof(unsigned int));
	si=endian(si);
	
	if(si>0){
		si++;	//for null

		char * cs;

		try{
			cs=new char[si];
		}catch(...){
			return false;
		}
		in->read (cs,si);

		s=cs;

		delete cs;

		in->seekg(pos+sizeof(unsigned int)+si);
	}else{
		in->seekg(pos+sizeof(unsigned int));
	}

	return true;
}

String String::toLower(){
	std::transform (s.begin(),s.end(), s.begin(),tolower);

	return s;
}

Array<String> String::explode(String div){

	Array<String>final;
	std::string temp;

	std::string::size_type pos;

	pos=s.find(div);

	std::string news;
	news=s;

	while(pos!=std::string::npos){
		//none found

		final.pushBack(news.substr(0,pos));
		news=news.substr(pos+div.size(),news.size());

		pos=news.find(div);
	}


	final.pushBack(news.substr(0,news.size()));

	return final;
}
