#ifndef SORT_H
#define SORT_H

#include "Containers.hpp"

class SortItem{
	public:
		float key;
		float value;

		SortItem(float k,float v){
			key=k;
			value=v;
		}

		SortItem(){
			
		}

};

class Sort{
	private:
		void quicksort(SortItem x[], int first, int last);
		int partition(SortItem y[], int f, int l);
		void quicksort(SortItem x[], int first, int last,int (*compare)(const int key1, const int key2));
		int partition(SortItem y[], int f, int l,int (*compare)(const int key1, const int key2));
	public:
		Array<int> sort(const Array<SortItem>&,bool asc);	//returns an array of the values in the 'key' variables in the sortItems, sorted based on the 'values' in the sortItems
		Array<int> sort(const Array<SortItem>&,int (*compare)(const int key1, const int key2)); //uses function 'compare' to compare values. 
			/*'compare(key1,key2) should return :
				<0	IF *key1 goes before *key2
				0	IF *key1 == *key2
				>0	IF *key1 goes after *key2*/
		Array<int> bubbleSort(Array<SortItem>,int (*compare)(const int key1, const int key2));
		Array<int> bubbleSort(Array<SortItem>,bool asc);

		Array<int> insertionSort(Array<SortItem>,int (*compare)(const int key1, const int key2));	//,bool sendKey=true);
};

extern Sort sort;


#endif