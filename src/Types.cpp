#include "Types.hpp"

#include <iostream>
#include "HelperLibMath.hpp"
#include "vectormath.hpp"

#include <math.h>



using namespace std;

bool operator != (SPACE_2D& in, SPACE_2D& out){
	if(in.x!=out.x){return true;}
	if(in.y!=out.y){return true;}
	if(in.sizeX!=out.sizeX){return true;}
	if(in.sizeY!=out.sizeY){return true;}
	
	return false;
}

bool operator != (Color& in, Color& out){
	if(in.r!=out.r){return true;}
	if(in.g!=out.g){return true;}
	if(in.b!=out.b){return true;}
	if(in.a!=out.a){return true;}
	
	return false;
}

Color::Color(){
r=1;
g=1;
b=1;
a=1;
}



float FloatVector2d::magnitude(){
	return pow(float(x*x+y*y),float(0.5));
}

FloatVector2d& FloatVector2d::operator /= (float t){
	float f = 1.0F / t;
	x *= f;
	y *= f;
	return (*this);
}

FloatVector2d& FloatVector2d::operator /= (FloatVector2d& t){
	x /= t.x;
	y /= t.y;
	return (*this);
}


FloatVector2d operator+(const FloatVector2d& left, const FloatVector2d& right){
	FloatVector2d final;
	final.x=left.x+right.x;
	final.y=left.y+right.y;
	return final;
}


FloatVector2d operator*(const float& left, const FloatVector2d& right){
	FloatVector2d final;
	final.x=left*right.x;
	final.y=left*right.y;
	return final;
}

FloatVector2d operator*(const FloatVector2d& left, const float& right){
	FloatVector2d final;
	final.x=left.x*right;
	final.y=left.y*right;
	return final;
}

void FloatVector2d::normalize(){
	*this /= sqrtf(x * x + y * y );
}

FloatVector2d::FloatVector2d(){
	x=0;
	y=0;
}

FloatVector2d::FloatVector2d(float a, float b){
	x=a;
	y=b;
}
  		

int operator==(const FloatVector2d& left, const FloatVector2d& right){
	return ((left.x==right.x) && (left.y==right.y) );
}

int operator==(const Ray3d& left, const Ray3d& right){
	return (left.start==right.start && left.end==right.end);
}

////////////////////////////////////////////////////////////////////////////////

FloatVector3d& FloatVector3d::normalize(){
	return (*this /= sqrtf(x * x + y * y + z * z));
}

FloatVector3d FloatVector3d::normalized(){
	FloatVector3d t=*this / sqrtf(x * x + y * y + z * z);
	return t;
}

DoubleVector3d DoubleVector3d::normalized(){
	DoubleVector3d t=*this / sqrtf(x * x + y * y + z * z);
	return t;
}


FloatVector2d FloatVector2d::normalized(){
	FloatVector2d t=*this / sqrtf(x * x + y * y);
	return t;
}


float FloatVector3d::magnitude(){
	float mag = (x*x)+(y*y)+(z*z);
    mag = sqrt(mag);
    
    return mag;
}

double DoubleVector3d::magnitude(){
	double mag = (x*x)+(y*y)+(z*z);
    mag = sqrt(mag);
    
    return mag;
}




FloatVector3d FloatVector3d::operator %(const FloatVector3d& v) const{
	return (FloatVector3d(y * v.z - z * v.y, z * v.x - x * v.z,x * v.y - y * v.x));
}

FloatVector3d& FloatVector3d::operator /= (float t){
	float f = 1.0F / t;
	x *= f;
	y *= f;
	z *= f;
	return (*this);
}

FloatVector3d& FloatVector3d::operator /= (FloatVector3d& t){
	x /= t.x;
	y /= t.y;
	z /= t.z;
	return (*this);
}


FloatVector3d::FloatVector3d(){
	x=0;
	y=0;
	z=0;
}

FloatVector3d::FloatVector3d(float a, float b, float c){
	x=a;
	y=b;
	z=c;
}
  		
void FloatVector3d::operator *= (const float s){
	x *= s;
	y *= s;
	z *= s;
}

void DoubleVector3d::operator *= (const float s){
	x *= s;
	y *= s;
	z *= s;
}

  		
FloatVector3d& FloatVector3d::operator *= (FloatVector3d& s){
	x *= s.z;
	y *= s.y;
	z *= s.z;
	return *this;
}


//even though it'd be faster to use the "left" var to store the result, DON'T
//it will change the value of that var outside of the function and mess up the number

FloatVector3d operator+(const FloatVector3d& left, const FloatVector3d& right){
	FloatVector3d final;
	final.x=left.x+right.x;
	final.y=left.y+right.y;
	final.z=left.z+right.z;
	return final;
}

FloatVector3d operator+(const FloatVector3d& left, const float& right){
	FloatVector3d final;
	final.x=left.x+right;
	final.y=left.y+right;
	final.z=left.z+right;
	return final;
}

int operator<(const FloatVector3d& left, const FloatVector3d& right){
	if(left.x<right.x){return true;}
	if(left.y<right.y){return true;}
	if(left.z<right.z){return true;}

	return false;
}

int operator<(const FloatVector3d& left, const float& right){
	if(left.x>right){return false;}
	if(left.y>right){return false;}
	if(left.z>right){return false;}

	return true;
}


DoubleVector3d operator+(const DoubleVector3d& left, const DoubleVector3d& right){
	DoubleVector3d final;
	final.x=left.x+right.x;
	final.y=left.y+right.y;
	final.z=left.z+right.z;
	return final;
}

DoubleVector3d operator+(const DoubleVector3d& left, const FloatVector3d& right){
	DoubleVector3d final;
	final.x=left.x+right.x;
	final.y=left.y+right.y;
	final.z=left.z+right.z;
	return final;
}

DoubleVector3d operator-(const FloatVector3d& left, const DoubleVector3d& right){
	DoubleVector3d final;
	final.x=left.x-right.x;
	final.y=left.y-right.y;
	final.z=left.z-right.z;
	return final;
}

DoubleVector3d operator-(const DoubleVector3d& left, const DoubleVector3d& right){
	DoubleVector3d final;
	final.x=left.x-right.x;
	final.y=left.y-right.y;
	final.z=left.z-right.z;
	return final;
}

DoubleVector3d operator-(const DoubleVector3d& left, const FloatVector3d& right){
	DoubleVector3d final;
	final.x=left.x-right.x;
	final.y=left.y-right.y;
	final.z=left.z-right.z;
	return final;
}
  	  		
FloatVector2d operator-(const FloatVector2d& left, const FloatVector2d& right){
	FloatVector2d final;
	final.x=left.x-right.x;
	final.y=left.y-right.y;
	return final;
}

FloatVector3d operator-(const FloatVector3d& left, const FloatVector3d& right){
	FloatVector3d final;
	final.x=left.x-right.x;
	final.y=left.y-right.y;
	final.z=left.z-right.z;
	return final;
}

FloatVector3d operator-(const FloatVector3d& input){
	FloatVector3d final;
    		
	final.x=-input.x;
	final.y=-input.y;
	final.z=-input.z;
	return final;
}


DoubleVector3d operator-(const DoubleVector3d& input){
	DoubleVector3d final;
    		
	final.x=-input.x;
	final.y=-input.y;
	final.z=-input.z;
	return final;
}


  
FloatVector3d operator*(const FloatVector3d& left, const FloatVector3d& right){
	FloatVector3d final;
	final.x=left.x*right.x;
	final.y=left.y*right.y;
	final.z=left.z*right.z;
	return final;
}

FloatVector3d operator*(const FloatVector3d& left, const float& right){
	FloatVector3d final;
	final.x=left.x*right;
	final.y=left.y*right;
	final.z=left.z*right;
	return final;
}

FloatVector3d operator*(const FloatVector3d& left, const double& right){
	FloatVector3d final;
	final.x=left.x*right;
	final.y=left.y*right;
	final.z=left.z*right;
	return final;
}

FloatVector3d operator*(const float& right, const FloatVector3d& left){
	FloatVector3d final;
	final.x=left.x*right;
	final.y=left.y*right;
	final.z=left.z*right;
	return final;
}

DoubleVector3d operator*(const double& right, const DoubleVector3d& left){
	DoubleVector3d final;
	final.x=left.x*right;
	final.y=left.y*right;
	final.z=left.z*right;
	return final;
}

FloatVector3d operator/(const FloatVector3d& left, const float& right){
	FloatVector3d final;
	final.x=left.x/right;
	final.y=left.y/right;
	final.z=left.z/right;
	return final;
}


FloatVector3d operator/(const FloatVector3d& left, const FloatVector3d& right){
	FloatVector3d final;
	final.x=left.x/right.x;
	final.y=left.y/right.y;
	final.z=left.z/right.z;
	return final;
}
void operator+=(FloatVector3d& right, const FloatVector3d& left) {
	right.x += left.x;
	right.y += left.y;
	right.z += left.z;
}

void operator+=(DoubleVector3d& right, const FloatVector3d& left) {
	right.x += left.x;
	right.y += left.y;
	right.z += left.z;
}


void FloatVector3d::operator-=(const FloatVector3d& left) {
	x -= left.x;
	y -= left.y;
	z -= left.z;
}

void DoubleVector3d::operator-=(const DoubleVector3d& left) {
	x -= left.x;
	y -= left.y;
	z -= left.z;
}
		
int operator!=(const FloatVector3d& left, const float& right){
	return !((left.x == right) && (left.y == right) && (left.z == right));
}
		
int operator==(const FloatVector3d& left, const FloatVector3d& right){
	return ((left.x==right.x) && (left.y==right.y) && (left.z==right.z));
}



int operator>(const FloatVector3d& left, const int right){
	return ((left.x>float(right)) && (left.y>float(right)) && (left.z>float(right)));
}

		
	
int operator<(const IntVector3d& left, const IntVector3d& right){
	if( (left.x<right.x) && (left.y<right.y) && (left.z<right.z)){
		return true;
	}

	return false;
}


void operator+=(FloatVector4d& right, const FloatVector4d& left) {
	right.x += left.x;
	right.y += left.y;
	right.z += left.z;
	right.w += left.w;
}

void operator-=(FloatVector4d& right, const DoubleVector3d& left) {
	right.x -= left.x;
	right.y -= left.y;
	right.z -= left.z;
}


FloatVector4d operator+(const FloatVector4d& left, const float& right){
	FloatVector4d final;
	final.x=left.x+right;
	final.y=left.y+right;
	final.z=left.z+right;
	final.w=left.w+right;
	return final;
}

FloatVector4d operator+(const FloatVector4d& left, const double& right){
	FloatVector4d final;
	final.x=left.x+right;
	final.y=left.y+right;
	final.z=left.z+right;
	final.w=left.w+right;
	return final;
}

FloatVector4d operator+(const double& right,const FloatVector4d& left){
	FloatVector4d final;
	final.x=left.x+right;
	final.y=left.y+right;
	final.z=left.z+right;
	final.w=left.w+right;
	return final;
}

FloatVector4d operator+(const FloatVector4d& right,const FloatVector4d& left){
	FloatVector4d final;
	final.x=left.x+right.x;
	final.y=left.y+right.y;
	final.z=left.z+right.z;
	final.w=left.w+right.w;
	return final;

}
FloatVector4d operator*(const double& right,const FloatVector4d& left){
	FloatVector4d final;
	final.x=left.x*right;
	final.y=left.y*right;
	final.z=left.z*right;
	final.w=left.w*right;
	return final;
}

FloatVector4d operator*(const FloatVector4d& left,const double& right){
	FloatVector4d final;
	final.x=left.x*right;
	final.y=left.y*right;
	final.z=left.z*right;
	final.w=left.w*right;
	return final;
}

FloatVector4d operator*(const FloatVector4d& left,const float& right){
	FloatVector4d final;
	final.x=left.x*right;
	final.y=left.y*right;
	final.z=left.z*right;
	final.w=left.w*right;
	return final;
}

///////////////////////////////////////////

FloatVector4d Matrix4d4d::row(int i){

	FloatVector4d r;

	if(i==0){
		r.x=entries[0];
		r.y=entries[1];
		r.z=entries[2];
		r.w=entries[3];
	}else if(i==1){
		r.x=entries[4];
		r.y=entries[5];
		r.z=entries[6];
		r.w=entries[7];
	}else if(i==2){
		r.x=entries[8];
		r.y=entries[9];
		r.z=entries[10];
		r.w=entries[11];
	}else if(i==3){
		r.x=entries[12];
		r.y=entries[13];
		r.z=entries[14];
		r.w=entries[15];
	}

	return r;
};

Matrix4d4d::Matrix4d4d(float v00, float v01, float v02, float v03,
			float v10, float v11, float v12, float v13,
			float v20, float v21, float v22, float v23,
			float v30, float v31, float v32, float v33){

	entries[0]=v00;
	entries[1]=v01;
	entries[2]=v02;
	entries[3]=v03;

	entries[4]=v10;
	entries[5]=v11;
	entries[6]=v12;
	entries[7]=v13;

	entries[8]=v20;
	entries[9]=v21;
	entries[10]=v22;
	entries[11]=v23;

	entries[12]=v30;
	entries[13]=v31;
	entries[14]=v32;
	entries[15]=v33;


}


Matrix4d4d Matrix4d4d::operator+(const Matrix4d4d & rhs) const		//overloaded operators
{
	return Matrix4d4d(	entries[0]+rhs.entries[0],
						entries[1]+rhs.entries[1],
						entries[2]+rhs.entries[2],
						entries[3]+rhs.entries[3],
						entries[4]+rhs.entries[4],
						entries[5]+rhs.entries[5],
						entries[6]+rhs.entries[6],
						entries[7]+rhs.entries[7],
						entries[8]+rhs.entries[8],
						entries[9]+rhs.entries[9],
						entries[10]+rhs.entries[10],
						entries[11]+rhs.entries[11],
						entries[12]+rhs.entries[12],
						entries[13]+rhs.entries[13],
						entries[14]+rhs.entries[14],
						entries[15]+rhs.entries[15]);
}

Matrix4d4d Matrix4d4d::operator-(const Matrix4d4d & rhs) const		//overloaded operators
{
	return Matrix4d4d(	entries[0]-rhs.entries[0],
						entries[1]-rhs.entries[1],
						entries[2]-rhs.entries[2],
						entries[3]-rhs.entries[3],
						entries[4]-rhs.entries[4],
						entries[5]-rhs.entries[5],
						entries[6]-rhs.entries[6],
						entries[7]-rhs.entries[7],
						entries[8]-rhs.entries[8],
						entries[9]-rhs.entries[9],
						entries[10]-rhs.entries[10],
						entries[11]-rhs.entries[11],
						entries[12]-rhs.entries[12],
						entries[13]-rhs.entries[13],
						entries[14]-rhs.entries[14],
						entries[15]-rhs.entries[15]);
}

Matrix4d4d Matrix4d4d::operator*(const Matrix4d4d & rhs) const
{
	//Optimise for matrices in which bottom row is (0, 0, 0, 1) in both matrices
	if(	entries[3]==0.0f && entries[7]==0.0f && entries[11]==0.0f && entries[15]==1.0f	&&
		rhs.entries[3]==0.0f && rhs.entries[7]==0.0f &&
		rhs.entries[11]==0.0f && rhs.entries[15]==1.0f)
	{
		return Matrix4d4d(	entries[0]*rhs.entries[0]+entries[4]*rhs.entries[1]+entries[8]*rhs.entries[2],
							entries[1]*rhs.entries[0]+entries[5]*rhs.entries[1]+entries[9]*rhs.entries[2],
							entries[2]*rhs.entries[0]+entries[6]*rhs.entries[1]+entries[10]*rhs.entries[2],
							0.0f,
							entries[0]*rhs.entries[4]+entries[4]*rhs.entries[5]+entries[8]*rhs.entries[6],
							entries[1]*rhs.entries[4]+entries[5]*rhs.entries[5]+entries[9]*rhs.entries[6],
							entries[2]*rhs.entries[4]+entries[6]*rhs.entries[5]+entries[10]*rhs.entries[6],
							0.0f,
							entries[0]*rhs.entries[8]+entries[4]*rhs.entries[9]+entries[8]*rhs.entries[10],
							entries[1]*rhs.entries[8]+entries[5]*rhs.entries[9]+entries[9]*rhs.entries[10],
							entries[2]*rhs.entries[8]+entries[6]*rhs.entries[9]+entries[10]*rhs.entries[10],
							0.0f,
							entries[0]*rhs.entries[12]+entries[4]*rhs.entries[13]+entries[8]*rhs.entries[14]+entries[12],
							entries[1]*rhs.entries[12]+entries[5]*rhs.entries[13]+entries[9]*rhs.entries[14]+entries[13],
							entries[2]*rhs.entries[12]+entries[6]*rhs.entries[13]+entries[10]*rhs.entries[14]+entries[14],
							1.0f);
	}

	//Optimise for when bottom row of 1st matrix is (0, 0, 0, 1)
	if(	entries[3]==0.0f && entries[7]==0.0f && entries[11]==0.0f && entries[15]==1.0f)
	{
		return Matrix4d4d(	entries[0]*rhs.entries[0]+entries[4]*rhs.entries[1]+entries[8]*rhs.entries[2]+entries[12]*rhs.entries[3],
							entries[1]*rhs.entries[0]+entries[5]*rhs.entries[1]+entries[9]*rhs.entries[2]+entries[13]*rhs.entries[3],
							entries[2]*rhs.entries[0]+entries[6]*rhs.entries[1]+entries[10]*rhs.entries[2]+entries[14]*rhs.entries[3],
							rhs.entries[3],
							entries[0]*rhs.entries[4]+entries[4]*rhs.entries[5]+entries[8]*rhs.entries[6]+entries[12]*rhs.entries[7],
							entries[1]*rhs.entries[4]+entries[5]*rhs.entries[5]+entries[9]*rhs.entries[6]+entries[13]*rhs.entries[7],
							entries[2]*rhs.entries[4]+entries[6]*rhs.entries[5]+entries[10]*rhs.entries[6]+entries[14]*rhs.entries[7],
							rhs.entries[7],
							entries[0]*rhs.entries[8]+entries[4]*rhs.entries[9]+entries[8]*rhs.entries[10]+entries[12]*rhs.entries[11],
							entries[1]*rhs.entries[8]+entries[5]*rhs.entries[9]+entries[9]*rhs.entries[10]+entries[13]*rhs.entries[11],
							entries[2]*rhs.entries[8]+entries[6]*rhs.entries[9]+entries[10]*rhs.entries[10]+entries[14]*rhs.entries[11],
							rhs.entries[11],
							entries[0]*rhs.entries[12]+entries[4]*rhs.entries[13]+entries[8]*rhs.entries[14]+entries[12]*rhs.entries[15],
							entries[1]*rhs.entries[12]+entries[5]*rhs.entries[13]+entries[9]*rhs.entries[14]+entries[13]*rhs.entries[15],
							entries[2]*rhs.entries[12]+entries[6]*rhs.entries[13]+entries[10]*rhs.entries[14]+entries[14]*rhs.entries[15],
							rhs.entries[15]);
	}

	//Optimise for when bottom row of 2nd matrix is (0, 0, 0, 1)
	if(	rhs.entries[3]==0.0f && rhs.entries[7]==0.0f &&
		rhs.entries[11]==0.0f && rhs.entries[15]==1.0f)
	{
		return Matrix4d4d(	entries[0]*rhs.entries[0]+entries[4]*rhs.entries[1]+entries[8]*rhs.entries[2],
							entries[1]*rhs.entries[0]+entries[5]*rhs.entries[1]+entries[9]*rhs.entries[2],
							entries[2]*rhs.entries[0]+entries[6]*rhs.entries[1]+entries[10]*rhs.entries[2],
							entries[3]*rhs.entries[0]+entries[7]*rhs.entries[1]+entries[11]*rhs.entries[2],
							entries[0]*rhs.entries[4]+entries[4]*rhs.entries[5]+entries[8]*rhs.entries[6],
							entries[1]*rhs.entries[4]+entries[5]*rhs.entries[5]+entries[9]*rhs.entries[6],
							entries[2]*rhs.entries[4]+entries[6]*rhs.entries[5]+entries[10]*rhs.entries[6],
							entries[3]*rhs.entries[4]+entries[7]*rhs.entries[5]+entries[11]*rhs.entries[6],
							entries[0]*rhs.entries[8]+entries[4]*rhs.entries[9]+entries[8]*rhs.entries[10],
							entries[1]*rhs.entries[8]+entries[5]*rhs.entries[9]+entries[9]*rhs.entries[10],
							entries[2]*rhs.entries[8]+entries[6]*rhs.entries[9]+entries[10]*rhs.entries[10],
							entries[3]*rhs.entries[8]+entries[7]*rhs.entries[9]+entries[11]*rhs.entries[10],
							entries[0]*rhs.entries[12]+entries[4]*rhs.entries[13]+entries[8]*rhs.entries[14]+entries[12],
							entries[1]*rhs.entries[12]+entries[5]*rhs.entries[13]+entries[9]*rhs.entries[14]+entries[13],
							entries[2]*rhs.entries[12]+entries[6]*rhs.entries[13]+entries[10]*rhs.entries[14]+entries[14],
							entries[3]*rhs.entries[12]+entries[7]*rhs.entries[13]+entries[11]*rhs.entries[14]+entries[15]);
	}	
	
	return Matrix4d4d(	entries[0]*rhs.entries[0]+entries[4]*rhs.entries[1]+entries[8]*rhs.entries[2]+entries[12]*rhs.entries[3],
						entries[1]*rhs.entries[0]+entries[5]*rhs.entries[1]+entries[9]*rhs.entries[2]+entries[13]*rhs.entries[3],
						entries[2]*rhs.entries[0]+entries[6]*rhs.entries[1]+entries[10]*rhs.entries[2]+entries[14]*rhs.entries[3],
						entries[3]*rhs.entries[0]+entries[7]*rhs.entries[1]+entries[11]*rhs.entries[2]+entries[15]*rhs.entries[3],
						entries[0]*rhs.entries[4]+entries[4]*rhs.entries[5]+entries[8]*rhs.entries[6]+entries[12]*rhs.entries[7],
						entries[1]*rhs.entries[4]+entries[5]*rhs.entries[5]+entries[9]*rhs.entries[6]+entries[13]*rhs.entries[7],
						entries[2]*rhs.entries[4]+entries[6]*rhs.entries[5]+entries[10]*rhs.entries[6]+entries[14]*rhs.entries[7],
						entries[3]*rhs.entries[4]+entries[7]*rhs.entries[5]+entries[11]*rhs.entries[6]+entries[15]*rhs.entries[7],
						entries[0]*rhs.entries[8]+entries[4]*rhs.entries[9]+entries[8]*rhs.entries[10]+entries[12]*rhs.entries[11],
						entries[1]*rhs.entries[8]+entries[5]*rhs.entries[9]+entries[9]*rhs.entries[10]+entries[13]*rhs.entries[11],
						entries[2]*rhs.entries[8]+entries[6]*rhs.entries[9]+entries[10]*rhs.entries[10]+entries[14]*rhs.entries[11],
						entries[3]*rhs.entries[8]+entries[7]*rhs.entries[9]+entries[11]*rhs.entries[10]+entries[15]*rhs.entries[11],
						entries[0]*rhs.entries[12]+entries[4]*rhs.entries[13]+entries[8]*rhs.entries[14]+entries[12]*rhs.entries[15],
						entries[1]*rhs.entries[12]+entries[5]*rhs.entries[13]+entries[9]*rhs.entries[14]+entries[13]*rhs.entries[15],
						entries[2]*rhs.entries[12]+entries[6]*rhs.entries[13]+entries[10]*rhs.entries[14]+entries[14]*rhs.entries[15],
						entries[3]*rhs.entries[12]+entries[7]*rhs.entries[13]+entries[11]*rhs.entries[14]+entries[15]*rhs.entries[15]);
}

Matrix4d4d Matrix4d4d::operator*(const float rhs) const
{
	return Matrix4d4d(	entries[0]*rhs,
						entries[1]*rhs,
						entries[2]*rhs,
						entries[3]*rhs,
						entries[4]*rhs,
						entries[5]*rhs,
						entries[6]*rhs,
						entries[7]*rhs,
						entries[8]*rhs,
						entries[9]*rhs,
						entries[10]*rhs,
						entries[11]*rhs,
						entries[12]*rhs,
						entries[13]*rhs,
						entries[14]*rhs,
						entries[15]*rhs);
}

FloatVector4d Matrix4d4d::operator*(const FloatVector4d & rhs) const		//overloaded operators
{
	return FloatVector4d(	entries[0]*rhs.x+entries[4]*rhs.y+entries[8]*rhs.z+entries[12]*rhs.w,
							entries[1]*rhs.x+entries[5]*rhs.y+entries[9]*rhs.z+entries[13]*rhs.w,
							entries[2]*rhs.x+entries[6]*rhs.y+entries[10]*rhs.z+entries[14]*rhs.w,
							entries[3]*rhs.x+entries[7]*rhs.y+entries[11]*rhs.z+entries[15]*rhs.w);
}

Matrix4d4d::Matrix4d4d(){

}

void Matrix4d4d::loadIdentity(){
	entries[0]=1;
	entries[1]=0;
	entries[2]=0;
	entries[3]=0;
	entries[4]=0;
	entries[5]=1;
	entries[6]=0;
	entries[7]=0;
	entries[8]=0;
	entries[9]=0;
	entries[10]=1;
	entries[11]=0;
	entries[12]=0;
	entries[13]=0;
	entries[14]=0;
	entries[15]=1;
}

void Matrix4d4d::translate(float x,float y, float z){
	FloatVector3d v=FloatVector3d(x,y,z);

	entries[12]+=math.dotProduct(FloatVector3d(entries[0],entries[4],entries[8]),v);
	entries[13]+=math.dotProduct(FloatVector3d(entries[1],entries[5],entries[9]),v);
	entries[14]+=math.dotProduct(FloatVector3d(entries[2],entries[6],entries[10]),v);
	entries[15]+=math.dotProduct(FloatVector3d(entries[3],entries[7],entries[11]),v);
}

void Matrix4d4d::translateRaw(float x,float y, float z){
	entries[12]+=x;
	entries[13]+=y;
	entries[14]+=z;
}

void Matrix4d4d::scale(float x,float y, float z){
	Matrix4d4d mat4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1);

	Matrix4d4d t=*this*mat4;
	entries[0]=t.entries[0];
	entries[1]=t.entries[1];
	entries[2]=t.entries[2];
	entries[3]=t.entries[3];
	entries[4]=t.entries[4];
	entries[5]=t.entries[5];
	entries[6]=t.entries[6];
	entries[7]=t.entries[7];
	entries[8]=t.entries[8];
	entries[9]=t.entries[9];
	entries[10]=t.entries[10];
	entries[11]=t.entries[11];
	entries[12]=t.entries[12];
	entries[13]=t.entries[13];
	entries[14]=t.entries[14];
	entries[15]=t.entries[15];
}

void Matrix4d4d::rotateX(float angle){
	float cosA = math.cosDegrees(angle), sinA = math.sinDegrees(angle);

	Matrix4d4d mat4(
		1, 0,     0,    0,
		0, cosA, sinA, 0,
		0, -sinA,  cosA, 0,
		0, 0,     0,    1);

	Matrix4d4d t=*this*mat4;
	entries[0]=t.entries[0];
	entries[1]=t.entries[1];
	entries[2]=t.entries[2];
	entries[3]=t.entries[3];
	entries[4]=t.entries[4];
	entries[5]=t.entries[5];
	entries[6]=t.entries[6];
	entries[7]=t.entries[7];
	entries[8]=t.entries[8];
	entries[9]=t.entries[9];
	entries[10]=t.entries[10];
	entries[11]=t.entries[11];
	entries[12]=t.entries[12];
	entries[13]=t.entries[13];
	entries[14]=t.entries[14];
	entries[15]=t.entries[15];

}

void Matrix4d4d::rotateY(float angle){
	float cosA = math.cosDegrees(angle), sinA = math.sinDegrees(angle);

	Matrix4d4d mat4(
		cosA, 0, -sinA, 0,
		0,    1,  0,    0,
		sinA, 0,  cosA, 0,
		0,    0,  0,    1);

	Matrix4d4d t=*this*mat4;
	entries[0]=t.entries[0];
	entries[1]=t.entries[1];
	entries[2]=t.entries[2];
	entries[3]=t.entries[3];
	entries[4]=t.entries[4];
	entries[5]=t.entries[5];
	entries[6]=t.entries[6];
	entries[7]=t.entries[7];
	entries[8]=t.entries[8];
	entries[9]=t.entries[9];
	entries[10]=t.entries[10];
	entries[11]=t.entries[11];
	entries[12]=t.entries[12];
	entries[13]=t.entries[13];
	entries[14]=t.entries[14];
	entries[15]=t.entries[15];
}

void Matrix4d4d::transpose(){

	double entriesT[16];
	entriesT[0]=entries[0];
	entriesT[1]=entries[1];
	entriesT[2]=entries[2];
	entriesT[3]=entries[3];

	entriesT[4]=entries[4];
	entriesT[5]=entries[5];
	entriesT[6]=entries[6];
	entriesT[7]=entries[7];

	entriesT[8]=entries[8];
	entriesT[9]=entries[9];
	entriesT[10]=entries[10];
	entriesT[11]=entries[11];

	entriesT[12]=entries[12];
	entriesT[13]=entries[13];
	entriesT[14]=entries[14];
	entriesT[15]=entries[15];

	entries[0]=entriesT[0];
	entries[1]=entriesT[4];
	entries[2]=entriesT[8];
	entries[3]=entriesT[12];

	entries[4]=entriesT[1];
	entries[5]=entriesT[5];
	entries[6]=entriesT[9];
	entries[7]=entriesT[13];

	entries[8]=entriesT[2];
	entries[9]=entriesT[6];
	entries[10]=entriesT[10];
	entries[11]=entriesT[14];

	entries[12]=entriesT[3];
	entries[13]=entriesT[7];
	entries[14]=entriesT[11];
	entries[15]=entriesT[15];
}

void Matrix4d4d::invert(){
	Matrix4d4d mat;
	Matrix4d4d m=*this;

	float p00 = m.row(2).z * m.row(3).w;
	float p01 = m.row(3).z * m.row(2).w;
	float p02 = m.row(1).z * m.row(3).w;
	float p03 = m.row(3).z * m.row(1).w;
	float p04 = m.row(1).z * m.row(2).w;
	float p05 = m.row(2).z * m.row(1).w;
	float p06 = m.row(0).z * m.row(3).w;
	float p07 = m.row(3).z * m.row(0).w;
	float p08 = m.row(0).z * m.row(2).w;
	float p09 = m.row(2).z * m.row(0).w;
	float p10 = m.row(0).z * m.row(1).w;
	float p11 = m.row(1).z * m.row(0).w;

	mat.entries[0] = (p00 * m.row(1).y + p03 * m.row(2).y + p04 * m.row(3).y) - (p01 * m.row(1).y + p02 * m.row(2).y + p05 * m.row(3).y);
	mat.entries[4] = (p01 * m.row(0).y + p06 * m.row(2).y + p09 * m.row(3).y) - (p00 * m.row(0).y + p07 * m.row(2).y + p08 * m.row(3).y);
	mat.entries[8] = (p02 * m.row(0).y + p07 * m.row(1).y + p10 * m.row(3).y) - (p03 * m.row(0).y + p06 * m.row(1).y + p11 * m.row(3).y);
	mat.entries[12] = (p05 * m.row(0).y + p08 * m.row(1).y + p11 * m.row(2).y) - (p04 * m.row(0).y + p09 * m.row(1).y + p10 * m.row(2).y);
	mat.entries[1] = (p01 * m.row(1).x + p02 * m.row(2).x + p05 * m.row(3).x) - (p00 * m.row(1).x + p03 * m.row(2).x + p04 * m.row(3).x);
	mat.entries[5] = (p00 * m.row(0).x + p07 * m.row(2).x + p08 * m.row(3).x) - (p01 * m.row(0).x + p06 * m.row(2).x + p09 * m.row(3).x);
	mat.entries[9] = (p03 * m.row(0).x + p06 * m.row(1).x + p11 * m.row(3).x) - (p02 * m.row(0).x + p07 * m.row(1).x + p10 * m.row(3).x);
	mat.entries[13] = (p04 * m.row(0).x + p09 * m.row(1).x + p10 * m.row(2).x) - (p05 * m.row(0).x + p08 * m.row(1).x + p11 * m.row(2).x);

	float q00 = m.row(2).x * m.row(3).y;
	float q01 = m.row(3).x * m.row(2).y;
	float q02 = m.row(1).x * m.row(3).y;
	float q03 = m.row(3).x * m.row(1).y;
	float q04 = m.row(1).x * m.row(2).y;
	float q05 = m.row(2).x * m.row(1).y;
	float q06 = m.row(0).x * m.row(3).y;
	float q07 = m.row(3).x * m.row(0).y;
	float q08 = m.row(0).x * m.row(2).y;
	float q09 = m.row(2).x * m.row(0).y;
	float q10 = m.row(0).x * m.row(1).y;
	float q11 = m.row(1).x * m.row(0).y;

	mat.entries[2] = (q00 * m.row(1).w + q03 * m.row(2).w + q04 * m.row(3).w) - (q01 * m.row(1).w + q02 * m.row(2).w + q05 * m.row(3).w);
	mat.entries[6] = (q01 * m.row(0).w + q06 * m.row(2).w + q09 * m.row(3).w) - (q00 * m.row(0).w + q07 * m.row(2).w + q08 * m.row(3).w);
	mat.entries[10] = (q02 * m.row(0).w + q07 * m.row(1).w + q10 * m.row(3).w) - (q03 * m.row(0).w + q06 * m.row(1).w + q11 * m.row(3).w);
	mat.entries[14] = (q05 * m.row(0).w + q08 * m.row(1).w + q11 * m.row(2).w) - (q04 * m.row(0).w + q09 * m.row(1).w + q10 * m.row(2).w);
	mat.entries[3] = (q02 * m.row(2).z + q05 * m.row(3).z + q01 * m.row(1).z) - (q04 * m.row(3).z + q00 * m.row(1).z + q03 * m.row(2).z);
	mat.entries[7] = (q08 * m.row(3).z + q00 * m.row(0).z + q07 * m.row(2).z) - (q06 * m.row(2).z + q09 * m.row(3).z + q01 * m.row(0).z);
	mat.entries[11]= (q06 * m.row(1).z + q11 * m.row(3).z + q03 * m.row(0).z) - (q10 * m.row(3).z + q02 * m.row(0).z + q07 * m.row(1).z);
	mat.entries[15] = (q10 * m.row(2).z + q04 * m.row(0).z + q09 * m.row(1).z) - (q08 * m.row(1).z + q11 * m.row(2).z + q05 * m.row(0).z);

	float det = m.row(0).w * m.row(1).z * m.row(2).y * m.row(3).x-m.row(0).z * m.row(1).w * m.row(2).y * m.row(3).x-m.row(0).w * m.row(1).y * m.row(2).z * m.row(3).x+m.row(0).y * m.row(1).w * m.row(2).z * m.row(3).x+
      m.row(0).z * m.row(1).y * m.row(2).w * m.row(3).x-m.row(0).y * m.row(1).z * m.row(2).w * m.row(3).x-m.row(0).w * m.row(1).z * m.row(2).x * m.row(3).y+m.row(0).z * m.row(1).w * m.row(2).x * m.row(3).y+
      m.row(0).w * m.row(1).x * m.row(2).z * m.row(3).y-m.row(0).x * m.row(1).w * m.row(2).z * m.row(3).y-m.row(0).z * m.row(1).x * m.row(2).w * m.row(3).y+m.row(0).x * m.row(1).z * m.row(2).w * m.row(3).y+
      m.row(0).w * m.row(1).y * m.row(2).x * m.row(3).z-m.row(0).y * m.row(1).w * m.row(2).x * m.row(3).z-m.row(0).w * m.row(1).x * m.row(2).y * m.row(3).z+m.row(0).x * m.row(1).w * m.row(2).y * m.row(3).z+
      m.row(0).y * m.row(1).x * m.row(2).w * m.row(3).z-m.row(0).x * m.row(1).y * m.row(2).w * m.row(3).z-m.row(0).z * m.row(1).y * m.row(2).x * m.row(3).w+m.row(0).y * m.row(1).z * m.row(2).x * m.row(3).w+
      m.row(0).z * m.row(1).x * m.row(2).y * m.row(3).w-m.row(0).x * m.row(1).z * m.row(2).y * m.row(3).w-m.row(0).y * m.row(1).x * m.row(2).z * m.row(3).w+m.row(0).x * m.row(1).y * m.row(2).z * m.row(3).w;

	mat=mat * (1.0f / det);

	entries[0]=mat.entries[0];
	entries[1]=mat.entries[1];
	entries[2]=mat.entries[2];
	entries[3]=mat.entries[3];
	entries[4]=mat.entries[4];
	entries[5]=mat.entries[5];
	entries[6]=mat.entries[6];
	entries[7]=mat.entries[7];
	entries[8]=mat.entries[8];
	entries[9]=mat.entries[9];
	entries[10]=mat.entries[10];
	entries[11]=mat.entries[11];
	entries[12]=mat.entries[12];
	entries[13]=mat.entries[13];
	entries[14]=mat.entries[14];
	entries[15]=mat.entries[15];

	this->transpose();
}

FloatVector4d Matrix4d4d::column(int i){
	if(i==0){
		return FloatVector4d(entries[0],entries[4],entries[8],entries[12]);
	}else if(i==1){
		return FloatVector4d(entries[1],entries[5],entries[9],entries[13]);
	}else if(i==2){
		return FloatVector4d(entries[2],entries[6],entries[10],entries[14]);
	}else{
		return FloatVector4d(entries[3],entries[7],entries[11],entries[15]);
	}

}

void Matrix4d4d::rotateZ(float angle){
	float cosA = math.cosDegrees(angle), sinA = math.sinDegrees(angle);

	Matrix4d4d mat4(
		cosA, sinA, 0, 0,
		-sinA,  cosA, 0, 0,
		0,     0,    1, 0,
		0,     0,    0, 1);

	Matrix4d4d t=*this*mat4;
	entries[0]=t.entries[0];
	entries[1]=t.entries[1];
	entries[2]=t.entries[2];
	entries[3]=t.entries[3];
	entries[4]=t.entries[4];
	entries[5]=t.entries[5];
	entries[6]=t.entries[6];
	entries[7]=t.entries[7];
	entries[8]=t.entries[8];
	entries[9]=t.entries[9];
	entries[10]=t.entries[10];
	entries[11]=t.entries[11];
	entries[12]=t.entries[12];
	entries[13]=t.entries[13];
	entries[14]=t.entries[14];
	entries[15]=t.entries[15];
}


enum QuatPart {X, Y, Z, W};

#define EulParEven   0
#define EulRepNo     0
#define EulFrmS	     0

#define EulSafe	     "\000\001\002\000"
#define EulNext	     "\001\002\000\001"
#define EulRepYes    1
#define EulFrm(ord)  ((unsigned)(ord)&1)

#define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned o=ord;f=o&1;o>>1;s=o&1;o>>1;\
    n=o&1;o>>1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];h=s?k:i;}
    /* EulOrd creates an order value between 0 and 23 from 4-tuple choices. */

#define EulOrd(i,p,r,f)	   (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))
    /* Static axes */

#define EulOrdXYZs    EulOrd(X,EulParEven,EulRepNo,EulFrmS)
#define EulOrdZYXs    EulOrd(Z,EulParOdd,EulRepNo,EulFrmS)

#define EulParOdd    1

int rc(int i, int j){
	return j*4+i;
}

int rcR(int i, int j){
	return i*4+j;
}

#include <float.h>

//http://vered.rose.utoronto.ca/people/david_dir/GEMS/GEMS.html

FloatVector3d Matrix4d4d::getRotation(){

	int order=EulOrdZYXs;
	int i,j,k,h,n,s,f;

	FloatVector3d ea;

	EulGetOrd(order,i,j,k,h,n,s,f);

	if (s==EulRepYes) {
		double sy = sqrt(entries[rc(i,j)]*entries[rc(i,j)] + entries[rc(i,k)]*entries[rc(i,k)]);
		if (sy > 16*FLT_EPSILON) {
			ea.x = math.arctan2Degrees(entries[rc(i,j)], entries[rc(i,k)]);
			ea.y = math.arctan2Degrees(sy, entries[rc(i,i)]);
			ea.z = math.arctan2Degrees(entries[rc(j,i)], -entries[rc(k,i)]);
		} else {
			ea.x = math.arctan2Degrees(-entries[rc(j,k)], entries[rc(j,j)]);
			ea.y = math.arctan2Degrees(sy, entries[rc(i,i)]);
			ea.z = 0;
		}
    } else {
		double cy = sqrt(entries[rc(i,i)]*entries[rc(i,i)] + entries[rc(j,i)]*entries[rc(j,i)]);
		if (cy > 16*FLT_EPSILON) {
			ea.x = math.arctan2Degrees(entries[rc(k,j)], entries[rc(k,k)]);
			ea.y = math.arctan2Degrees(-entries[rc(k,i)], cy);
			ea.z = math.arctan2Degrees(entries[rc(j,i)], entries[rc(i,i)]);
		} else {
			ea.x = math.arctan2Degrees(-entries[rc(j,k)], entries[rc(j,j)]);
			ea.y = math.arctan2Degrees(-entries[rc(k,i)], cy);
			ea.z = 0;
		}
    }
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}

	return ea;
}


FloatVector3d Matrix4d4d::getRotationReverse(){

	int order=EulOrdXYZs;
	int i,j,k,h,n,s,f;

	FloatVector3d ea;

	EulGetOrd(order,i,j,k,h,n,s,f);

	if (s==EulRepYes) {
		double sy = sqrt(entries[rcR(i,j)]*entries[rcR(i,j)] + entries[rcR(i,k)]*entries[rcR(i,k)]);
		if (sy > 16*FLT_EPSILON) {
			ea.x = math.arctan2Degrees(entries[rcR(i,j)], entries[rcR(i,k)]);
			ea.y = math.arctan2Degrees(sy, entries[rcR(i,i)]);
			ea.z = math.arctan2Degrees(entries[rcR(j,i)], -entries[rcR(k,i)]);
		} else {
			ea.x = math.arctan2Degrees(-entries[rcR(j,k)], entries[rcR(j,j)]);
			ea.y = math.arctan2Degrees(sy, entries[rcR(i,i)]);
			ea.z = 0;
		}
    } else {
		double cy = sqrt(entries[rcR(i,i)]*entries[rcR(i,i)] + entries[rcR(j,i)]*entries[rcR(j,i)]);
		if (cy > 16*FLT_EPSILON) {
			ea.x = math.arctan2Degrees(entries[rcR(k,j)], entries[rcR(k,k)]);
			ea.y = math.arctan2Degrees(-entries[rcR(k,i)], cy);
			ea.z = math.arctan2Degrees(entries[rcR(j,i)], entries[rcR(i,i)]);
		} else {
			ea.x = math.arctan2Degrees(-entries[rcR(j,k)], entries[rcR(j,j)]);
			ea.y = math.arctan2Degrees(-entries[rcR(k,i)], cy);
			ea.z = 0;
		}
    }
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}

	ea=-ea;

	return ea;
}

float Ray2d::length(){
	float x=(end.x-start.x);
	float y=(end.y-start.y);

	return sqrt(x*x+y*y);
}

FloatVector2d Ray2d::posAlong(float percent){
	FloatVector2d temp;
	temp=end-start;
	temp*=percent;
	return temp+start;
}

Box3d::Box3d(){
	nx=0.0f;
	px=0.0f;

	ny=0.0f;
	py=0.0f;

	nz=0.0f;
	pz=0.0f;

	xSet=false;
	ySet=false;
	zSet=false;
}


Box2d::Box2d(){
	nx=0;
	px=0;

	ny=0;
	py=0;

}

DoubleVector3d::DoubleVector3d(){
	x=0;
	y=0;
	z=0;
}

float Box3d::closestDistance(FloatVector3d in){	//find the distance from in to the point that is closest to it
	float d[8];
	d[0]=math.abs(FloatVector3d(FloatVector3d(px,py,nz)-in).magnitude());
	d[1]=math.abs(FloatVector3d(FloatVector3d(nx,py,nz)-in).magnitude());
	d[2]=math.abs(FloatVector3d(FloatVector3d(nx,py,pz)-in).magnitude());
	d[3]=math.abs(FloatVector3d(FloatVector3d(px,py,pz)-in).magnitude());
	d[4]=math.abs(FloatVector3d(FloatVector3d(px,ny,nz)-in).magnitude());
	d[5]=math.abs(FloatVector3d(FloatVector3d(nx,ny,nz)-in).magnitude());
	d[6]=math.abs(FloatVector3d(FloatVector3d(nx,ny,pz)-in).magnitude());
	d[7]=math.abs(FloatVector3d(FloatVector3d(px,ny,pz)-in).magnitude());

	float smallest=d[0];

	for(int i=1; i<8; i++){
		if(d[i]<smallest){smallest=d[i];}
	}

	return smallest;
}

float Box3d::closestDistance(Plane3d in){	//find the distance from in to the point that is closest to it
	float d[8];
	d[0]=math.distance(in,FloatVector3d(px,py,nz));
	d[1]=math.distance(in,FloatVector3d(nx,py,nz));
	d[2]=math.distance(in,FloatVector3d(nx,py,pz));
	d[3]=math.distance(in,FloatVector3d(px,py,pz));
	d[4]=math.distance(in,FloatVector3d(px,ny,nz));
	d[5]=math.distance(in,FloatVector3d(nx,ny,nz));
	d[6]=math.distance(in,FloatVector3d(nx,ny,pz));
	d[7]=math.distance(in,FloatVector3d(px,ny,pz));

	float smallest=d[0];

	for(int i=1; i<8; i++){
		if(d[i]>0){
			if(d[i]<smallest){smallest=d[i];}
		}
	}

	return smallest;
}
float Box3d::closestDistanceSigned(Plane3d in){	//find the distance from in to the point that is closest to it
	float d[8];
	d[0]=math.distanceSigned(in,FloatVector3d(px,py,nz));
	d[1]=math.distanceSigned(in,FloatVector3d(nx,py,nz));
	d[2]=math.distanceSigned(in,FloatVector3d(nx,py,pz));
	d[3]=math.distanceSigned(in,FloatVector3d(px,py,pz));
	d[4]=math.distanceSigned(in,FloatVector3d(px,ny,nz));
	d[5]=math.distanceSigned(in,FloatVector3d(nx,ny,nz));
	d[6]=math.distanceSigned(in,FloatVector3d(nx,ny,pz));
	d[7]=math.distanceSigned(in,FloatVector3d(px,ny,pz));

	float smallest=d[0];

	for(int i=1; i<8; i++){
		if(d[i]>0){
			if(d[i]<smallest || smallest<0){smallest=d[i];}
		}
	}

	return smallest;
}


float Box3d::averageDistance(FloatVector3d in){	//find the distance from in to the point that is closest to it
	float d[8];
	d[0]=math.abs(FloatVector3d(FloatVector3d(px,py,nz)-in).magnitude());
	d[1]=math.abs(FloatVector3d(FloatVector3d(nx,py,nz)-in).magnitude());
	d[2]=math.abs(FloatVector3d(FloatVector3d(nx,py,pz)-in).magnitude());
	d[3]=math.abs(FloatVector3d(FloatVector3d(px,py,pz)-in).magnitude());
	d[4]=math.abs(FloatVector3d(FloatVector3d(px,ny,nz)-in).magnitude());
	d[5]=math.abs(FloatVector3d(FloatVector3d(nx,ny,nz)-in).magnitude());
	d[6]=math.abs(FloatVector3d(FloatVector3d(nx,ny,pz)-in).magnitude());
	d[7]=math.abs(FloatVector3d(FloatVector3d(px,ny,pz)-in).magnitude());

	
	return (d[0]+d[1]+d[2]+d[3]+d[4]+d[5]+d[6]+d[7])/float(8);
}

bool Box3d::inside(FloatVector3d p){

	if(p.x>px){return false;}
	if(p.x<nx){return false;}

	if(p.y>py){return false;}
	if(p.y<ny){return false;}

	if(p.z>pz){return false;}
	if(p.z<nz){return false;}

	return true;
}

bool castRay(FloatVector3d& normal, FloatVector3d& point,FloatVector3d tp1, FloatVector3d tp2, FloatVector3d tp3){

	FloatVector3d pnormal=math.crossProduct((tp1-tp2).normalized(),(tp3-tp2).normalized()).normalized();

	float d=intersectRayPlane(point,normal,tp1,pnormal);

	if(d>0){
		FloatVector3d ipoint=point+normal*d;
		if(CheckPointInTriangle(ipoint,tp1,tp2,tp3)){
			return true;
		}
	}

	return false;
}

bool Box3d::intersect(FloatVector3d p,FloatVector3d n){

	if(p.x>nx && p.x<px &&
		p.y>ny && p.y<py &&
		p.z>nz && p.z<pz){

		return true;
	}

	if(castRay(n,p,FloatVector3d(nx,ny,nz),FloatVector3d(px,ny,nz),FloatVector3d(px,py,nz))){return true;}
	if(castRay(n,p,FloatVector3d(nx,ny,nz),FloatVector3d(px,py,nz),FloatVector3d(nx,py,nz))){return true;}

	if(castRay(n,p,FloatVector3d(px,ny,nz),FloatVector3d(px,ny,pz),FloatVector3d(px,py,pz))){return true;}
	if(castRay(n,p,FloatVector3d(px,ny,nz),FloatVector3d(px,py,pz),FloatVector3d(px,py,nz))){return true;}

	if(castRay(n,p,FloatVector3d(nx,py,nz),FloatVector3d(px,py,nz),FloatVector3d(px,py,pz))){return true;}
	if(castRay(n,p,FloatVector3d(nx,py,nz),FloatVector3d(px,py,pz),FloatVector3d(nx,py,pz))){return true;}

	if(castRay(n,p,FloatVector3d(nx,ny,nz),FloatVector3d(px,ny,nz),FloatVector3d(px,ny,pz))){return true;}
	if(castRay(n,p,FloatVector3d(nx,ny,nz),FloatVector3d(px,ny,pz),FloatVector3d(nx,ny,pz))){return true;}

	if(castRay(n,p,FloatVector3d(nx,ny,pz),FloatVector3d(px,ny,pz),FloatVector3d(px,py,pz))){return true;}
	if(castRay(n,p,FloatVector3d(nx,ny,pz),FloatVector3d(px,py,pz),FloatVector3d(nx,py,pz))){return true;}

	if(castRay(n,p,FloatVector3d(nx,ny,nz),FloatVector3d(nx,ny,pz),FloatVector3d(nx,py,pz))){return true;}
	if(castRay(n,p,FloatVector3d(nx,ny,nz),FloatVector3d(nx,py,pz),FloatVector3d(nx,py,nz))){return true;}

	return false;
}

void operator+=(Box3d& left, const Box3d& right){
	if(left.px<right.px){left.px=right.px;}
	if(left.py<right.py){left.py=right.py;}
	if(left.pz<right.pz){left.pz=right.pz;}

	if(left.nx>right.nx){left.nx=right.nx;}
	if(left.ny>right.ny){left.ny=right.ny;}
	if(left.nz>right.nz){left.nz=right.nz;}
}

void Box3d::operator+=(const FloatVector4d& right){
	if(px<right.x || !xSet){px=right.x;xSet=true;}
	if(py<right.y || !ySet){py=right.y;ySet=true;}
	if(pz<right.z || !zSet){pz=right.z;zSet=true;}

	if(nx>right.x || !xSet){nx=right.x;xSet=true;}
	if(ny>right.y || !ySet){ny=right.y;ySet=true;}
	if(nz>right.z || !zSet){nz=right.z;zSet=true;}
}

FloatVector4d& FloatVector4d::operator /= (const float& t){
	x /= t;
	y /= t;
	z /= t;
	w /= t;
	return (*this);
}
