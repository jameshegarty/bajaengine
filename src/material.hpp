#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "HelperLibString.hpp"

class MaterialControl{
	public:
		bool deleteMaterial(String name);
		bool add(String name);
		bool scriptInit();
};

extern MaterialControl material;


#endif