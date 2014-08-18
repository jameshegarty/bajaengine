#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <iostream>
#include <fstream>

#include <vector>

#ifdef _WIN32
	#include <windows.h>
	#include <stdlib.h>
	#include <math.h>
#else
	#include <math.h>
#endif

class Color{
	public:
	float r;
	float g;
	float b;
	float a;
	
	Color();
};


class GLubyteVector3d{
	public:
		unsigned char x;
		unsigned char y;
		unsigned char z;
};

////////////////////////////////////////////////////////////////////////////////

class FloatVector2d{
	public:
		float x;
		float y;
  		
  		bool operator!=(const FloatVector2d& right) {
			if(x!=right.x){return true;}
			if(y!=right.y){return true;}
			return false;
		}

		FloatVector2d normalized();	//return normalized, DON'T SAVE

		void operator*=(const FloatVector2d s){
			x*=s.x;
			y*=s.y;
		}

		void operator*=(const float s){
			x*=s;
			y*=s;
		}

		FloatVector2d& operator /=(float t);
		FloatVector2d& operator /=(FloatVector2d& t);


		void operator+=(const FloatVector2d s){
			x+=s.x;
			y+=s.y;
		}

		int operator<(const FloatVector2d& in){
			return (x<in.x);
		}

		float magnitude();

		void normalize();

  		FloatVector2d(float a, float b);
  		FloatVector2d();

};

int operator==(const FloatVector2d& left, const FloatVector2d& right);
FloatVector2d operator-(const FloatVector2d& left, const FloatVector2d& right);
FloatVector2d operator+(const FloatVector2d& left, const FloatVector2d& right);
FloatVector2d operator*(const FloatVector2d& left, const float& right);
FloatVector2d operator*(const float& left, const FloatVector2d& right );

////////////////////////////////////////////////////////////////////////////////

class FloatVector3d{

	
	public:
		float x;
		float y;
		float z;
		
  		
  		operator FloatVector2d() const { 
			FloatVector2d out;
     		out.x=x;
     		out.y=y;
       		return out;
		}


		FloatVector3d& normalize();	//save normalized
		FloatVector3d normalized();	//return normalized, DON'T SAVE
		float magnitude();

		FloatVector3d& operator /=(float t);
		FloatVector3d& operator /=(FloatVector3d& t);

		FloatVector3d operator %(const FloatVector3d& v) const;

  		FloatVector3d& operator=(const int& right) {
			x=float(right);
			y=float(right);
			z=float(right);
			return *this;
		}

		bool operator!=(const FloatVector3d& right) {

			if(x!=right.x){return true;}
			if(y!=right.y){return true;}
			if(z!=right.z){return true;}
			return false;
		}


  		void operator*=(const float s);
		FloatVector3d& operator*=(FloatVector3d& s);
		void operator-=(const FloatVector3d& left);

  		FloatVector3d();
  		FloatVector3d(float a, float b, float c);
  		FloatVector3d(FloatVector2d t){
  			x=t.x;
  			y=t.y;
  			z=0;
  		}

};



FloatVector3d operator+(const FloatVector3d& left, const FloatVector3d& right);
FloatVector3d operator+(const FloatVector3d& left, const float& right);
FloatVector3d operator-(const FloatVector3d& left, const FloatVector3d& right);
FloatVector3d operator-(const FloatVector3d& input);
FloatVector3d operator*(const FloatVector3d& left, const FloatVector3d& right);
FloatVector3d operator*(const FloatVector3d& left, const float& right);
FloatVector3d operator*(const FloatVector3d& left, const double& right);
FloatVector3d operator/(const FloatVector3d& left, const float& right);
FloatVector3d operator/(const FloatVector3d& left, const FloatVector3d& right);
FloatVector3d operator*(const float& right, const FloatVector3d& left);
FloatVector3d operator*(const double& right, const FloatVector3d& left);

void operator+=(FloatVector3d& left, const FloatVector3d& right);
int operator==(const FloatVector3d& left, const FloatVector3d& right);
int operator>(const FloatVector3d& left, const int right);
int operator<(const FloatVector3d& left, const FloatVector3d& right);
int operator<(const FloatVector3d& left, const float& right);
int operator!=(const FloatVector3d& left, const float& right);
	

///////////////////////////////////////////////////////////////////////////////////////////

class DoubleVector3d{

	
	public:
		double x;
		double y;
		double z;
  		
  		operator FloatVector2d() const { 
			FloatVector2d out;
     		out.x=(float)x;
     		out.y=(float)y;
       		return out;
		}
		
		operator FloatVector3d() const { 
			FloatVector3d out;
     		out.x=(float)x;
     		out.y=(float)y;
			out.z=(float)z;
       		return out;
		}

		DoubleVector3d& normalize();	//save normalized
		DoubleVector3d normalized();	//return normalized, DON'T SAVE
		double magnitude();

		DoubleVector3d& operator /=(float t);
		DoubleVector3d& operator /=(FloatVector3d& t);

		DoubleVector3d operator %(const FloatVector3d& v) const;

  		DoubleVector3d& operator=(const int& right) {
			x=double(right);
			y=double(right);
			z=double(right);
			return *this;
		}

		bool operator!=(const DoubleVector3d& right) {
			if(x!=right.x){return true;}
			if(y!=right.y){return true;}
			if(z!=right.z){return true;}
			return false;
		}

  		void operator*=(const float s);
		DoubleVector3d& operator*=(FloatVector3d& s);
		void operator-=(const FloatVector3d& left);

  		DoubleVector3d();
  		DoubleVector3d(float a, float b, float c);
  		DoubleVector3d(FloatVector2d t){
  			x=t.x;
  			y=t.y;
  			z=0;
  		}

		DoubleVector3d(FloatVector3d t){
  			x=t.x;
  			y=t.y;
  			z=t.z;
  		}

};

DoubleVector3d operator+(const DoubleVector3d& left, const DoubleVector3d& right);
DoubleVector3d operator+(const DoubleVector3d& left, const FloatVector3d& right);

DoubleVector3d operator-(const DoubleVector3d& left, const DoubleVector3d& right);
DoubleVector3d operator-(const DoubleVector3d& left, const FloatVector3d& right);
DoubleVector3d operator-(const FloatVector3d& left, const DoubleVector3d& right);
DoubleVector3d operator-(const DoubleVector3d& input);

void operator+=(DoubleVector3d& left, const FloatVector3d& right);

////////////////////////////////////////////////////////////////////////////////////////////
class FloatVector4d{
	private:
		float entries[4];

	public:
		float x;
		float y;
		float z;
		float w;
		
		FloatVector4d(){
			x=0;
			y=0;
			z=0;
			w=0;
		}
		
		FloatVector4d(float a, float b, float c, float d){
			x=a;
			y=b;
			z=c;
			w=d;
		}

		FloatVector4d(const float& t){
  			x=t;
  			y=t;
  			z=t;
			w=t;
  		}

		bool operator!=(const FloatVector4d& right) {
			if(x!=right.x){return true;}
			if(y!=right.y){return true;}
			if(z!=right.z){return true;}
			if(w!=right.w){return true;}
			return false;
		}

		operator float* ()  {
			entries[0]=x;
			entries[1]=y;
			entries[2]=z;
			entries[3]=w;
			return (float*)entries;
		}
		
		float magnitude(){
			return sqrt(x*x+y*y+z*z+w*w);
		}

		FloatVector4d& operator /=(const float& t);

};


FloatVector4d operator*(const FloatVector4d& left, const double& right);
FloatVector4d operator*(const FloatVector4d& left, const float& right);
FloatVector4d operator*( const double& right,const FloatVector4d& left);
void operator+=(FloatVector4d& left, const FloatVector4d& right);
void operator-=(FloatVector4d& left, const FloatVector4d& right);
void operator-=(FloatVector4d& left, const DoubleVector3d& right);

FloatVector4d operator+(const FloatVector4d& left,const FloatVector4d& right);

/////////////////////////////////////////////////////////////////////////////////////////////
class DoubleVector4d{
	private:
		double entries[4];

	public:
		double x;
		double y;
		double z;
		double w;
		
		DoubleVector4d(){
			x=0;
			y=0;
			z=0;
			w=0;
		}
		
		DoubleVector4d(FloatVector4d t){
  			x=t.x;
  			y=t.y;
  			z=t.z;
			w=t.w;
  		}


		DoubleVector4d(float a, float b, float c, float d){
			x=a;
			y=b;
			z=c;
			w=d;
		}

		DoubleVector4d(const float& t){
  			x=t;
  			y=t;
  			z=t;
			w=t;
  		}

		bool operator!=(const FloatVector4d& right) {
			if(x!=right.x){return true;}
			if(y!=right.y){return true;}
			if(z!=right.z){return true;}
			if(w!=right.w){return true;}
			return false;
		}

		operator float* ()  {
			
			entries[0]=x;
			entries[1]=y;
			entries[2]=z;
			entries[3]=w;
			return (float*)entries;
		}

};

////////////////////////////////////////////////////////////////////////////////////////////
class Matrix4d4d{
	private:
	
	public:
		/*in stanford form:
		[0	4	8	12] [x]
		[1	5	9	13]	[y]
		[2	6	10	14]	[z]
		[3	7	11	15]	[w]
		*/

		float entries[16];

		FloatVector4d row(int i);
		FloatVector4d column(int i);	//i=0 for first column

		Matrix4d4d(float v00, float v01, float v02, float v03,
			float v10, float v11, float v12, float v13,
			float v20, float v21, float v22, float v23,
			float v30, float v31, float v32, float v33);

		Matrix4d4d();

		Matrix4d4d operator+(const Matrix4d4d & rhs) const;
		Matrix4d4d operator-(const Matrix4d4d & rhs) const;
		Matrix4d4d operator*(const Matrix4d4d & rhs) const;
		FloatVector4d operator*(const FloatVector4d & rhs) const;
		Matrix4d4d operator*(const float rhs) const;
		
		void loadIdentity();
		void rotateX(float angle);
		void rotateY(float angle);
		void rotateZ(float angle);
		void scale(float x, float y, float z);
		void translate(float x,float y,float z);
		void translateRaw(float x,float y,float z);

		FloatVector3d getRotation();
		FloatVector3d getRotationReverse();

		void transpose();
		void invert();

		operator float* () const {return (float*)entries;}
		operator const float* () const {return (float*)entries;}

};


class Ray3d{
	public:
		FloatVector3d start;
		FloatVector3d end;
};


int operator==(const Ray3d& left, const Ray3d& right);
class Ray2d{
	public:
		float length();

		FloatVector2d start;
		FloatVector2d end;
		FloatVector2d posAlong(float percent);	//find a pos along the ray, "percent" is 0
};

class Plane3d{
	public:
		FloatVector3d normal;
		float x;	//an x value on the plane
		float y;	//an y value on the plane
		float z;	//an z value on the plane
};


class Box3d{

	private:
		bool xSet;
		bool ySet;
		bool zSet;

	public:
		float nx;	//negative x (positive NUMBER, but represends stuff on the negative part of the axis)
		float px;	//positive x

		float ny;
		float py;

		float nz;
		float pz;

		bool inside(FloatVector3d);

		void operator+=(const FloatVector4d& right);

		FloatVector3d center(){
			FloatVector3d t;
			t.x=(px+nx)/float(2);
			t.y=(py+ny)/float(2);
			t.z=(pz+nz)/float(2);
			return t;
		}

		float magnitude(){	//returns 
			float s1=px-nx;
			float s2=py-ny;
			float s3=pz-nz;
			return FloatVector3d(s1,s2,s3).magnitude();
		}

		float closestDistance(FloatVector3d in);	//find the distance from in to the point that is closest to it
		

		float closestDistance(Plane3d in);	//find the distance from in to the point that is closest to it
		float closestDistanceSigned(Plane3d in);


		float averageDistance(FloatVector3d in);	//find the distance from in to the point that is closest to it

		bool intersect(FloatVector3d point, FloatVector3d normal);

		Box3d();
};

void operator+=(Box3d& left, const Box3d& right);


class Box2d{

	
	public:
		float nx;	//negative x
		float px;	//positive x

		float ny;
		float py;

		Box2d();
};

class UnsignedIntVector2d{
    public:
        unsigned int x;
        unsigned int y;
        
        UnsignedIntVector2d(){
        	x=0;
        	y=0;
        }
};

class IntVector2d{
    public:
        int x;
        int y;

		IntVector2d(){
			x=0;
			y=0;
		}

		IntVector2d(int xx, int yy){
			x=xx;
			y=yy;
		}
};

class IntVector3d{
    public:
        int x;
        int y;
		int z;
};

int operator<(const IntVector3d& left, const IntVector3d& right);

typedef struct{
	float x;
	float y;
	float z;
} POS_3D;

typedef struct{
	float x;
	float y;
} POS_2D;


typedef struct{  //defines a 2d space
	float x;
	float y;
	float sizeX;
	float sizeY;
	
} Space2D;

typedef Space2D SPACE_2D;

bool operator != (SPACE_2D& in, SPACE_2D& out);

bool operator != (Color& in, Color& out);

///////////////////////////////////////////////////////////
class Object;

enum CommonType{
	COMMONTYPE_FLOATVECTOR3D,
	COMMONTYPE_FLOATVECTOR2D,
	COMMONTYPE_FLOAT,
	COMMONTYPE_INT
};

#endif
