#ifndef CAMERA_H
#define CAMERA_H

#include "Types.hpp"
#include "HelperLibString.hpp"
#include "collision.hpp"
#include <iostream>
#include "physics.h"
#include "SceneGraph.hpp"

class Camera{
	private:
		void scaleToEllipsoid(FloatVector3d&radiusVector);
		void collisionDetection(FloatVector3d& sourcePoint, 
						FloatVector3d& velocityVector, 
						FloatVector3d& gravityVector);

		DoubleVector3d collideWithWorld(DoubleVector3d sourcePoint, DoubleVector3d velocityVector);
		
		
		TCollisionPacket collision;

		DoubleVector3d gravityVelocityHack;

	public:
		SceneGraphNode* sceneGraphNode;

		DoubleVector3d GetPosition(DoubleVector3d position, DoubleVector3d velocity);
		DoubleVector3d ellipsoidRadius;	//radius of collide elipsoid, aka: scale

		unsigned int sceneGraphLevel;

  int eye;
		float speed;

		DoubleVector3d pos;
		DoubleVector3d rot;
		DoubleVector3d scale;
		DoubleVector3d velocity;
		
		bool collided;	//indicates that the camera collided with something in the last frame

		bool fly;
		bool freeze;

		unsigned int flags;

		bool freezeFrustum;	//lock the frustum, to examine culling

		float fov;			//in perspective, the FOV, in ortho, a scaling factor for object sizes (so essentially the same thing)
		bool orthographic;
		bool centerOrtho;
		float clipNear;
		float clipFar;

		bool limitXOn;
		float limitXMax;
		float limitXMin;

		void write(std::ofstream& out);

		void viewport();
		Box2d getViewport();
		void projectionMatrix();
		void adjust();
		void move();
		void transform();
		void reverseTransform();
		void reverseTransformRot();

		void draw();	//call BEFORE adjust

		String name;

		Camera();
};

#endif

