#ifndef COLLISION_H
#define COLLISION_H

#include "Types.hpp"

class Object;

struct TCollisionPacket {
 Object* object;
 // data about player movement
 FloatVector3d velocity;
 FloatVector3d sourcePoint;
  
 // radius of ellipsoid.  
 FloatVector3d eRadius;
  
 // for error handling  
 FloatVector3d lastSafePosition;
 bool stuck; 
    
 // data for collision response 
 bool foundCollision;
 double    nearestDistance; // nearest distance to hit
 FloatVector3d nearestIntersectionPoint; // on sphere
 FloatVector3d nearestPolygonIntersectionPoint; // on polygon
	FloatVector3d nearestPolygonNormal;
};


#endif

