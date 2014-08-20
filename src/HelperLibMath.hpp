#ifndef MATH_H
#define MATH_H

#include "../Types.hpp"

#ifdef _WIN32
	#include <math.h>	//std math
#endif

#define PI 3.14159265

class Math{
    private:
    
    public:
		const float pi;

	float abs(float);
	float pow(float,float power);
	float floor(float);
	float ceil(float);
	float sqrt(float);

	float maximum(float a,float b);
	float minimum(float a,float b);

	float cos(float);	//degrees
	float sin(float);	//degrees

	float cosDegrees(float);
	float sinDegrees(float);
	float tanDegrees(float);

	float cosRadians(float);
	float sinRadians(float);

	float arcsinDegrees(float);
	float arccosDegrees(float);
	float arctanDegrees(float);
	float arctan2Degrees(float,float);

	float arcsinRadians(float);
	float arccosRadians(float);
	float arctanRadians(float);
	float arctan2Radians(float,float);

	float ln(float);

	float area(FloatVector3d v1,FloatVector3d v2, FloatVector3d v3);
    float dotProduct(FloatVector2d v1, FloatVector2d v2);
    float dotProduct(FloatVector3d v1, FloatVector3d v2);
	float dotProduct(FloatVector4d v1, FloatVector4d v2);
    float vectorToAngle(FloatVector2d v1, FloatVector2d v2);
	float vectorToAngleDegrees(FloatVector2d v1);	//with (0,0) as the center pivot where the angle is calculated from
	FloatVector3d rayToAngle(FloatVector3d start, FloatVector3d end);
	float rayToAngle(FloatVector2d start, FloatVector2d end);

    FloatVector2d rotToNormal(float i);  //takes a rot stored in a vector and turns it into a normal
    FloatVector3d degreesToNormal(FloatVector3d angle);	//turns an angle into a normalized vector - works so that if you use this the results are right for opengl world coords
    float magnitude(FloatVector2d input);
    float magnitude(FloatVector3d input);
    float magnitude(GLubyteVector3d input);
    
    float distance(POS_3D one,  POS_3D two);
    float distance(POS_3D one,  float x, float y, float z);
    float distance(float x1, float y1, float z1, float x2, float y2, float z2);
    float distance(Plane3d plane,FloatVector3d point);
	float distanceSigned(Plane3d plane,FloatVector3d point);

    float distance(POS_2D one,  POS_2D two);
    float distance(float x1, float y1, float x2, float y2);
    float length(FloatVector2d);
    
    FloatVector3d crossProduct(FloatVector3d v1,FloatVector3d v2);
    FloatVector2d closestPointOnALine(FloatVector2d linePoint1,FloatVector2d linePoint2,FloatVector2d searchPoint);
	FloatVector2d lineIntersection(FloatVector2d lineAPoint1,FloatVector2d lineAPoint2,FloatVector2d lineBPoint1,FloatVector2d lineBPoint2);
    FloatVector3d normalize(FloatVector3d input);
    FloatVector3d normalize(int max, FloatVector3d input);
    FloatVector3d normalize(float max, FloatVector3d input);
    GLubyteVector3d normalize(GLubyteVector3d input);
    GLubyteVector3d normalize(int max, GLubyteVector3d input);
    
    FloatVector3d toNormal(FloatVector3d p1, FloatVector3d p2, FloatVector3d p3);
	FloatVector3d toNormal(FloatVector3d p1, FloatVector3d p2);
	FloatVector2d toNormal(FloatVector2d p1, FloatVector2d p2);
    bool point_in_rect();
    Color phong();

	float clampAngleDegrees(float);

	bool intersectRayTriangle(float* dist, float* b1, float* b2, FloatVector3d* o, FloatVector3d* d, FloatVector3d* p0, FloatVector3d* p1, FloatVector3d* p2){
		FloatVector3d e1=*p1-*p0;
		FloatVector3d e2=*p2-*p0;
		FloatVector3d s=*o-*p0;

		FloatVector3d s1=crossProduct(*d,e2);
		FloatVector3d s2=crossProduct(s,e1);

		float rcp=1/dotProduct(s1,e1);

		*dist=rcp*dotProduct(s2,e2);
		*b1=rcp*dotProduct(s1,s);
		*b2=rcp*dotProduct(s2,*d);

		if(*b1>=0 && *b2>=0 && *b1+*b2<=1){
			return true;
		}

		return false;
	}

	Math();
};

extern Math math;

#endif
