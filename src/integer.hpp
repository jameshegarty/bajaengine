/* Copyright (c) 2006 the authors listed at the following URL:
http://literateprograms.org/Arbitrary-precision_integer_arithmetic_(C)?action=history&offset=20060719224301

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://literateprograms.org/Arbitrary-precision_integer_arithmetic_(C)?oldid=7039
*/

#ifndef _INTEGER_H_
#define _INTEGER_H_

#include <limits.h>  /* for CHAR_BIT */

typedef unsigned short component_t;
typedef unsigned long double_component_t;

#define MAX_COMPONENT  ((component_t)(-1))

#define COMPONENT_BITS  (sizeof(component_t)*CHAR_BIT)

#define LOG_2_10    3.3219280948873623478703194294894


#define MIN(x,y)  ((x)<(y) ? (x) : (y))
#define MAX(x,y)  ((x)>(y) ? (x) : (y))

//use this to test if the thing is doing stupid stuff
class CCC{
	private:
		component_t* ic;
		int nc;
	public:
		component_t& operator[](int index){
			return ic[index];
		}

		void alloc(int lol){
			ic=new component_t[lol];
			nc=lol;
		}

};

typedef struct {
    component_t* c;    /* least-significant word first */
	//CCC c;
    int num_components;
	//int livenums;
} integer;

integer optimize_integer(integer i);
integer create_integer(int components);
void free_integer(integer i);
void set_zero_integer(integer i);
void copy_integer(integer source, integer target);
void add_integer(integer left, integer right, integer result);
void subtract_integer(integer left, integer right, integer result);
void multiply_small_integer(integer left, component_t right, integer result);
void multiply_integer(integer left, integer right, integer result);
int compare_integers(integer left, integer right);
void shift_left_one_integer(integer arg);
void shift_right_one_integer(integer arg);
component_t mod_small_integer(integer left, component_t right);
void mod_integer(integer left, integer right, integer result);
void divide_small_integer(integer left, component_t right, integer result);
integer string_to_integer(char* s);
char* integer_to_string(integer x);
char* integer_to_string_remain(integer x);
int is_zero_integer(integer x);

#endif /* #ifndef _INTEGER_H_ */

