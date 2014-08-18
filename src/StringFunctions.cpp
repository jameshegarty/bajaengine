#include "StringFunctions.hpp"

#include <sstream>

StringFunctions stringFunctions;



std::string StringFunctions::toStringBool(bool input){
	std::string temp;
	if(input){
 		temp="true";
   	}else{
    	temp="false";
    }
    
     return temp; 
}

std::string StringFunctions::toString(bool input){
	std::string temp;
	if(input){
 		temp="true";
   	}else{
    	temp="false";
    }
    
     return temp; 
}


std::string StringFunctions::toString(float input){
	char temp[20];
	
	sprintf(temp,"%f",input);
	
	std::string one=temp;
	return one;
}



std::string StringFunctions::toString(int input){
	std::string f;
	
	std::ostringstream s;
	s << input;
	f = s.str();
	
	return f;
}


std::string StringFunctions::toString(long input){

	std::string f;

	std::ostringstream s;
	s << input;
	f = s.str();

	return f;
}

std::string StringFunctions::toString(unsigned long input){

	std::string f;

	std::ostringstream s;
	s << input;
	f = s.str();

	return f;
}

#ifdef MACOSX
std::string StringFunctions::toString(UInt32 input){
	
	std::string f;
	
	std::ostringstream s;
	s << input;
	f = s.str();

	
	return f;
}
#endif

std::string StringFunctions::toString(unsigned int input){
	char temp[20];
	
	sprintf(temp,"%u",input);
	
	std::string one="(uint)";
 	one+=temp;
	return one;
}

std::string StringFunctions::toString(FloatVector3d input){
	char temp[50];
	
	sprintf(temp,"[x]%f[y]%f[z]%f",input.x,input.y,input.z);
	
	std::string one="(FloatVector3d)";
 	one+=temp;
	return one;
}


std::string StringFunctions::toString(char input){
	char temp[50];
	
	sprintf(temp,"%c",input);
	
 	std::string one=temp;
	return one;
}



bool StringFunctions::contains(std::string source,std::string find){
    std::string::size_type pos = source.find (find,0);
    if (pos != std::string::npos){
        return true;
    }else{
        return false;
    }
}

bool StringFunctions::toFixedCharArray(char* dest, std::string source){
	memcpy(dest,source.c_str(),sizeof(*dest));
	return true;
}

int StringFunctions::pixelSize(std::string source){
	return source.size()*9;
}


int StringFunctions::toInt(std::string source){
	return atoi(source.c_str());
}

std::string StringFunctions::toLower(std::string source){
	std::string f;

	for(int i=0; i<source.size(); i++){
		f.push_back(tolower(source[i]));
	}

	return f;
}
