#ifndef THUMBNAILS_H
#define THUMBNAILS_H

#include "Types.hpp"
#include "HelperLibString.hpp"
#include "Helperlib/Containers.hpp"
#include "script.hpp"
class Thumb{
	public:
		FloatVector2d pos;
		FloatVector2d scale;
		float rotate;
		int thumbTex;
		String path;
		bool loaded;
		//int index;

		FloatVector2d offsetTL;
		FloatVector2d offsetTR;
		FloatVector2d offsetBL;

		Thumb();
};

class Thumbnails{
	private:
		Array<Thumb*> visibleSet;			//currently visible thumbs
		Array<Thumb*> storedVisibleSet;

		Array<Thumb*> currentlyVisibleThumbs;

		//Array<Thumb> thumbs;
		SafeArray<Thumb> thumbs;			//all thumbs
		Map<String,int> remap;				//remap from filename to id

		bool dirty;
		FloatVector3d lastPos;
		FloatVector2d lastScale;
		float lastThumbnailSize;
		
	public:
		bool mouseover;
		String mouseoverThumbnail;
		//int mouseoverThumbnailInt;
		void setMouseover(int i);

		String name;

		bool visible;

		FloatVector3d pos;
		FloatVector2d scale;
		FloatVector3d rot;
		float thumbnailSize;

		int picsPerRow;

		FloatVector2d thumbPos(String);
		FloatVector2d thumbScale(String);
		bool add(String name);
		bool load(Thumb* t,String name);
		bool rotate(String name,float degrees);
		void doDraw();
		void draw();
		void clear();
		void storeVisibleSet();
		void restoreVisibleSet();
		bool isThumbVisible(String name);
		Array<Thumb*> currentlyVisible();
		Array<Thumb*> getVisibleSet();

		Thumbnails();
};

int luaThumbnailsGetVisibleSet(lua_State *L);
int luaThumbnailsCurrentlyVisible(lua_State *L);
int luaThumbnailsClear(lua_State *L);
int luaThumbnailsAdd(lua_State *L);
int luaThumbnailsThumbPos(lua_State *L);
int luaThumbnailsThumbScale(lua_State *L);
int luaThumbnailsRotate(lua_State *L);
int luaThumbnailsIsThumbVisible(lua_State *L);
int luaThumbnailsStoreVisibleSet(lua_State *L);
int luaThumbnailsRestoreVisibleSet(lua_State *L);

#endif
