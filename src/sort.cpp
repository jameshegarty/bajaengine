#include "sort.hpp"

Sort sort;

/*
WARNING: CONTAINS MEMORY LEAKS

*/

/*void quicksort(int x[], int first, int last) {
    int pivIndex = 0;
    if(first < last) {
        pivIndex = partition(x,first, last);
        quicksort(x,first,(pivIndex-1));
        quicksort(x,(pivIndex+1),last);
    }
}

int partition(int y[], int f, int l) {
    int up,down,temp;
    int piv = y[f];
    up = f;
    down = l;
    do { 
        while (y[up] <= piv && up < l) {
            up++;
        }
        while (y[down] > piv  ) {
            down--;
        }
        if (up < down ) {
            temp = y[up];
            y[up] = y[down];
            y[down] = temp;
        }
    } while (down > up);
    temp = piv;
    y[f] = y[down];
    y[down] = piv;
    return down;
}
*/

void Sort::quicksort(SortItem x[], int first, int last) {
    int pivIndex = 0;
    if(first < last) {
        pivIndex = partition(x,first, last);
        quicksort(x,first,(pivIndex-1));
        quicksort(x,(pivIndex+1),last);
    }
}

int Sort::partition(SortItem y[], int f, int l) {
    int up,down;

	SortItem temp;
	SortItem piv = y[f];
    up = f;
    down = l;
    do { 
        while (y[up].value <= piv.value && up < l) {
            up++;
        }
        while (y[down].value > piv.value  ) {
            down--;
        }
        if (up < down ) {
            temp = y[up];
            y[up] = y[down];
            y[down] = temp;
        }
    } while (down > up);
    temp = piv;
    y[f] = y[down];
    y[down] = piv;
    return down;
}

void Sort::quicksort(SortItem x[], int first, int last,int (*compare)(const int key1, const int key2)) {
    int pivIndex = 0;
    if(first < last) {
        pivIndex = partition(x,first, last,compare);
        quicksort(x,first,(pivIndex-1));
        quicksort(x,(pivIndex+1),last);
    }
}

int Sort::partition(SortItem y[], int f, int l,int (*compare)(const int key1, const int key2)) {
    int up,down;

	SortItem temp;
	SortItem piv = y[f];
    up = f;
    down = l;
    do { 

		while ( (compare(y[up].key,piv.key)<=0) && up < l) {
            up++;
        }

		while (compare(y[down].key,piv.key)>0) {

            down--;
        }
        if (up < down ) {
            temp = y[up];
            y[up] = y[down];
            y[down] = temp;
        }
    } while (down > up);
    temp = piv;
    y[f] = y[down];
    y[down] = piv;
    return down;
}


Array<int> Sort::sort(const Array<SortItem>& input,bool asc){
	

	SortItem* si=new SortItem[input.size()];

	for(int i=0; i<input.size(); i++){
		si[i]=input[i];
	}

	quicksort(si,0,input.size()-1);

	Array<int> final;

	//NOTE: the first index[0] is phony, so we need to ignore it

	if(!asc){
		for(int i=input.size()-1; i>=0; i--){
			final.pushBack(si[i].key);
		}

	}else{
		for(int i=0; i<input.size(); i++){
			final.pushBack(si[i].key);
		}
	}

	return final;
}

int (*tc2)(const int key1, const int key2);

int tc1(const void* v1, const void* v2){
	int* i=(int*)v1;
	int* j=(int*)v2;

	return tc2(*i,*j);
}

Array<int> Sort::sort(const Array<SortItem>& input,int (*compare)(const int key1, const int key2)){
	tc2=compare;

	int* base=new int[input.size()*sizeof(int)];

	for(int i=0; i<input.size(); i++){
		base[i]=input[i].key;
	}

	SortItem* si=new SortItem[input.size()];

	for(int i=0; i<input.size(); i++){
		si[i]=input[i];
	}

	qsort(base,input.size(),sizeof(int),tc1);

	Array<int> final;

	for(int i=0; i<input.size(); i++){
		final.pushBack(base[i]);
	}

	return final;
}

Array<int> Sort::bubbleSort(Array<SortItem> input,int (*compare)(const int key1, const int key2)){

	for(int i=input.size()-1; i>=0; i--){
		for(int j=0; j<=(i-1); j++){
			if(compare(input[j].key,input[j+1].key)>0){
				SortItem t=input[j];
				input[j]=input[j+1];
				input[j+1]=t;
			}
		}
	}

	Array<int> final;

	for(int i=0; i<input.size(); i++){
		final.pushBack(input[i].key);
	}

	return final;
}

Array<int> Sort::bubbleSort(Array<SortItem> input,bool asc){

	for(int i=input.size()-1; i>=0; i--){
		for(int j=0; j<(i-1); j++){
			if(asc){
				if(input[j].value>input[j+1].value){
					SortItem t=input[j];
					input[j]=input[j+1];
					input[j+1]=t;
				}
			}else{
				if(input[j].value<input[j+1].value){
					SortItem t=input[j];
					input[j]=input[j+1];
					input[j+1]=t;
				}
			}
		}
	}

	Array<int> final;

	for(int i=0; i<input.size(); i++){
		final.pushBack(input[i].key);
	}

	return final;
}

void insert(Array<SortItem>* input, int length,int (*compare)(const int key1, const int key2),SortItem value){
	int i=length-1;

	while(i>=0 && compare((*input)[i].key,value.key)>0){
		(*input)[i+1]=(*input)[i];
		i=i-1;
	}

	(*input)[i+1]=value;
}

Array<int> Sort::insertionSort(Array<SortItem> input,int (*compare)(const int key1, const int key2)){

	int i=1;

	while(i<input.size()){
		insert(&input,i,compare,input[i]);

		i=i+1;
	}

	Array<int> final;

	for(int i=0; i<input.size(); i++){
		final.pushBack(input[i].key);
	}

	return final;
}