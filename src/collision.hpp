#ifndef COLLISION_H
#define COLLISION_H

#include "Types.hpp"

class Object;

struct TCollisionPacket {
  Object* object;
  // data about player movement
  DoubleVector3d velocity;
  DoubleVector3d sourcePoint;
  
  // radius of ellipsoid.  
  DoubleVector3d eRadius;
  
  // for error handling  
  DoubleVector3d lastSafePosition;
  bool stuck; 
  
  // data for collision response 
  bool foundCollision;
  double    nearestDistance; // nearest distance to hit
  DoubleVector3d nearestIntersectionPoint; // on sphere
  DoubleVector3d nearestPolygonIntersectionPoint; // on polygon
  //	DoubleVector3d nearestPolygonNormal;
};


#endif

