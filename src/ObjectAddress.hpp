#ifndef OBJECT_ADDRESS_HPP
#define OBJECT_ADDRESS_HPP 1


enum ObjectTypeEnum{
	NULLOBJECT,
	CAMERA,
	OBJECT,
	NONE,
	LIGHT,
	PARTICLE,
	PARTICLE_2D,
	PANEL,
	TEXTPANEL,
	THUMBNAILS,
	VIDEOPANEL,
	HAIR,
	LINE,
	LINE2D,
	POINT3D,
	NULL3D,
	MATERIAL,
	IK_ROOT,
	IK_JOINT,
	IK_EFFECTOR,
	CURVE,
	SPHERE,
	DISK,
	CIRCLE,
	BOX,
	INSTANCE,
	POLYGON2D
};

class Object;
class Camera;
class Light;
class Particle;
class Particle2d;
class Panel;
class TextPanel;
class Thumbnails;
class VideoPanel;
class Hair;
class Point3d;
class Line3d;
class Line2d;
class Material;
class NullObject;
class IkRoot;
class IkJoint;
class IkEffector;
class Curve;
class Sphere;
class Disk;;
class Circle;
class Box;
class Instance;
class Polygon2d;

class ObjectAddress{
	public:
		ObjectTypeEnum type;

		union{
			Object* object;
			Camera* camera;
			Light* light;
			Particle* particle;
			Particle2d* particle2d;
			Panel* panel;
			TextPanel* textpanel;
			Thumbnails* thumbnails;
			VideoPanel* videopanel;
			Hair* hair;
			Point3d* point;
			Line3d* line;
			Line2d* line2d;
			Material* material;
			NullObject* null;
			IkRoot* ikRoot;
			IkJoint* ikJoint;
			IkEffector* ikEffector;
			Curve* curve;
			Sphere* sphere;
			Disk* disk;
			Circle* circle;
			Box* box;
			Instance* instance;
			Polygon2d* polygon2d;
		};

};

#endif