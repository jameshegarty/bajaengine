//-----------------------------------------------------------------------------
// File: Vectormath.h
//
// Desc: A collection of vector math related functions.
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------
#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H
//#include <d3d.h>
#include <math.h>
#include "Types.hpp"

#define PLANE_BACKSIDE 0x000001
#define PLANE_FRONT    0x000010
#define ON_PLANE       0x000100


// basic vector operations (inlined)
inline float dot(FloatVector3d& v1, FloatVector3d& v2) {
 return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z );	
}

inline void normalizeVector(FloatVector3d& v) {
 float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);	
 v.x /= len;
 v.y /= len;
 v.z /= len;
}

inline double lengthOfVector(FloatVector3d v) {
 return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

inline void setLength(FloatVector3d& v, float l) {
 float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);	
 v.x *= l/len;
 v.y *= l/len;
 v.z *= l/len;
} 

inline bool isZeroVector(FloatVector3d& v) {
 if ((v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f))
   return true;
   
  return false;	
}

inline FloatVector3d wedge(FloatVector3d v1, FloatVector3d v2) {
 FloatVector3d result;
 
 result.x = (v1.y * v2.z) - (v2.y * v1.z);
 result.y = (v1.z * v2.x) - (v2.z * v1.x);
 result.z = (v1.x * v2.y) - (v2.x * v1.y); 	
 
 return (result);	
}



// ray intersections. All return -1.0 if no intersection, otherwise the distance along the 
// ray where the (first) intersection takes place
double      intersectRayPlane(FloatVector3d rOrigin, FloatVector3d rVector, FloatVector3d pOrigin, FloatVector3d pNormal); 
double      intersectRaySphere(FloatVector3d rO, FloatVector3d rV, FloatVector3d sO, double sR);


// Distance to line of triangle
FloatVector3d  closestPointOnLine(FloatVector3d& a, FloatVector3d& b, FloatVector3d& p);
FloatVector3d  closestPointOnTriangle(FloatVector3d a, FloatVector3d b, FloatVector3d c, FloatVector3d p);

// point inclusion
bool CheckPointInTriangle(FloatVector3d point ,FloatVector3d a, FloatVector3d b, FloatVector3d c);
bool CheckPointInSphere(FloatVector3d point, FloatVector3d sO, double sR);

// Normal generation
FloatVector3d tangentPlaneNormalOfEllipsoid(FloatVector3d point,FloatVector3d eO, FloatVector3d eR);

// Point classification
int classifyPoint(FloatVector3d point, FloatVector3d pO, FloatVector3d pN);


#endif // VECTOR_MATH_H
