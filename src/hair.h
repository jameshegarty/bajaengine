#ifndef HAIR_H
#define HAIR_H

#include "Types.hpp"
#include "objects.hpp"
#include "Containers.hpp"
#include "HelperLibString.hpp"

#ifndef XSICONVERT
	#include "script.hpp"
#endif

/*

if wind==0 then it'll just go into a list and not update each frame
gravity affects the hair - each hair will need a random gravity rotation - the direction the hair bends due to gravity
	//wind will also bend it
*/

class HairTri{
	public:
		FloatVector3d pos1;
		FloatVector3d pos2;
		FloatVector3d pos3;
		FloatVector3d normal1;
		FloatVector3d normal2;
		FloatVector3d normal3;
		FloatVector2d coord1;
		FloatVector2d coord2;
		FloatVector2d coord3;
		float prevarea;	//not literally the next/prev area, actually a sequential thing for sorting
		float seqarea;
		float area;
};

class HairState{
	public:
		FloatVector4d* segmentColor;
		FloatVector3d* segmentPos;
		FloatVector3d normal;	//for modulateing wind, so that hairs going into the wind don't get messed up
		//IntVector2d	windCoord;
		FloatVector2d	windCoord;
};

class Hair{
	private:
		HairState* hairState;	//array of hairs
		unsigned char* windmap;
		unsigned char* densitymap;
		int windmapX;
		int windmapY;

		int densitymapX;
		int densitymapY;

		Array<HairTri> finalhairtriscopy;

		FloatVector3d windPos;

		Object* obj;
		void generate();
		int mapToTri(float xr,float xr2,Array<HairTri>& finalhairtris);
		HairState generateTheHair(int index,float r,float xr2,Array<HairTri>& finalhairtris);
		bool densityTest(int index,float xr,float xr2,Array<HairTri>& finalhairtris);
		FloatVector2d coordOnTri(HairTri& tri,float x,float y);

		float totalArea;
		float pastLength;
		float pastLengthRandom;
		FloatVector4d pastColorBase;
		FloatVector4d pastColorBaseRandom;
		FloatVector4d pastColorTip;
		FloatVector4d pastColorTipRandom;
		float pastNumber;
		float pastCurl;
		float windMapValue(float x,float y);
		float densityMapValue(float x,float y);
		int pastSegments;

	public:
		unsigned char type;	//0=point light,1=spot light
		unsigned int flags;
		bool visible;

		FloatVector4d colorBase;
		FloatVector4d colorBaseRandom;
		FloatVector4d colorTip;
		FloatVector4d colorTipRandom;

		float curl;				//
		//float clump;			//tendency for groups of hair to form | eh, just use a density map
		float length;			//length of each strand
		float lengthRandom;
		float width;			//width of each strand
		FloatVector3d windVelocity;	
		FloatVector3d windPower;	
		int number;				//number of hairs created
		int segments;			//number of segmets drawn in each strand of hair
		//float wind;				//
		float stiffness;		//how much the hair is affected by wind+gravity

		String name;

		bool loadWindMap(String file);
		bool loadColorMapBase(String file);
		bool loadColorMapTip(String file);
		bool loadDensityMap(String file);
		bool setObject(String obj);
		void draw();
		Hair();
};

int luaHairSetObject(lua_State *L);
int luaHairLoadWindMap(lua_State *L);
int luaHairLoadDensityMap(lua_State *L);


#endif
