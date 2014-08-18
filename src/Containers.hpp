#ifndef CONTAINERS_H
#define CONTAINERS_H

#include <vector>
#include <string>
#include <map>
#include <iostream>


class Bool{
	public:
		bool b;
		Bool(bool l){b=l;}
		Bool(){b=false;}
};

//#define NULL_CONTAINER 1
//#define SMALL_CONTAINER 1

#ifdef NULL_CONTAINER


template<class T>
class Array{
	private:
		T t;

	public:
	
	
	T& operator[](int index){return t;}

	const T& operator[](int index) const{ return t;}

	void reserve(int count){}
    
	void pushBack(){}
	
	void pushBackUnique(const T& input){}
	
	void pushBack(const T& input){}
	
	void popBack(){}

	void popFront(){}
	
	int size() const{return 0;}
	
	void clear(){}
	
	bool eraseValue(const T& input){return false;}

	bool erase(int index){return false;}
	
	
	bool swap(int one, int two){return false;}
	
	unsigned int dataRef(const T& t){return 0;}
	
	int find(const T& t){return 0;}

	Array(){}
	
	bool operator==(Array<T> in){
		return true;
	}

	void pushFront(const T& t){}

	void reverse(){}
};



template<class T>
class SafeArray{
	private:

		T* t;
	public:
	
	T& operator[](int i) {return *t;}
  
	void pushBack(){}
	
	void pushBack(const T& input){}

	
	int size(){
		return 0;
	}
	
	void clear(){
	
	}
	

	void eraseValue(const T& item){
		
	}

	void erase(int i){
		
	}
	

	int find(const T& t){
		return 0;
	}

	~SafeArray(){
		
	}

	SafeArray(const SafeArray& s){
		
	}

	SafeArray& operator=(const SafeArray& s) {
		return *this;
	}

	SafeArray(){

	}
};



template<class INDEX, class VALUE>
class Map{
		
	private:
		VALUE v;
	public:
		VALUE& operator[](INDEX i) {	//return based on first element
			return v;
    	}
    	
		void clear(){
		
		}

    	bool find(INDEX i){
    		return true;
    	}
    	
		Array<INDEX> indices(){
			Array<INDEX> t;
			return t;
		}

		int size(){
			return 0;
		}
		
		Array<VALUE> values(){
			Array<VALUE> t;
			return t;
		}

		void remove(INDEX i){
			
		}

};

#elif defined(SMALL_CONTAINER)

class ArrayBase{
	private:
		std::vector<void*> data;
	public:
		void* baseIndex(unsigned int index);
		void baseReserve(unsigned int count);
		void basePushBack(void*);
		unsigned int baseSize() const;
		void basePopBack();
		void baseClear();
		void baseSwap(unsigned int, unsigned int);
		bool baseErase(unsigned int);
		void basePushFront(void*);
		int baseFind(void *);
		void baseReverse();
		~ArrayBase();
};

template<class T>
class Array:public ArrayBase{
	public:

	Array( Array<T>& in ){
		for(int i=0; i<in.size(); i++){
			pushBack(in[i]);
		}
	}

	T& operator[](unsigned int index){
		void* pointerto=baseIndex(index);
		T* t=(T*)pointerto;
		return *t;
    }
    
  	const T& operator[](unsigned int index) const{
		return base[index];
    }

	void reserve(unsigned int count){
		baseReserve(count);
	}
    
	void pushBack(){
		T* temp=new T;
		basePushBack(temp);
	}
	
	void pushBackUnique(const T& input){
		for(int i=0; i<baseSize(); i++){
			void* ind=baseIndex(i);
			T* t=(T*)ind;
			if(*t==input){return;}
		}

		T* temp=new T(input);
		basePushBack(temp);
	}
	
	void pushBack(const T& input){
		T* t=new T(input);
		basePushBack(t);
	}

	void pushFront(const T& input){
		T* t=new T(input);
		basePushFront(t);
	}
	
	void popBack(){
		basePopBack();
	}
	
	unsigned int size() const{
		return baseSize();
	}
	
	void clear(){
		baseClear();
	}
	
	bool eraseValue(const T& input){
		for(int i=0; i<baseSize(); i++){
			void* ind=baseIndex(i);
			T* t=(T*)ind;
			if(*t==input){
				erase(i);
				return true;
			}
		}

		return false;
	}

	bool erase(unsigned int index){
		return baseErase(index);
	}
	
	
	bool swap(unsigned int one, unsigned int two){
		return baseSwap(one,two);
	}
	
	unsigned int dataRef(const T& t){
		int f=find(t);
		if(f!=-1){return f;}

		pushBack(t);
		return size()-1;
	}
	
	int find(const T& t){
		for(unsigned int i=0; i<baseSize(); i++){
			void* ind=baseIndex(i);
			T* tind=(T*)ind;
			if(*tind==t){
				return i;
			}
		}

		return -1;
	}
	
	bool operator==(Array<T> in){
		if(in.size()!=baseSize()){
			return false;
		}
		
		for(unsigned int i=0; i<in.size(); i++){
			if(in[i]!=baseIndex(i)){
				return false;
			}
		}
		
		return true;
	}

	void reverse(){
		baseReverse();
	}
};


template<class T>
class SafeArray:public ArrayBase{
	public:

	T& operator[](unsigned int index){
		void* pointerto=baseIndex(index);
		T* t=(T*)pointerto;
		return *t;
    }
    
  	const T& operator[](unsigned int index) const{
		return base[index];
    }

	void reserve(unsigned int count){
		baseReserve(count);
	}
    
	void pushBack(){
		T* temp=new T;
		basePushBack(temp);
	}
	
	void pushBackUnique(const T& input){
		for(int i=0; i<baseSize(); i++){
			void* ind=baseIndex(i);
			T* t=(T*)ind;
			if(*t==input){return;}
		}

		T* temp=new T(input);
		basePushBack(temp);
	}
	
	void pushBack(const T& input){
		T* t=new T(input);
		basePushBack(t);
	}

	void pushFront(const T& input){
		T* t=new T(input);
		basePushFront(t);
	}
	
	void popBack(){
		basePopBack();
	}
	
	void popFront(){
		basePopFront();
	}
	
	unsigned int size() const{
		return baseSize();
	}
	
	void clear(){
		baseClear();
	}
	
	bool eraseValue(const T& input){
		for(int i=0; i<baseSize(); i++){
			void* ind=baseIndex(i);
			T* t=(T*)ind;
			if(*t==input){
				erase(i);
				return true;
			}
		}

		return false;
	}

	bool erase(unsigned int index){
		return baseErase(index);
	}
	
	
	bool swap(unsigned int one, unsigned int two){
		return baseSwap(one,two);
	}
	
	int find(const T& t){
		for(unsigned int i=0; i<baseSize(); i++){
			void* ind=baseIndex(i);
			T* tind=(T*)ind;
			if(*tind==t){
				return i;
			}
		}

		return -1;
	}

	SafeArray(){
			
	}
	
	bool operator==(Array<T> in){
		if(in.size()!=baseSize()){
			return false;
		}
		
		for(unsigned int i=0; i<in.size(); i++){
			if(in[i]!=baseIndex(i)){
				return false;
			}
		}
		
		return true;
	}

	void reverse(){
		baseReverse();
	}
};



template<class INDEX, class VALUE>
class Map{
		
	private:
		std::map<INDEX,VALUE> theMap;

	public:
		VALUE& operator[](INDEX i) {	//return based on first element
			return theMap[i];
    	}
    	
		void clear(){
			theMap.clear();
		}

    	bool find(INDEX i){
			if(theMap.find(i)!=theMap.end()){
    			return true;
    		}
    		
    		return false;
    	}
    	
		Array<INDEX> indices(){
			#ifdef _WIN32
				std::map<INDEX,VALUE>::iterator iter;
			#else
				typename std::map<INDEX,VALUE>::iterator iter;
			#endif
			
			Array<INDEX> final;

			for(iter = theMap.begin(); iter != theMap.end(); iter++)
			{
				final.pushBack(iter->first);
			}

			return final;
		}

		int size(){
			return values().size();
		}
		
		Array<VALUE> values(){
			#ifdef _WIN32
				std::map<INDEX,VALUE>::iterator iter;
			#else
				typename std::map<INDEX,VALUE>::iterator iter;
			#endif
			
			Array<VALUE> final;

			for(iter = theMap.begin(); iter != theMap.end(); iter++)
			{
				final.pushBack(iter->second);
			}

			return final;
			
		}

		void remove(INDEX i){
			theMap.erase(i);
		}

};


#else

//should maybe rewrite as a linked list with an index table?

template<class T>
class Array{
	private:
		std::vector<T> data;
	
	public:
	
	T& operator[](unsigned int index){
	
		#ifdef DEBUG
		if(index>=data.size() || index < 0){

			int lol=0;
		}
		#endif

		return data[index];
    }
    
  	const T& operator[](unsigned int index) const{

		#ifdef DEBUG
		if(index>=data.size() || index < 0){

			int lol=0;
		}
		#endif

		return data[index];
    }

	void reserve(int count){
		data.reserve(count);
	}
    
	void pushBack(){
		T temp;
		data.push_back(temp);
	}
	
	void pushBackUnique(const T& input){
		bool add=true;
		
		for(int i=0; i<data.size(); i++){
			if(data[i]==input){
				add=false;
			}
		}
		
		if(add){
			data.push_back(input);
		}
	}
	
	void pushBack(const T& input){
		data.push_back(input);
	}

	void pushFront(const T& input){
		data.push_back(T());
		for(int i=data.size()-1; i>0; i--){
			data[i]=data[i-1];
		}
		data[0]=input;
	}
	
	void popBack(){
		//pops off object at the end
		data.pop_back();
	}
	
	void popFront(){
		//pops off object at the begining
		data.pop_front();
	}
	
	int size() const{
		return (int)data.size();
	}
	
	void clear(){
		data.clear();
	}
	
	bool eraseValue(const T& input){
		int x=find(input);

		if(x==-1){return false;}

		return erase(x);
	}

	bool erase(int index){
		int size=data.size();
		
		if(size>0){
			T* temp=new T[size];
		
			for(int i=0; i<size; i++){
				temp[i]=data[i];
			}
		
			data.clear();
		
			for(int i=0; i<size; i++){
				if(i!=index){
					/*this may be a bug, the old way was:
    				data.push_back();
   					data[real_index]=temp[i];
					real_index++;
					*/
				
					data.push_back(temp[i]);
				}
			}

			delete[] temp;
		}

		return true;
	}
	
	
	bool swap(int one, int two){
		//swap two indexes in the array
		if(one!=two){
			T temp;
			temp=data[one];
			data[one]=data[two];
			data[two]=temp;
		}
		return true;
	}
	
	unsigned int dataRef(const T& t){
		//finds if the data already exists.  if not, makes it.  returns a ref to it.
		
		for(unsigned int i=0; i<data.size(); i++){
			if(t==data[i]){
				return i;
			}
		}
		
		pushBack(t);
		return data.size()-1;
	}
	
	int find(const T& t){
		//finds if the data already exists.  if not, returns -1 (dataRef makes a new value if it doesn't exist)
		
		for(unsigned int i=0; i<data.size(); i++){
			if(t==data[i]){
				return i;
			}
		}
		
		return -1;
	}

	Array(){
			
	}
	
	bool operator==(Array<T> in){
		if(in.size()!=data.size()){
			return false;
		}
		
		for(int i=0; i<in.size(); i++){
			if(in[i]!=data[i]){
				return false;
			}
		}
		
		return true;
	}

	void reverse(){
		std::vector<T> nd;

		for(unsigned int i=0; i<data.size(); i++){
			nd.push_back(data[data.size()-1-i]);
		}

		data=nd;


	}
};



/*this container supports a "safe erase" that will not reshuffle data when eraseing
do not use unless really needed, as it will slow stuff down*/

/*template<class T>
class SafeArray{
	private:
		std::vector<T> data;
		std::vector<int> erased;	//for safe erase
		Array<T*> index;
		bool safeEraseActive;
	
	public:
	
	T& operator[](int i) {
		//if(index<data.size()){ //prevent going over end of array

		if(safeEraseActive){
			return *index[i];
		}else{
			return data[i];
		}
    }
  
	void pushBack(){
		if(safeEraseActive){
			
			T temp;
			data[erased[0]]=temp;
			index.pushBack(&data[erased[0]]);
			
			if(erased.size()>1){
				std::vector<int> t;
				
				int i;
				for(i=1; i<erased.size(); i++){
					t.push_back(erased[i]);
				}
			
				erased.clear(); //clear because we're refilling with "t"
			
				for(i=0; i<t.size(); i++){
					erased.push_back(t[i]);
				}
			
			}else{
				//no longer any erased with safe erase
				erased.clear();
				safeEraseActive=false;
			}
			
			
			
			
		}else{
			data.push_back(input);
			//index.pushBack(&data[data.size()-1]);
			index.clear();
			for(int i=0; i<data.size(); i++){
				index.pushBack(&data[i]);
			}
		}
	}
	
	void pushBack(const T& input){
		if(safeEraseActive){
			
			data[erased[0]]=input;
			index.pushBack(&data[erased[0]]);
			
			if(erased.size()>1){
				std::vector<int> t;
				
				int i;
				for(i=1; i<erased.size(); i++){
					t.push_back(erased[i]);
				}
			
				erased.clear(); //clear because we're refilling with "t"
			
				for(i=0; i<t.size(); i++){
					erased.push_back(t[i]);
				}
			
			}else{
				//no longer any erased with safe erase
				erased.clear();
				safeEraseActive=false;
			}
			
			
			
			
		}else{
			data.push_back(input);
			//index.pushBack(&data[data.size()-1]);
			index.clear();
			for(int i=0; i<data.size(); i++){
				index.pushBack(&data[i]);
			}
		}
		
		//logs().main << "data.size" << data.size() << endLine;
        //logs().main << "index.size" << index.size() << endLine;
  		//return true;
	}

	
	int size(){
		return index.size();
	}
	
	void clear(){
		if(safeEraseActive){
			erased.clear();
			safeEraseActive=false;
		}
		
		index.clear();
		data.clear();
	}
	

	
	void erase(int i){
		erased.push_back(i);
		index.erase(i);
   		safeEraseActive=true;	
	}
	

	
		SafeArray(){
			safeEraseActive=false;
		}
};
*/

#ifdef LEAK_TEST
extern int outstandingAllocs;
#endif

template<class T>
class SafeArray{
	private:
		std::vector<T*> links;
	
	public:
	
	T& operator[](unsigned int i) {

		#ifdef DEBUG
		if(i>=links.size() || i < 0){

			int lol=0;
		}
		#endif

		return *links[i];
    }
  
	void pushBack(){
		T* temp=new T;
		
		links.push_back(temp);

		#ifdef LEAK_TEST
		outstandingAllocs++;
		#endif
	}
	
	void pushBack(const T& input){
		//make a local copy
		T* temp=new T;
		*temp=input;
		links.push_back(temp);

		#ifdef LEAK_TEST
		outstandingAllocs++;
		#endif
	}

	
	int size(){
		return links.size();
	}
	
	void clear(){
	
		for(int i=0; i<links.size(); i++){
			delete links[i];
			#ifdef LEAK_TEST
			outstandingAllocs--;
			#endif
		}
		
		links.clear();
	}
	

	void eraseValue(const T& item){
		int lol=find(item);

		if(lol!=-1){
			erase(lol);
		}
	}

	void erase(int i){
		delete links[i];
		#ifdef LEAK_TEST
		outstandingAllocs--;
		#endif

		std::vector<T*> m;
		
		for(unsigned int l=0; l<links.size(); l++){
			if(l!=i){
				m.push_back(links[l]);
			}
		}
		
		links.clear();
		
		for(unsigned int l=0; l<m.size(); l++){
				links.push_back(m[l]);
		}
   			
	}
	

	int find(const T& t){
		//finds if the data already exists.  if not, returns -1 (dataRef makes a new value if it doesn't exist)
		
		for(unsigned int i=0; i<links.size(); i++){
			if(t==*(links[i])){
				//cout << "found:" << i << endl;
				//return &data[i];
				return i;
			}
		}
		
		return -1;
	}

	~SafeArray(){
		//need to delete everything to prevent memory leaks

		for(unsigned int l=0; l<links.size(); l++){
			if(links[l]!=NULL){

				#ifdef LEAK_TEST
				outstandingAllocs--;
				#endif
				delete links[l];
			}
		}

		links.clear();
	}

	SafeArray(const SafeArray& s){
		for(unsigned int l=0; l<s.links.size(); l++){
			T* t=new T;
			*t=*(s.links[l]);

			#ifdef LEAK_TEST
			outstandingAllocs++;
			#endif

			links.push_back(t);
		}
	}

	SafeArray& operator=(const SafeArray& s) {
		for(unsigned int l=0; l<s.links.size(); l++){
			T* t=new T;
			*t=*(s.links[l]);

			#ifdef LEAK_TEST
			outstandingAllocs++;
			#endif

			links.push_back(t);
		}

		return *this;
	}

	SafeArray(){

	}
};



template<class INDEX, class VALUE>
class Map{
		
	private:
		std::map<INDEX,VALUE> theMap;
		
	public:
		VALUE& operator[](INDEX i) {	//return based on first element
			return theMap[i];
    	}
    	
		void clear(){
			theMap.clear();
		}

    	bool find(INDEX i){
			if(theMap.find(i)!=theMap.end()){
    			return true;
    		}
    		
    		return false;
    	}
    	
		Array<INDEX> indices(){
			
			#ifdef _WIN32
				std::map<INDEX,VALUE>::iterator iter;
			#else
				typename std::map<INDEX,VALUE>::iterator iter;
			#endif
			
			Array<INDEX> final;

			for(iter = theMap.begin(); iter != theMap.end(); iter++)
			{  
				final.pushBack(iter->first);
			}

			return final;
		}

		int size(){
			return values().size();
		}
		
		Array<VALUE> values(){
			#ifdef _WIN32
				std::map<INDEX,VALUE>::iterator iter;
			#else
				typename std::map<INDEX,VALUE>::iterator iter;
			#endif
			
			Array<VALUE> final;

			for(iter = theMap.begin(); iter != theMap.end(); iter++)
			{ 
				final.pushBack(iter->second);
			}

			return final;
			
		}

		void remove(INDEX i){
			theMap.erase(i);
		}
};




#endif


template<class KEY, class VALUE>
class SlowMap{
	private:
		std::vector<KEY> keys;
		std::vector<VALUE> values;
		
	public:
		VALUE& operator[](KEY k) {	//return based on first element

			for(unsigned int i=0; i<keys.size(); i++){
				if(keys[i]==k){
					return values[i];
				} 
			}

			values.push_back(VALUE());
			keys.push_back(k);

			return values[values.size()-1];
    	}
    	
    	bool find(KEY k){
    		for(unsigned int i=0; i<keys.size(); i++){
				if(keys[i]==k){
					return true;
				}
			}
    		
    		return false;
    	}

		void clear(){

			keys.clear();
			values.clear();

		}
    	
};

#endif
