#ifndef LEAKS_H
#define LEAKS_H 1

#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"

class Leaks{
	public:
		Map<String,int> count;

		//if global, i should be Class::var
		//if local, i should be Class::function::var

		void alloc(String i){
			count[i]++;
		}

		void rem(String i){
			count[i]--;
		}

		Leaks(){

		}
};

extern Leaks leaks;

#endif
