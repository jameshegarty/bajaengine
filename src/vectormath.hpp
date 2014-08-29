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
inline double dot(const DoubleVector3d& v1, const DoubleVector3d& v2) {
 return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z );	
}

inline void normalizeVector(DoubleVector3d& v) {
 double len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);	
 v.x /= len;
 v.y /= len;
 v.z /= len;
}

inline double lengthOfVector(DoubleVector3d v) {
 return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

inline void setLength(DoubleVector3d& v, double l) {
 double len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);	
 v.x *= l/len;
 v.y *= l/len;
 v.z *= l/len;
} 

inline bool isZeroVector(DoubleVector3d& v) {
 if ((v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f))
   return true;
   
  return false;	
}

inline DoubleVector3d wedge(DoubleVector3d v1, DoubleVector3d v2) {
 DoubleVector3d result;
 
 result.x = (v1.y * v2.z) - (v2.y * v1.z);
 result.y = (v1.z * v2.x) - (v2.z * v1.x);
 result.z = (v1.x * v2.y) - (v2.x * v1.y); 	
 
 return (result);	
}



// ray intersections. All return -1.0 if no intersection, otherwise the distance along the 
// ray where the (first) intersection takes place
double      intersectRayPlane(DoubleVector3d rOrigin, DoubleVector3d rVector, DoubleVector3d pOrigin, DoubleVector3d pNormal); 
double      intersectRaySphere(DoubleVector3d rO, DoubleVector3d rV, DoubleVector3d sO, double sR);

double plane_dist(DoubleVector3d planeOrigin, DoubleVector3d planeNormal, DoubleVector3d point);

// Distance to line of triangle
DoubleVector3d  closestPointOnLine(DoubleVector3d& a, DoubleVector3d& b, DoubleVector3d& p);
DoubleVector3d  closestPointOnTriangle(DoubleVector3d a, DoubleVector3d b, DoubleVector3d c, DoubleVector3d p);

// point inclusion
bool CheckPointInTriangle(DoubleVector3d point ,DoubleVector3d a, DoubleVector3d b, DoubleVector3d c);
bool CheckPointInSphere(DoubleVector3d point, DoubleVector3d sO, double sR);

// Normal generation
DoubleVector3d tangentPlaneNormalOfEllipsoid(DoubleVector3d point,DoubleVector3d eO, DoubleVector3d eR);

// Point classification
int classifyPoint(DoubleVector3d point, DoubleVector3d pO, DoubleVector3d pN);


#endif // VECTOR_MATH_H
