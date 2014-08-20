#ifndef STRING_FUNCTIONS
#define STRING_FUNCTIONS

#include <string>
#include "../Types.hpp"

#ifdef MACOSX
    #include <Carbon/Carbon.h>
#endif

class StringFunctions{
	private:
	
	public:
		#ifdef MACOSX
			std::string toString(UInt32 input);
		#endif

		std::string toStringBool(bool input);
		std::string toString(float input);
		std::string toString(bool input);
		std::string toString(int input);
		std::string toString(unsigned int input);
		std::string toString(FloatVector3d input);
		std::string toString(char input);
		std::string toString(long input);
		std::string toString(unsigned long input);

		int toInt(std::string);

		bool toFixedCharArray(char* dest, std::string source);
		int pixelSize(std::string source);
		
		bool contains(std::string source, std::string find);
		std::string toLower(std::string);
};

extern StringFunctions stringFunctions;

#endif
