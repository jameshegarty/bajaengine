//-----------------------------------------------------------------------------
// File: Response.cpp
//
// Desc: Implementation of the collision response
//
// Copyright (c) 2000 Telemachos of Peroxide
// www.peroxide.dk
//-----------------------------------------------------------------------------

#include "vectormath.hpp"
#include "../camera.hpp"
#include "../level.hpp"

#define EPSILON 0.05f


//-----------------------------------------------------------------------------
// Name: GetPosition()
// Desc: Main collision detection function. This is what you call to get
//       a position.
//-----------------------------------------------------------------------------
FloatVector3d Camera::GetPosition(FloatVector3d position, FloatVector3d velocity) {
	
	collided=false;

 FloatVector3d scaledPosition, scaledVelocity;	 
 FloatVector3d finalPosition;

 // the first thing we do is scale the player and his velocity to
 // ellipsoid space
 scaledPosition = position / ellipsoidRadius;
 scaledVelocity = velocity / ellipsoidRadius;	
	
 // call the recursive collision response function	
 finalPosition = collideWithWorld(scaledPosition, scaledVelocity);
 	
 // when the function returns the result is still in ellipsoid space, so
 // we have to scale it back to R3 before we return it 	
 finalPosition = finalPosition * ellipsoidRadius;
  
 return finalPosition; 	
	
}




//-----------------------------------------------------------------------------
// Name: collideWithWorld()
// Desc: Recursive part of the collision response. This function is the
//       one who actually calls the collision check on the meshes
//-----------------------------------------------------------------------------
FloatVector3d Camera::collideWithWorld(FloatVector3d position, FloatVector3d velocity) {
 
  FloatVector3d pos;
 
 
  // do we need to worry ?
  if (lengthOfVector(velocity) < EPSILON){
	  //collided=false;
    return position;
  }else{

	  //int lol=0;
  }
 	
  FloatVector3d destinationPoint = position + velocity;
 
  // reset the collision package we send to the mesh 
  collision.velocity = velocity;
  collision.sourcePoint = position;
  collision.foundCollision = false;
  collision.stuck = false;
  collision.nearestDistance = -1;	
  collision.eRadius=ellipsoidRadius;
  


  for(int i=0; i<level->colliders.size(); i++){
		level->colliders[i]->CheckCollision(&collision);	
  }

   	
  
  
  // check return value here, and possibly call recursively 	
	
	if (collision.foundCollision == false){ 
		// if no collision move very close to the desired destination. 
		float l = lengthOfVector(velocity);
		FloatVector3d V = velocity; 
		setLength(V, l-EPSILON);
	     	
		// update the last safe position for future error recovery	
		collision.lastSafePosition = position;      

		// return the final position
		return position + V;
	}else { // There was a collision
   
		collided=true;
		// If we are stuck, we just back up to last safe position
		if (collision.stuck) {
			return collision.lastSafePosition;
		}
	    
	      
		// OK, first task is to move close to where we hit something :
		FloatVector3d newSourcePoint;
	                   
		// only update if we are not already very close
		if (collision.nearestDistance >= EPSILON) {
	         
			FloatVector3d V = velocity;
			setLength(V, collision.nearestDistance-EPSILON);
			newSourcePoint = collision.sourcePoint + V;
		}else{
			newSourcePoint = collision.sourcePoint;
		}
	   

		// Now we must calculate the sliding plane
		FloatVector3d slidePlaneOrigin = collision.nearestPolygonIntersectionPoint;
		FloatVector3d slidePlaneNormal = newSourcePoint - collision.nearestPolygonIntersectionPoint;
	  
	      
		// We now project the destination point onto the sliding plane
		double l = intersectRayPlane(destinationPoint, slidePlaneNormal, 
									slidePlaneOrigin, slidePlaneNormal); 

		// We can now calculate a new destination point on the sliding plane
		FloatVector3d newDestinationPoint;
		
		
		newDestinationPoint.x = destinationPoint.x + l * slidePlaneNormal.x;
		newDestinationPoint.y = destinationPoint.y + l * slidePlaneNormal.y;
		newDestinationPoint.z = destinationPoint.z + l * slidePlaneNormal.z;
		
	    
		// Generate the slide vector, which will become our new velocity vector
		// for the next iteration
		FloatVector3d newVelocityVector = newDestinationPoint - collision.nearestPolygonIntersectionPoint;
	       	       
		// now we recursively call the function with the new position and velocity 
		collision.lastSafePosition = position;
		newVelocityVector*=1-collision.object->friction;
		return collideWithWorld(newSourcePoint, newVelocityVector); 
  }
}






