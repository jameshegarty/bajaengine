//-----------------------------------------------------------------------------
// based on "Improving the Numerical Robustness of Sphere Swept Collision Detection" 
// by Jeff Linahan
// and
// "Improved Collision Detection and Response"
// by Kasper Fauerby
//-----------------------------------------------------------------------------

#include "vectormath.hpp"
#include "camera.hpp"
#include "level.hpp"

// this collision method relies on this being small. If you make it large, you will get jittering
#define EPSILON 0.001f

const bool COLLISION_PRINTF = false;

//-----------------------------------------------------------------------------
// Name: GetPosition()
// Desc: Main collision detection function. This is what you call to get
//       a position.
//-----------------------------------------------------------------------------
DoubleVector3d Camera::GetPosition(DoubleVector3d position, DoubleVector3d velocity) {

  if(COLLISION_PRINTF){printf("GetPosition %f %f %f, %f %f %f\n",position.x,position.y,position.z, velocity.x, velocity.y, velocity.z);}

	collided=false;

 DoubleVector3d scaledPosition, scaledVelocity;	 
 DoubleVector3d finalPosition;

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
DoubleVector3d Camera::collideWithWorld(DoubleVector3d pos, DoubleVector3d velocity) {
  if(COLLISION_PRINTF){printf("collide with world pos %f %f %f, vmag %f, %f %f %f\n",pos.x, pos.y, pos.z, velocity.magnitude(),velocity.x, velocity.y, velocity.z);}
 
 
  DoubleVector3d dest = pos + velocity;
 
  DoubleVector3d first_plane_p;
  DoubleVector3d first_plane_n;

  for(int i=0; i<3; i++){
    if(COLLISION_PRINTF){
      printf("round %d pos %f %f %f vel %f %f %f\n",i,pos.x,pos.y,pos.z,velocity.x,velocity.y,velocity.z);
      printf("\tdest %f %f %f\n",dest.x,dest.y,dest.z);
    }

    // reset the collision package we send to the mesh 
    collision.velocity = velocity;
    collision.sourcePoint = pos;
    collision.foundCollision = false;
    collision.stuck = false;
    collision.nearestDistance = -1;	
    collision.eRadius=ellipsoidRadius;
  

    for(int i=0; i<level->colliders.size(); i++){
      level->colliders[i]->CheckCollision(&collision);	
    }

  
    // check return value here, and possibly call recursively 	
    
    if (collision.foundCollision == false){ 
      if(COLLISION_PRINTF){printf("no collision, return %f %f %f\n",dest.x, dest.y, dest.z);}
      return dest;
    }
   
		// If we are stuck, we just back up to last safe position
    
		if (collision.stuck) {
      if(COLLISION_PRINTF){printf("return last safe %f %f %f\n",collision.lastSafePosition.x, collision.lastSafePosition.y, collision.lastSafePosition.z);}
      //			return collision.lastSafePosition;
		}else{
      collision.lastSafePosition = pos;
    }
    
    double dist = collision.nearestDistance;
    double short_dist = fmax(dist - EPSILON, 0.0f);

    DoubleVector3d touchPoint = pos + velocity.normalized() * dist;
    pos += velocity.normalized() * short_dist;

    if(i==0){
      float long_radius = 1.0f + EPSILON;

      first_plane_p = collision.nearestPolygonIntersectionPoint;
      first_plane_n = touchPoint - collision.nearestPolygonIntersectionPoint;

      if(COLLISION_PRINTF){printf("\tplane %f %f %f n %f %f %f\n",first_plane_p.x,first_plane_p.y,first_plane_p.z, first_plane_n.x,first_plane_n.y,first_plane_n.z);}

      double pdist = plane_dist(first_plane_p, first_plane_n, dest);
      if(COLLISION_PRINTF){printf("\tpdist %f\n",pdist);}
      dest -= (pdist - long_radius) * first_plane_n; // project onto plane
      if(COLLISION_PRINTF){printf("\tnewdest %f %f %f\n",dest.x,dest.y,dest.z);}
      velocity = dest - pos;
      if(COLLISION_PRINTF){printf("\tnewvel %f %f %f\n",velocity.x,velocity.y,velocity.z);}
    } else if(i==1){
      //DoubleVector3d second_plane_p = touchPoint;
      DoubleVector3d second_plane_p = collision.nearestPolygonIntersectionPoint;
      DoubleVector3d second_plane_n = touchPoint - collision.nearestPolygonIntersectionPoint;

      if(COLLISION_PRINTF){printf("\tplane %f %f %f n %f %f %f\n",second_plane_p.x,second_plane_p.y,second_plane_p.z, second_plane_n.x,second_plane_n.y,second_plane_n.z);}

      DoubleVector3d crease = wedge(first_plane_n,second_plane_n).normalized();
      
      double dis = dot((dest - pos),crease); 
      velocity = dis * crease;
      dest = pos + velocity;
    }
  }

  if(COLLISION_PRINTF){printf("return %f %f %f\n",pos.x,pos.y,pos.z);}
  return pos;
}






