#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <iostream>
#include <fstream>

#include <vector>

#ifdef _WIN32
	#include <windows.h>
	#include <stdlib.h>
#endif

#include "HelperLibString.hpp"
#include "collision.hpp"
#include "Containers.hpp"
#include "Types.hpp"
#include "animation.hpp"
#include "envelope.hpp"
#include "os.h"

#ifdef ODE
	#include "ode/ode.h"
#endif

//#define SOFTWARE_TRANSFORMS 1
#define SOFTWARE_TRANSFORM_TEXCOORDS 1

#define TEXTURE_CUBE_FRONT		0x1
#define TEXTURE_CUBE_BACK		0x2
#define TEXTURE_CUBE_LEFT		0x4
#define TEXTURE_CUBE_RIGHT		0x8
#define TEXTURE_CUBE_TOP		0x10
#define TEXTURE_CUBE_BOTTOM		0x20
#define TEXTURE_NORMAL			0x40	//turn into a normal map (conversion done in converter)
#define TEXTURE_SPECULAR		0x80	//turn into a specular map

class TextureImage{

	private:
		unsigned int gl;
		bool glSet;
		
	protected:
		

	public:
		
		int loadedTextureLevel;

		bool ref;	//true: just gives filename where the textureimage can be found, false: includes texture data
		unsigned int x;
		unsigned int y;
		unsigned char* data;
		bool* dataLoaded;		//a pointers so that we can keep track of the actual data accross multiple bookkeeping protocols
		unsigned short	bytesPP;	//bytes per pixel
		String file;	//absolute path
		String fileRelative;
		int filePos;
		unsigned int flags;	//w00t

		String name;

		void reload(); //reloads the data, then deletes it, (for textureLevel)
		void load(); 	//loads the data if it has been deleted, doesn't delete it

		unsigned int glName(unsigned int set);
		unsigned int glName();
		bool glNameSet();	//true if name was set
		void glNameSet(bool set);
		
		unsigned long size();
		bool write(std::ofstream& out);
		TextureImage();
};

#define MATERIAL_SHADER_MIXER			0x1
#define MATERIAL_SHADER_ENVIRONMENT		0x2
#define MATERIAL_SHADER_DISPLACEMENT	0x4
#define MATERIAL_SHADER_SPECULAR		0x8

class Material{
	friend class Object;

	private:
		FloatVector4d oldDiffuse;
		FloatVector4d oldAmbient;
		FloatVector4d oldSpecular;
		int oldNonstaticRefs;
		float oldShiny;
	protected:
		
	public:
		
	
		bool uploaded;
		int nonstaticRefs;	//the number of nonstatic objects that reference this (used for optimization)
		Array<Object*> nonstaticRefList;
		void update();

		String name;

		unsigned int shader;	//shader flags

		FloatVector4d diffuse;
		FloatVector4d specular;
		FloatVector4d ambient;
		FloatVector4d reflectivity;	//the color of the reflectivity (usually a cube map)
		FloatVector2d texLightmapPos;
		FloatVector2d texLightmapScale;
		FloatVector2d texLightmapRot;
		FloatVector2d tex0pos;		//diffuse		| mix_base
		FloatVector2d tex0scale;
		FloatVector2d tex0rot;
		FloatVector2d tex1pos;		//spec			| mix_c_1
		FloatVector2d tex1scale;
		FloatVector2d tex1rot;
		FloatVector2d tex2pos;		//displacement	| mix_w_1
		FloatVector2d tex2scale;
		FloatVector2d tex2rot;
		FloatVector2d tex3pos;		//bump			| mix_c_2
		FloatVector2d tex3scale;
		FloatVector2d tex3rot;
		FloatVector2d tex4pos;		//mix_w_2
		FloatVector2d tex4scale;
		FloatVector2d tex4rot;
		FloatVector2d tex5pos;		//mix_c_3
		FloatVector2d tex5scale;
		FloatVector2d tex5rot;
		FloatVector2d tex6pos;		//mix_w_3
		FloatVector2d tex6scale;
		FloatVector2d tex6rot;

		bool dirty;

		float shiny;	//aka gloss/specular decay
		int textureDiffuse;	//-1 for no texture			|| serves as base texture for mixer
		int textureDisplacement;	//-1 for no texture
		int textureSpecular;	//-1 for no texture
		int textureBump;	//-1 for no texture

		int textureEnvironmentFront;	//-1 for no texture		||color1 for mixer
		int textureEnvironmentBack;	//-1 for no texture			||color1_weight for mixer
		int textureEnvironmentLeft;	//-1 for no texture			||color2 for mixer
		int textureEnvironmentRight;	//-1 for no texture		||color2_weight for mixer
		int textureEnvironmentTop;	//-1 for no texture			||color3 for mixer
		int textureEnvironmentBottom;	//-1 for no texture		||color3_weight for mixer

		unsigned long size();
		bool write(std::ofstream& out);

#ifndef XSICONVERT
		bool addTexture(String type,Path file);
		bool deleteTexture(String type);
#endif

		Material();
};

#define END_TRI 0x1
#define END_STRIP 0x2
#define START_TRI 0x3
#define START_STRIP 0x4

class Vertex{
	public:
		FloatVector3d pos;
		FloatVector3d normal;
		FloatVector3d tangent;
		FloatVector3d binormal;
		FloatVector2d uvLightmap;	//this is the only one saved to the file, all other must derive from this on load
		FloatVector2d uv0;			
		FloatVector2d uv1;
		FloatVector2d uv2;
		FloatVector2d uv3;
		FloatVector2d uv4;
		FloatVector2d uv5;
		FloatVector2d uv6;
		unsigned int end;
		float* envelope;
		unsigned int envelopeIndex;

		unsigned long size();	//in bytes
		bool write(std::ofstream& out);
		Vertex();
};

#define OBJECT_VISIBLE		0x1
#define OBJECT_COLLIDE		0x2
#define OBJECT_LIGHTMAP		0x4
#define OBJECT_STATIC		0x8
#define OBJECT_WATER		0x10
#define OBJECT_TRANSPARENT	0x20
#define OBJECT_REFLECTIVE	0x40
#define OBJECT_DYNAMIC		0x80	//physics dynamics
#define OBJECT_INSIDEBB		0x100	
#define OBJECT_NOFOG		0x200
#define OBJECT_NOCLIP		0x400
#define CONF_GLSL			0x800
#define CONF_ALL			0x1000
#define CONF_11				0x2000

class ObjectAddress;
class SceneGraphNode;

class Object{
	private:
		int oldVertexCount;
		FloatVector3d oldPos;
		FloatVector3d oldRot;
		FloatVector3d oldScale;
		bool oldVisible;
		FloatVector2d oldTexLightmapPos;
		FloatVector2d oldTexLightmapScale;
		FloatVector2d oldTexLightmapRot;
		FloatVector2d oldTex0pos;
		FloatVector2d oldTex0scale;
		FloatVector2d oldTex0rot;
		FloatVector2d oldTex1pos;
		FloatVector2d oldTex1scale;
		FloatVector2d oldTex1rot;
		FloatVector2d oldTex2pos;
		FloatVector2d oldTex2scale;
		FloatVector2d oldTex2rot;
		FloatVector2d oldTex3pos;
		FloatVector2d oldTex3scale;
		FloatVector2d oldTex3rot;
		FloatVector2d oldTex4pos;
		FloatVector2d oldTex4scale;
		FloatVector2d oldTex4rot;
		FloatVector2d oldTex5pos;
		FloatVector2d oldTex5scale;
		FloatVector2d oldTex5rot;
		FloatVector2d oldTex6pos;
		FloatVector2d oldTex6scale;
		FloatVector2d oldTex6rot;
		int oldMaterial;
		bool oldCollide;	//note:collide is a flag
		bool oldDynamic;	//note:dynamic is a flag
		unsigned long vertexCount;

		#ifdef ODE
			
		#endif

		Array<Vertex> vertices;
		
		Array<Vertex> transformedVertexCache;

		bool rebuildTransformedVertexCache;
		Vertex transformVertex(Vertex v,FloatVector3d pos,FloatVector3d rot, FloatVector3d scale);

		
		void buildBox();

	public:
	    //GLuint vboId;
	
		String constraintPosName;
		unsigned int constraintPosType;
		String constraintRotName;
		unsigned int constraintRotType;
		String constraintScaleName;
		unsigned int constraintScaleType;
		ObjectAddress* constraintPos;
		ObjectAddress* constraintRot;
		ObjectAddress* constraintScale;

		int drawCurrentEnvelope;
		Array<Vertex*>* envelopeVertices;
		Array<int>* envelopeVerticesEnvelopes;
		int envelopeVerticesCount;
		unsigned int* glListNameEnvelope;

		void CheckCollision(TCollisionPacket* colPackage);

		Array<ObjectAnimation> animations;
		Array<ObjectEnvelope> envelopes;

		Vertex& getVertex(int id);
		Vertex& getVertexRaw(int id);
		Vertex& setVertexRaw(int id);	//use this if you intend to edit the vertices!  This updates state that getVertexRaw doesn't (bounding box)
		int getVertexCount();
		void addVertex(Vertex);
		void addVertex();
		void setVertexCount(int id);
		void reserveVertices(int count);
		Array<Vertex>* Vertices();

		Array<unsigned int> glListNameDynamic;
		Array<Bool> listBuiltDynamic;
		unsigned int glListName;
		unsigned int glListNameQuick;

		#ifdef DEVELOPER
			unsigned int glListNameQuickPolygon;
			bool listBuiltQuickPolygon;
			unsigned int glListNameQuickEdge;
			bool listBuiltQuickEdge;
			unsigned int glListNameQuickVertex;
			bool listBuiltQuickVertex;
		#endif

		bool dirty;
		bool listBuilt;
		bool listBuiltQuick;

		bool boxBuilt;
		Box3d rawBox;	//contains untransformed object box
		Box3d box;		//bounding box - IN WORLD SPACE - no transformations need to be applied EXCEPT ROTATION (Box3d doesn't have enough data to represent rots)
		Box2d uv0Box;	//bounding box of texture
		Box2d uv1Box;
		Box2d uv2Box;
		Box2d uv3Box;
		Box2d uv4Box;
		Box2d uv5Box;
		Box2d uv6Box;

		bool nofog;
		bool noclip;
		bool insidebb;
		bool collide;	//note, this syncs with the flags (allows access to flag var from lua)
		bool dynamic;	//note, syncs with flags

		#ifdef ODE
			dMass ODEmass;
			dBodyID bodyId;
			dGeomID geomId;
			bool geomIdInit;
			bool bodyIdInit;
			
		#endif

		unsigned char type;	//0=poly, 1=point light, 2=spot light, 3=null, 4=camera, 5=ik root, 6=inf light, 7=ik joint, 8=ik eff, 9=curve
		unsigned int flags;	//w00t
		int material;	//-1 if none			
		int lightmap;	//-1 if no lightmap

		unsigned int sceneGraphLevel;
		SceneGraphNode* sceneGraphNode;

		bool visible;	//hide/show.  read from flags initially, but this actually controls visibility (not flags)

		FloatVector3d pos;
		FloatVector3d rot;		//in degrees.  (light: spotlight interest)
		FloatVector3d scale;	//(light: falloff_start [-1 if no falloff],falloff_end,cone angle,nil) (camera: fov,near,far)

		float friction;

		#ifdef ODE
			float mass;
			
			float bounce;
			float bounceThreshold;
			String collideType;	//"sphere","box","plane","cylinder"
			String oldCollideType;
			FloatVector3d collideParameters;
		#endif

		bool hasBasePose;
		FloatVector3d basePos;
		FloatVector3d baseRot;
		FloatVector3d baseScale;

		String name;
		String action;

		Object();

		String rotMode;

		void update();
		void become(Object*);	//copy all info from this object and become it

		unsigned long size();	//in bytes
		bool write(std::ofstream& out);
		void drawNormals();
		void drawBB();
		void draw(int mode=-1);

		void rebuildUVs();

		#ifdef DEVELOPER
			void drawPolygon();	//draw w/polygon picking info
			void drawVertex();	//draw w/vertex picking info
			void drawEdge();	//draw w/edge picking info
		#endif

};


#endif

