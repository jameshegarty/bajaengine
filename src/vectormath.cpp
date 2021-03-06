//-----------------------------------------------------------------------------
// File: Vectormath.cpp
//
// Desc: A collection of vector math related functions.
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------

//#define D3D_OVERLOADS
//#define STRICT
#include <math.h>
#include <stdio.h>
#include "vectormath.hpp"

#define PI 3.14159265



// ----------------------------------------------------------------------
// Name  : intersectRayPlane()
// Input : rOrigin - origin of ray in world space
//         rVector - vector describing direction of ray in world space
//         pOrigin - Origin of plane 
//         pNormal - Normal to plane
// Notes : Normalized directional vectors expected
// Return: distance to plane in world units, -1 if no intersection.
// -----------------------------------------------------------------------  
double intersectRayPlane(DoubleVector3d rOrigin, DoubleVector3d rVector, DoubleVector3d pOrigin, DoubleVector3d pNormal) {
  
  double d = - (dot(pNormal,pOrigin));
 
  double numer = dot(pNormal,rOrigin) + d;
  double denom = dot(pNormal,rVector);
  
  if (denom == 0){  // normal is orthogonal to vector, cant intersect
   return (-1.0f);
  }

  return -(numer / denom);	
}

double plane_dist(DoubleVector3d planeOrigin, DoubleVector3d planeNormal, DoubleVector3d point){
  //double nom = dot(planeNormal, point);
  //double denom = sqrt(planeNormal.x*planeNormal.x+planeNormal.y*planeNormal.y+planeNormal.z*planeNormal.z);
  //return nom/denom;
  //  return intersectRayPlane(point, -planeNormal, planeOrigin, planeNormal);
  //return dot(planeNormal, point)-planeOrigin;

  double    sb, sn, sd;

  sn = -dot( planeNormal, (point - planeOrigin));
  sd = dot(planeNormal, planeNormal);
  sb = sn / sd;

  DoubleVector3d B = point + sb * planeNormal;
  return (point-B).magnitude();
}

// ----------------------------------------------------------------------
// Name  : intersectRaySphere()
// Input : rO - origin of ray in world space
//         rV - vector describing direction of ray in world space
//         sO - Origin of sphere 
//         sR - radius of sphere
// Notes : Normalized directional vectors expected
// Return: distance to sphere in world units, -1 if no intersection.
// -----------------------------------------------------------------------  

double intersectRaySphere(DoubleVector3d rO, DoubleVector3d rV, DoubleVector3d sO, double sR) {
	
   DoubleVector3d Q = sO-rO;
   
   double c = lengthOfVector(Q);
   double v = dot(Q,rV);
   double d = sR*sR - (c*c - v*v);

   // If there was no intersection, return -1
   if (d < 0.0) return (-1.0f);

   // Return the distance to the [first] intersecting point
   return (v - sqrt(d));
}




// ----------------------------------------------------------------------
// Name  : CheckPointInTriangle()
// Input : point - point we wish to check for inclusion
//         a - first vertex in triangle
//         b - second vertex in triangle 
//         c - third vertex in triangle
// Notes : Triangle should be defined in clockwise order a,b,c
// Return: TRUE if point is in triangle, FALSE if not.
// -----------------------------------------------------------------------  

bool CheckPointInTriangle(DoubleVector3d point, DoubleVector3d a, DoubleVector3d b, DoubleVector3d c) {
  
  double total_angles = 0.0f;
       
  // make the 3 vectors
  DoubleVector3d v1 = point-a;
  DoubleVector3d v2 = point-b;
  DoubleVector3d v3 = point-c;
  
  normalizeVector(v1);
  normalizeVector(v2);
  normalizeVector(v3);

  total_angles += acos(dot(v1,v2));   
  total_angles += acos(dot(v2,v3));
  total_angles += acos(dot(v3,v1)); 
     
  if (fabs(total_angles-2*PI) <= 0.005)
   return (true);
     
  return(false);
}



// ----------------------------------------------------------------------
// Name  : closestPointOnLine()
// Input : a - first end of line segment
//         b - second end of line segment
//         p - point we wish to find closest point on line from 
// Notes : Helper function for closestPointOnTriangle()
// Return: closest point on line segment
// -----------------------------------------------------------------------  

DoubleVector3d closestPointOnLine(DoubleVector3d& a, DoubleVector3d& b, DoubleVector3d& p) {
	
   // Determine t (the length of the vector from �a� to �p�)
   DoubleVector3d c = p-a;
   DoubleVector3d V = b-a; 
      
   double d = lengthOfVector(V);
      
   normalizeVector(V);  
   double t = dot(V,c);

   
   // Check to see if �t� is beyond the extents of the line segment
   if (t < 0.0f) return (a);
   if (t > d) return (b);
 
  
   // Return the point between �a� and �b�
   //set length of V to t. V is normalized so this is easy
   V.x = V.x * t;
   V.y = V.y * t;
   V.z = V.z * t;
           
   return (a+V);	
}




// ----------------------------------------------------------------------
// Name  : closestPointOnTriangle()
// Input : a - first vertex in triangle
//         b - second vertex in triangle 
//         c - third vertex in triangle
//         p - point we wish to find closest point on triangle from 
// Notes : 
// Return: closest point on line triangle edge
// -----------------------------------------------------------------------  

DoubleVector3d closestPointOnTriangle(DoubleVector3d a, DoubleVector3d b, DoubleVector3d c, DoubleVector3d p) {
	
  DoubleVector3d Rab = closestPointOnLine(a, b, p);
  DoubleVector3d Rbc = closestPointOnLine(b, c, p);
  DoubleVector3d Rca = closestPointOnLine(c, a, p);
    
  double dAB = lengthOfVector(p-Rab);
  double dBC = lengthOfVector(p-Rbc);
  double dCA = lengthOfVector(p-Rca);
  
  
  double min = dAB;
  DoubleVector3d result = Rab;
  
  if (dBC < min) {
    min = dBC;
    result = Rbc;
    }
 
  if (dCA < min)
   result = Rca;
  
    
  return (result);	
}



// ----------------------------------------------------------------------
// Name  : CheckPointInTriangle()
// Input : point - point we wish to check for inclusion
//         sO - Origin of sphere
//         sR - radius of sphere 
// Notes : 
// Return: TRUE if point is in sphere, FALSE if not.
// -----------------------------------------------------------------------  

bool CheckPointInSphere(DoubleVector3d point, DoubleVector3d sO, double sR) {
	
 double d = lengthOfVector(point-sO);
 
 if(d<= sR) return true;
 return false;	
}




// ----------------------------------------------------------------------
// Name  : tangentPlaneNormalOfEllipsoid()
// Input : point - point we wish to compute normal at 
//         eO - Origin of ellipsoid
//         eR - radius vector of ellipsoid 
// Notes : 
// Return: a unit normal vector to the tangent plane of the ellipsoid in the point.
// -----------------------------------------------------------------------  
DoubleVector3d tangentPlaneNormalOfEllipsoid(DoubleVector3d point, DoubleVector3d eO, DoubleVector3d eR) {

 DoubleVector3d p = point - eO;

 double a2 = eR.x * eR.x;
 double b2 = eR.y * eR.y;
 double c2 = eR.z * eR.z;

 
 DoubleVector3d res;
 res.x = p.x / a2;
 res.y = p.y / b2;
 res.z = p.z / c2;

 normalizeVector(res);	
 return (res);	
}



// ----------------------------------------------------------------------
// Name  : classifyPoint()
// Input : point - point we wish to classify 
//         pO - Origin of plane
//         pN - Normal to plane 
// Notes : 
// Return: One of 3 classification codes
// -----------------------------------------------------------------------  

int classifyPoint(DoubleVector3d point, DoubleVector3d pO, DoubleVector3d pN) {

 DoubleVector3d dir = pO - point;
 double d = dot(dir, pN);
 
 if (d<-0.001f)
  return PLANE_FRONT;	
 else
 if (d>0.001f)
  return PLANE_BACKSIDE;	

return ON_PLANE;	
}




