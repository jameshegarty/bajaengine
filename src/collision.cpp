//-----------------------------------------------------------------------------
// File: Collision.cpp
//
// Desc: Implementation of the collision detection
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------

#include "vectormath.hpp"
#include "../objects.hpp"
#include "collision.hpp"
// ----------------------------------------------------------------------------
// Descr : Structure keeping track of the collisions as we find them. 
// Note  : Somewhat simplified from what is needed if you deside to implement
//         the features described in the section of dynamic geometry.
// ----------------------------------------------------------------------------





// ----------------------------------------------------------------------
// Name  : CheckCollision()
// Descr : Checks one mesh for collision
// Return: updated collision structure.
// -----------------------------------------------------------------------  

void Object::CheckCollision(TCollisionPacket* colPackage) {
 
 // plane data
 int A, B, C;
 FloatVector3d p1,p2,p3;
 FloatVector3d pNormal;
 FloatVector3d pOrigin;
 FloatVector3d v1, v2;

 
 // from package
 FloatVector3d source = colPackage->sourcePoint;
 FloatVector3d eRadius = colPackage->eRadius;
 FloatVector3d velocity = colPackage->velocity;	
 
 // keep a copy of this as it's needed a few times
 FloatVector3d normalizedVelocity = velocity;
 normalizeVector(normalizedVelocity);
 
 // intersection data
 FloatVector3d sIPoint;    // sphere intersection point
 FloatVector3d pIPoint;    // plane intersection point 	
 FloatVector3d polyIPoint; // polygon intersection point
 
 // how long is our velocity
 double distanceToTravel = lengthOfVector(velocity);	
 
 double distToPlaneIntersection;
 double distToEllipsoidIntersection;
  
 // loop through all faces in mesh  	
	for (unsigned int i=0; i<getVertexCount(); i+=3){
               
     A=i;
	 B=i+1;
	 C=i+2;

     // Get the data for the triangle in question and scale to ellipsoid space

     p1.x = getVertex(A).pos.x / eRadius.x;
     p1.y = getVertex(A).pos.y / eRadius.y;
     p1.z = getVertex(A).pos.z / eRadius.z;
        
     p2.x = getVertex(B).pos.x / eRadius.x;
     p2.y = getVertex(B).pos.y / eRadius.y;
     p2.z = getVertex(B).pos.z / eRadius.z;
             
     p3.x = getVertex(C).pos.x / eRadius.x;
     p3.y = getVertex(C).pos.y / eRadius.y;
     p3.z = getVertex(C).pos.z / eRadius.z;          

     // Make the plane containing this triangle.      
     pOrigin = p1;
     v1 = p2-p1;
     v2 = p3-p1;
    
        
     // You might not need this if you KNOW all your triangles are valid
     if (!(isZeroVector(v1) || isZeroVector(v2))) {
                         
     	// determine normal to plane containing polygon  
     	pNormal = wedge(v1, v2);
     	normalizeVector(pNormal);
	
     		// calculate sphere intersection point
     		sIPoint = source - pNormal;     
        
     		// classify point to determine if ellipsoid span the plane
     		int pClass = classifyPoint(sIPoint, pOrigin, pNormal);
         
     
     		// find the plane intersection point
     		if (pClass == PLANE_BACKSIDE) { // plane is embedded in ellipsoid
     
      			// find plane intersection point by shooting a ray from the 
      			// sphere intersection point along the planes normal.
      			distToPlaneIntersection = intersectRayPlane(sIPoint, pNormal, pOrigin, pNormal);
                       
      			// calculate plane intersection point
      			pIPoint.x = sIPoint.x + distToPlaneIntersection * pNormal.x; 
      			pIPoint.y = sIPoint.y + distToPlaneIntersection * pNormal.y; 
      			pIPoint.z = sIPoint.z + distToPlaneIntersection * pNormal.z; 	
       
     		}else { 
     
     			// shoot ray along the velocity vector
     			distToPlaneIntersection = intersectRayPlane(sIPoint, normalizedVelocity, pOrigin, pNormal);
               
     			// calculate plane intersection point
     			pIPoint.x = sIPoint.x + distToPlaneIntersection * normalizedVelocity.x; 
     			pIPoint.y = sIPoint.y + distToPlaneIntersection * normalizedVelocity.y; 
     			pIPoint.z = sIPoint.z + distToPlaneIntersection * normalizedVelocity.z; 	
          	
     		}
     
       
     
     		// find polygon intersection point. By default we assume its equal to the 
     		// plane intersection point
     
     		polyIPoint = pIPoint;
     		distToEllipsoidIntersection = distToPlaneIntersection;
     
     		if (!CheckPointInTriangle(pIPoint,p1,p2,p3)) { // if not in triangle
     	
      			polyIPoint = closestPointOnTriangle(p1, p2, p3, pIPoint);	
           
      			distToEllipsoidIntersection = intersectRaySphere(polyIPoint, -normalizedVelocity, source, 1.0f);  
                  
      			if (distToEllipsoidIntersection > 0) { 	
     			// calculate true sphere intersection point
     				sIPoint.x = polyIPoint.x + distToEllipsoidIntersection * -normalizedVelocity.x;
     				sIPoint.y = polyIPoint.y + distToEllipsoidIntersection * -normalizedVelocity.y;
     				sIPoint.z = polyIPoint.z + distToEllipsoidIntersection * -normalizedVelocity.z;
     			}
    
     		} 
     
    
  		// Here we do the error checking to see if we got ourself stuck last frame
   		if (CheckPointInSphere(polyIPoint, source, 1.0f)) 
			colPackage->stuck = true;
       
    
		// Ok, now we might update the collision data if we hit something
    		if ((distToEllipsoidIntersection > 0) && (distToEllipsoidIntersection <= distanceToTravel)) { 
     			if ((colPackage->foundCollision == false) || (distToEllipsoidIntersection < colPackage->nearestDistance))  {
           
                		// if we are hit we have a closest hit so far. We save the information
      				colPackage->nearestDistance = distToEllipsoidIntersection;
      		       	colPackage->nearestIntersectionPoint = sIPoint;
      				colPackage->nearestPolygonIntersectionPoint = polyIPoint;
      				colPackage->foundCollision = true;
					colPackage->object=this;
					colPackage->nearestPolygonNormal.x=getVertex(A).normal.x*0.333+getVertex(B).normal.x*0.333+getVertex(C).normal.x*0.333;
					colPackage->nearestPolygonNormal.y=getVertex(A).normal.y*0.333+getVertex(B).normal.y*0.333+getVertex(C).normal.y*0.333;
					colPackage->nearestPolygonNormal.z=getVertex(A).normal.z*0.333+getVertex(B).normal.z*0.333+getVertex(C).normal.z*0.333;
			}
    		} 
   } // if a valid plane 	
 } // for all faces	
}
