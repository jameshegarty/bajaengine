#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

#include "Containers.hpp"
#include "HelperLibString.hpp"

class ObjectAddress;

class ObjectEnvelope{
	public:
		ObjectAddress* deformer;

		String deformerName;
		unsigned int deformerType;

		unsigned int size();
		bool write(std::ofstream& out);
		
		Array<float> weights;
		Array<unsigned int> indices;
};

#endif

