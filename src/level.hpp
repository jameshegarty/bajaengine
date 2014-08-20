#ifndef LEVEL_H
#define LEVEL_H

#include "Types.hpp"
#include "camera.hpp"
#include "light.h"
#include "objects.hpp"
#include "particle.h"
#include "Containers.hpp"
#include "string.h"
#include "panel.h"
#include "null.h"
#include "line.h"
#include "line2d.hpp"
#include "point.h"
#include "particle2d.h"
#include "textpanel.h"
#include "thumbnails.h"
#include "videopanel.h"
#include "hair.h"
#include "curve.hpp"
#include "ikchain.hpp"
#include "SphereObject.hpp"
#include "disk.hpp"
#include "circle.hpp"
#include "BoxObject.hpp"
#include "instance.hpp"
#include "Polygon2d.hpp"
#include "SceneGraph.hpp"
#include "ObjectAddress.hpp"
#include "os.h"

#define LOAD_PROGRESSIVE 1

/*lua:
x.add(name,type)
x.load(file)
x.delete(name)
*/

#ifdef LOAD_PROGRESSIVE
	extern bool currentlyLoading;
#endif

#ifndef XSICONVERT
	#include "script.hpp"
#endif


class Fog{
	private:

	public:

		FloatVector3d color;
		float fogNear;
		float fogFar;
		bool on;

		void perFrame();
		bool update();
		Fog();

};

class Level{
	friend class Material;

	protected:
		
	private:
		LuaType* luaCameraFov;
		LuaType* luaCameraFly;
		LuaType* luaCameraFreeze;
		LuaType* luaCameraFreezeFrustum;
		LuaType* luaCameraSpeed;
		LuaType* luaCameraPosX;
		LuaType* luaCameraPosY;
		LuaType* luaCameraPosZ;
		LuaType* luaCameraRotX;
		LuaType* luaCameraRotY;
		LuaType* luaCameraRotZ;
		LuaType* luaCameraScaleX;
		LuaType* luaCameraScaleY;
		LuaType* luaCameraScaleZ;
		LuaType* luaCameraClipNear;
		LuaType* luaCameraClipFar;
		LuaType* luaCameraLimitXOn;
		LuaType* luaCameraLimitXMin;
		LuaType* luaCameraLimitXMax;
		
		void loadingScreen();
		void resolveChains();
		void resolveConstraints();
		void resolveEnvelopes();

		bool loadObj(Path file);

	public:
		unsigned int glListName;	//the big display list for static objects

		bool luaUpload(Material* o);
		bool luaUpload(Particle* o);
		bool luaUpload(Particle2d* o);
		bool luaUpload(Object* o);
		bool luaUpload(Panel* o);
		bool luaUpload(Light* o);
		bool luaUpload(TextPanel* o);
		bool luaUpload(Camera* o);
		bool luaUpload(Thumbnails* o);
		bool luaUpload(VideoPanel* o);
		bool luaUpload(Hair* o);
		bool luaUpload(Point3d* o);
		bool luaUpload(Line3d* o);
		bool luaUpload(Line2d* o);
		bool luaUpload(NullObject* o);
		bool luaUpload(IkRoot* o);
		bool luaUpload(IkJoint* o);
		bool luaUpload(IkEffector* o);
		bool luaUpload(Curve* o);
		bool luaUpload(Sphere* o);
		bool luaUpload(Disk* o);
		bool luaUpload(Circle* o);
		bool luaUpload(Box* o);
		bool luaUpload(Instance* o);
		bool luaUpload(TextureImage* o);

		bool luaRemove(Object* o);
		bool luaRemove(Panel* o);
		bool luaRemove(Instance* o);

		LuaType* luaMat;
		LuaType* luaObj;
		LuaType* luaAddress;
		LuaType* luaManagerAddress;
		LuaType* luaFog;
		LuaType* luaCamera;

		bool luaUploadAll();

		Map<int,  std::vector<Object*>  > textureSorter; //(texture-shader)

		unsigned long objectCount;

		unsigned long textureCount;
		Array<TextureImage> textures;

		unsigned long lightmapCount;
		Array<TextureImage> lightmaps;

		unsigned long materialCount;
		SafeArray<Material> materials;

		bool defaultMaterial();				//create the default material, if needed

		int test;

		Fog fog;

		FloatVector2d viewportPos;
		FloatVector2d viewportScale;

		FloatVector3d ambient;	//global ambience, added to object ambiences
		FloatVector3d ambientOverride;	//multiplied by obj amb

		//indices
		Array<Object*> colliders;
		Array<Object*> dynamicObjects;			//affected by physics
		Array<Object*> staticObjects;			//these don't ever move
		Array<Object*> nonstaticObjects;		//these are each made into an individual list, that's remade when it's set to dirty in its update func
		Array<Object*> transparentObjects;		//objects with an alpha channel or color.a<1; sorted and drawn last
		Array<Object*> reflectiveObjects;		//objects with a real time reflection map
		
		Array<Object*> objects;	//all objects (meshes, not other types of objects)
		Array<Light*> lights;	
		Array<Camera*> cameras;
		Array<Particle*> particles;
		Array<Particle2d*> particles2d;
		Array<Panel*> panels;
		Array<Line3d*> lines;
		Array<Line2d*> lines2d;
		Array<Point3d*> points;
		Array<Thumbnails*> thumbnails;
		Array<TextPanel*> textpanels;
		Array<VideoPanel*> videopanels;
		Array<Hair*> hair;
		Array<NullObject*> nulls;
		Array<IkRoot*> ikRoots;
		Array<IkJoint*> ikJoints;
		Array<IkEffector*> ikEffectors;
		Array<Curve*> curves;
		Array<Sphere*> spheres;
		Array<Disk*> disks;
		Array<Circle*> circles;
		Array<Box*> boxes;
		Array<Instance*> instances;

		Object* water;

		Map<String,Object*> objectKey;
		Map<String,Light*> lightKey;
		Map<String,Camera*> cameraKey;
		Map<String,Particle*> particleKey;
		Map<String,Particle2d*> particle2dKey;
		Map<String,Panel*> panelKey;
		Map<String,Line3d*> lineKey;
		Map<String,Line2d*> line2dKey;
		Map<String,Point3d*> pointKey;
		Map<String,TextPanel*> textpanelKey;
		Map<String,Thumbnails*> thumbnailsKey;
		Map<String,VideoPanel*> videopanelKey;
		Map<String,Hair*> hairKey;
		Map<String,NullObject*> nullKey;
		Map<String,Material*> materialKey;
		Map<String,int> materialIndexKey;
		Map<String,IkRoot*> ikRootKey;
		Map<String,IkJoint*> ikJointKey;
		Map<String,IkEffector*> ikEffectorKey;
		Map<String,Curve*> curveKey;
		Map<String,Sphere*> sphereKey;
		Map<String,Disk*> diskKey;
		Map<String,Circle*> circleKey;
		Map<String,Box*> boxKey;
		Map<String,Instance*> instanceKey;

		SceneGraph sceneGraph;

		Camera* camera;
		
		bool visible;
		bool load(Path file);
		bool save(Path file, bool developer=false);
		bool savePrep(Path file, bool developer=false);

		ObjectAddress add(String name,String type);
		bool remove(String name);
		bool copy(String from, String to);
		bool rename(String from, String to);

		bool applyMaterial(String objectname, String mat);

		String maskLevel;	// name of another level to use as a mask

		String name;
		Path file;

		bool scriptInit(bool reload=false);

		bool objectExists(String name);	//searches all object types (meshes, nulls, etc) and returns true if the named object exists

		void perFrame();
		void update();
		Level();
		~Level();
};

bool newLevel();
bool deleteLevel(String name);
bool hideLevel(String name);
bool showLevel(String name);
bool reloadLevel();	//reload CURRENT level, completely

int luaAdd(lua_State *L);
int luaMask(lua_State *L);
int luaCopy(lua_State *L);
int luaLoad(lua_State *L);
int luaSave(lua_State *L);
int luaDelete(lua_State *L);
int luaRename(lua_State *L);
int luaLevelHide(lua_State *L);
int luaLevelShow(lua_State *L);
int luaLevelSwap(lua_State *L);
int luaLevelNew(lua_State *L);
int luaLevelDelete(lua_State *L);
int luaLevelList(lua_State *L);

extern Array<Level*> loadedLevels;
extern Level* level;

#endif

