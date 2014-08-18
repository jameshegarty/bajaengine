#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <string>
#include <sstream>

#include "Containers.hpp"
#include "Types.hpp"

class String{
	private:
		std::string s;
	public:
	
		String& operator=(const std::string& right) {
			s=right;
			return *this;
		}
		
		String& operator=(const char* right) {
			s=right;
			return *this;
		}
		
		String& operator=(int right) {
			std::ostringstream t;
			t << right;
			s=t.str();
			
			return *this;
		}

		String& operator=(char right) {
			s=right;
			return *this;
		}
		
		operator std::string() const{ 
			//turn this into a std::string
			
       		return s;
		}
		

		String(std::string in){
			s=in;
		}
		
		
		String(const wchar_t* sr){
			char* t=new char[wcslen(sr)+1];

			const wchar_t *tempsour = sr;
			char *tempdest = t;
			while( (*tempdest++=*tempsour++) );
			
			s=t;
		}



		String(const char* sr){
			s=sr;
		}

		String(const unsigned char* sr){
			s=(const char*)sr;
		}
		
		String(const int& sr){
			std::ostringstream t;
			t << sr;
			s=t.str();
		}
		
		String(const float& sr){
			std::ostringstream t;
			t << sr;
			s=t.str();
		}

		String(Matrix4d4d& sr){
			s="["+String(sr.row(0))+"]\n";
			s+="["+String(sr.row(1))+"]\n";
			s+="["+String(sr.row(2))+"]\n";
			s+="["+String(sr.row(3))+"]";
		}

		String(const FloatVector4d& sr){
			std::ostringstream x;
			x << sr.x;
			std::ostringstream y;
			y << sr.y;
			std::ostringstream z;
			z << sr.z;
			std::ostringstream w;
			w << sr.w;

			s="[x]"+x.str()+" [y]"+y.str()+" [z]"+z.str()+" [w]"+w.str();
		}

		String(const FloatVector3d& sr){
			std::ostringstream x;
			x << sr.x;
			std::ostringstream y;
			y << sr.y;
			std::ostringstream z;
			z << sr.z;
	
			s="[x]"+x.str()+" [y]"+y.str()+" [z]"+z.str();
		}

		String(const FloatVector2d& sr){
			std::ostringstream x;
			x << sr.x;
			std::ostringstream y;
			y << sr.y;
		
	
			s="[x]"+x.str()+" [y]"+y.str();
		}

		String(const DoubleVector3d& sr){
			std::ostringstream x;
			x << sr.x;
			std::ostringstream y;
			y << sr.y;
			std::ostringstream z;
			z << sr.z;
	
			s="[x]"+x.str()+" [y]"+y.str()+" [z]"+z.str();
		}

		String(const unsigned int& sr){
			std::ostringstream t;
			t << sr;
			s=t.str();
		}

		String(const double& sr){
			std::ostringstream t;
			t << sr;
			s=t.str();
		}

		#ifdef _WIN32
		String(const DWORD& sr){
			std::ostringstream t;
			t << sr;
			s=t.str();
		}
		#else
		String(const unsigned long& sr){
			std::ostringstream t;
			t << sr;
			s=t.str();
		}
		#endif
		
		String(){
			s="";
		}
		
		
		const char* c_str(){
			return s.c_str();
		}
		
		const char* cStr(){
			return s.c_str();
		}

		const wchar_t* wcStr(){
			wchar_t *r;
			r = new wchar_t[strlen(s.c_str())+1];

			const char *tempsour = s.c_str();
			wchar_t *tempdest = r;
			while( (*tempdest++=*tempsour++) );

			return r;
		}

		const unsigned char* cuStr(){

			unsigned char *r;
			r = new unsigned char[strlen(s.c_str())+1];

			const char *tempsour = s.c_str();
			unsigned char *tempdest = r;
			while( (*tempdest++=*tempsour++) );

			return r;
		}
		
		const unsigned char* pascalStr(){

			unsigned char *r;
			r = new unsigned char[strlen(s.c_str())+2];

			const char *tempsour = s.c_str();
			r[0]=s.size();
			unsigned char *tempdest = r+1;
			while( (*tempdest++=*tempsour++) );

			return r;
		}

		char& operator[](int i) {
			return s[i];
		}

		int size(){
			return (int)s.size();
		}

		int filesize();
		
		bool write(std::ofstream* out);
		bool read(std::ifstream* out);
		
		String substr(int start, int length);

		String substrIndex(int start, int end){
			//returns a subset of this string
			
			String t;
			t.s=s.substr(start,end-start);
			return t;
		}
		
		void clear(){
			s.clear();
		}

		
		Array<String> explode(String);

		//manip functions
		void replace(const char* find, const char* replace);
		bool contains(const char* find);
		
		String toLower();

		//operators
		friend int operator!=(const std::string& left, const String& right);
		friend int operator!=(const String& left, const String& right);
		friend int operator==(const std::string& left, const String& right);
		friend int operator==(const String& left, const char* right);
		friend int operator<(const String& left, const String& right);
  		friend String operator+(const String& left, const int& right);
		friend String operator+(const String& left, const float& right);
		friend String operator+(const String& left, const double& right);

		#ifdef _WIN32
			friend String operator+(const String& left, const DWORD& right);
		#endif

		friend String operator+(const String& left, const String& right);
		friend String operator+(const String& left, const FloatVector3d& right);
		friend std::ostream& operator<<(std::ostream& left, const String& right);
		friend void operator+=(String& left, const String& right);
		friend void operator+=(String& left, const char& right);
};


#endif

