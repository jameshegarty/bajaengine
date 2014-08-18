#include "Containers.hpp"

#ifdef SMALL_CONTAINER

void* ArrayBase::baseIndex(unsigned int i){

	#ifdef DEBUG
	if(i>=data.size() || i < 0){

		int lol=0;
	}
	#endif

	return data[i];
}

unsigned int ArrayBase::baseSize() const{
	return data.size();
}

void ArrayBase::basePushBack(void* t){
	data.push_back(t);
}

void ArrayBase::baseClear(){
	for(int i=0; i<data.size(); i++){
		delete data[i];
	}

	data.clear();
}

bool ArrayBase::baseErase(unsigned int index){
	int size=data.size();
	
	if(size>0){
		void** temp=new void*[size];
	
		for(int i=0; i<size; i++){
			temp[i]=data[i];
		}
	
		data.clear();
	
		for(int i=0; i<size; i++){
			if(i!=index){
			
				data.push_back(temp[i]);
			}else{
				delete temp[i];
			}
		}

		delete[] temp;
	}

	return true;
}


void ArrayBase::basePushFront(void * input){
	data.push_back(NULL);
	for(int i=data.size()-1; i>0; i--){
		data[i]=data[i-1];
	}
	data[0]=input;
}

void ArrayBase::baseReverse(){
	std::vector<void*> nd;

	for(unsigned int i=0; i<data.size(); i++){
		nd.push_back(data[data.size()-1-i]);
	}

	data=nd;

}

void ArrayBase::baseReserve(unsigned int i){
	data.reserve(i);
}


int ArrayBase::baseFind(void* t){
	for(unsigned int i=0; i<data.size(); i++){
		if(t==data[i]){
			return i;
		}
	}
	
	return -1;
}

ArrayBase::~ArrayBase(){
	for(int i=0; i<data.size(); i++){
		delete data[i];
	}
}

/*void ArrayBase::basePopFront(){
	delete data[0];
	data.pop_front();
}*/

void ArrayBase::basePopBack(){
	delete data[data.size()-1];
	data.pop_back();
};

#endif