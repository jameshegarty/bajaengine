#ifndef EDITOR_H
#define EDITOR_H
#ifdef DEVELOPER

//toggle with ~ key

#ifdef _WIN32
#include <windows.h>	// Header File For Windows
#endif

#include "GlHeader.hpp"

#include "level.hpp"
#include "Types.hpp"
#include "helperLibstring.hpp"
#include "text.hpp"

#include "EditorUndo.h"

class Editor{
	private:


		FontId monoSmall;
		FontId monoLarge;

		bool visible;
		
		String forceShowGui;
		String forceHideGui;

		float rotHack;

		Array<Object*> soloedObjects;	//initially NULL
		Array<Light*> soloedLights;		//initially NULL

		Map<Object*,bool> objectSoloState;
		Map<Light*,bool> lightSoloState;

		

		GLUquadricObj* quad;
		
		void scalePolygons(int i,float x,float y, float z);

		void movePolygons(int i,float x,float y, float z);
		void moveEdges(int i,float x,float y, float z);
		void moveVertices(int i,float x,float y, float z);

		void drawArrowCamera();
		void drawArrowTranslate(FloatVector3d pos);
		void drawArrowRotate(FloatVector3d pos);
		void drawArrowScale(FloatVector3d pos);
		void addButton(String name,void (*action)(),String icon,int panel); //0=left, 1=right, 2=top,3=bottom

		void loadLightmapSizes();
		void saveLightmapSizes();
		
		
		void invertNormals();

		void duplicateEdges();
		void duplicatePolygon();
		void bridgePolygons();

		void drawProjects();
		void drawExplorer();
		int drawExplorerNode(SceneGraphNode&,int);
		void drawProperties();
		void projectsInit();
		void drawMaterial();

		void initUv();
		void drawUv();
		void initAlerts();
		void drawAlerts();
		void initExplorer();
		void initProperties();
		void initMaterial();

		void initScript();
		void drawScript();
		
		void drawNew();
		void initNew();

		FloatVector3d freePerspectiveRot;
		FloatVector3d freePerspectivePos;
		float freePerspectiveFov;
		float topZoom;
		
		
		void buildEditorList();

		void rebuildSelection();
		
		
		void addAllEdges(int oid, int eid);
		void addAllVertices(int oid, int eid);
		

		Array<ObjectAddress> grid;

		

		String editorGui;
		Map<String,String> editorTool;	//key=gui, value=tool
		Map<String,String> editorToolPast;	//key=gui, value=tool

		void tools();
		void startStickyTimer();
		bool unstik();

		bool stickywatch;
		String lastToolString;

		String cameraMode;

		bool hideGUIs;

		FloatVector2d remapScreenWorld(float x, float y);

	public:

		bool expertMode;	
		void hideSelection();
		void freeCamera();
		void freeOrthographicCamera();
		void topCamera();
		void rightCamera();
		void frontCamera();

		void centerAll();

		void toggleSolo();
		void quit();
		void selectAll();

		void deleteSelection();

		FloatVector3d cameraCenter;
		float cameraDistance;

		void updateCamera();
		void clearSelection();

		EditorUndo undo;

		void yescancel(String text,void (*yescallback)());
		void yesnocancel(String text,void (*yescallback)(),void (*nocallback)());
		void openfile(String dir,String type,void (*opencallback)(String));

		void saveLevel();

		bool marquee;

		void editorError(String);
		void editorWarning(String);

		bool activateTool(String tool, String gui);
		bool activateGui(String gui);
		bool deactivateGui(String gui);
		void forceGui(String gui,bool active);
		bool activateEvent(String key,bool ctrl, String gui);	//for non-modal stuff
		void forceActive(String tool,String gui);
		bool toolActive(String tool,String gui);
		bool unstick();
		bool wasSticky();	//true if the last activate message was due to a sticky release
		String previousTool(String gui);

		ObjectAddress add(String name,String type);

		String mainScriptFile;
		String loadedMainScript;

		Array<String> projectList;
		Array<Array<String> > levelList;
		Array<String> mainScriptFiles;

		//these are for the currently loaded project
		Array<String> scriptFiles;	//all this project's script filenames, including the main script
		Array<String> loadedScripts;
		Array<String> lmtpacker;
		Array<String> misc;

		int loadedProject;
		int loadedLevel;

		void centerSelected();

		void saveProjectDatabase();
		void loadProjectDatabase();

		void showProjects();

		bool scriptEditor;
		void drawStatus();
		void doEditorPickingObjects();
		void doEditorPickingPolygons();
		void doEditorPickingEdges();
		void doEditorPickingVertices();
		void draw();
		void initGrid();
		bool init();
		void show();
		bool scriptInit();
		void preCamera();
		bool isVisible();
		void postCamera();
		Editor();
};

extern Editor editor;

void addCubeAction();
void addPointLightAction();
void addSpotlightAction();
void addGridAction();

extern bool mouseVisibleHack;
extern bool oldEditorMouseVisible;
extern FloatVector4d oldEditorMouseColor;
extern int oldEditorMouseTextureId;
extern FloatVector2d oldEditorMouseScale;
extern bool renderLMMode;
extern int statusTexId;
extern Map<String,IntVector2d> lmSizes;
extern Array<Object*> selectedObjects;
extern Array<Light*> selectedLights;
extern Array<NullObject*> selectedNulls;
extern Array<Array<int> > selectedPolygons;
extern Array<Array<int> > selectedEdges;
extern Array<Array<int> > selectedVertices;
extern bool editorButtonDownHack;

extern ObjectAddress toolCommands;
extern String toolCommandsStart;

extern ObjectAddress stickyToolCommands;

extern ObjectAddress editorLeftBkg;
extern ObjectAddress editorRightBkg;
extern ObjectAddress editorTopBkg;
extern ObjectAddress editorBottomBkg;
extern ObjectAddress scriptBkg;
extern ObjectAddress scriptText;
extern ObjectAddress tooltipBkg;
extern ObjectAddress tooltipText;
extern ObjectAddress solomode;

extern ObjectAddress viewTitle;

extern ObjectAddress marqueeBkg;

extern Array<ObjectAddress> buttonsL;
extern Array<ObjectAddress> buttonsLB;
extern Array<ObjectAddress> buttonsLI;

extern Array<ObjectAddress> buttonsR;
extern Array<ObjectAddress> buttonsRB;
extern Array<ObjectAddress> buttonsRI;

extern Array<ObjectAddress> buttonsT;
extern Array<ObjectAddress> buttonsTB;
extern Array<ObjectAddress> buttonsTI;

extern Array<ObjectAddress> buttonsB;
extern Array<ObjectAddress> buttonsBB;
extern Array<ObjectAddress> buttonsBI;

extern int mousetexid;

extern bool editorLocal;


extern Array<Line3d*> editorLines;
extern Array<TextPanel*> editorTextpanels;
extern Array<Panel*> editorPanels;
extern Array<Line2d*> editorLines2d;
extern Array<Polygon2d*> editorPolygons2d;
#endif
#endif