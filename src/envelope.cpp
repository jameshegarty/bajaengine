#include "envelope.hpp"


unsigned int ObjectEnvelope::size(){
	unsigned int final=0; 

	final+=deformerName.filesize();
	final+=sizeof(unsigned int);	//deformer type
	final+=sizeof(unsigned int);	
	final+=sizeof(float)*weights.size();
	final+=sizeof(unsigned int)*indices.size();

	return final;
}

bool ObjectEnvelope::write(std::ofstream& out){
	deformerName.write(&out);
	out.write((char*)&deformerType,sizeof(deformerType));

	unsigned int cnt=indices.size();
	out.write((char*)&cnt,sizeof(cnt));

	for(int i=0; i<weights.size(); i++){
		out.write((char*)&indices[i],sizeof(unsigned int));
		out.write((char*)&weights[i],sizeof(float));
	}

	return true;
}

