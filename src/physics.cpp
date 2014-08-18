#include "physics.h"
#include "script.hpp"
#include "level.hpp"
#include "HelperLibMath.hpp"
#include "FileIo.hpp"
#include "Log.hpp"

Physics physics;

Physics::Physics(){
	gravity.y=-10;
	gravity.y=-1;
	collide="slide";

#ifdef ODE
	worldId=NULL;
#endif
	
}

static int luaTrace(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=3){
		console().write("Trace Error: bad arguments");
		return 0;
	}

	FloatVector3d from;
	FloatVector3d velocity;
	FloatVector3d rad;

	if(args[0].type==SCRIPT_TABLE){
		if(args[0].children.size()==3){
			for(int i=0; i<3; i++){
				if(args[0].children[i].name=="x"){
					from.x=args[0].children[i].value.ds;
				}else if(args[0].children[i].name=="y"){
					from.y=args[0].children[i].value.ds;
				}else if(args[0].children[i].name=="z"){
					from.z=args[0].children[i].value.ds;
				}
			}
		}else{
			console().write("Trace Error: bad arguments, from wrong size");
			return 0;
		}
	}else{	
		console().write("Trace Error: bad arguments, from must be a vector3d");
		return 0;
	}

	if(args[1].type==SCRIPT_TABLE){
		if(args[1].children.size()==3){
			for(int i=0; i<3; i++){
				if(args[1].children[i].name=="x"){
					velocity.x=args[1].children[i].value.ds;
				}else if(args[1].children[i].name=="y"){
					velocity.y=args[1].children[i].value.ds;
				}else if(args[1].children[i].name=="z"){
					velocity.z=args[1].children[i].value.ds;
				}
			}
		}else{
			console().write("Trace Error: bad arguments, velocity vector wrong size");
			return 0;
		}
	}else{	
		console().write("Trace Error: bad arguments, velocity must be vector3d");
		return 0;
	}

	if(args[2].type==SCRIPT_TABLE){
		if(args[2].children.size()==3){
			for(int i=0; i<3; i++){
				if(args[2].children[i].name=="x"){
					rad.x=args[2].children[i].value.ds;
				}else if(args[2].children[i].name=="y"){
					rad.y=args[2].children[i].value.ds;
				}else if(args[2].children[i].name=="z"){
					rad.z=args[2].children[i].value.ds;
				}
			}
		}else{
			console().write("Trace Error: bad arguments, radius wrong number of arguments");
			return 0;
		}
	}else{	
		console().write("Trace Error: bad arguments, radius vector must be vector3d");
		return 0;
	}

	FloatVector3d oor=level->camera->ellipsoidRadius;
	level->camera->ellipsoidRadius=rad;
	FloatVector3d f=level->camera->GetPosition(from,velocity);
	level->camera->ellipsoidRadius=oor;

	script().resultTable(L);
	script().result(L,"x",f.x);
	script().result(L,"y",f.y);
	script().result(L,"z",f.z);

	return 1;
}

bool Physics::init(){
	
	#ifdef LOOP_DEBUG
		logs().main.write("START PHYSICS INIT");
	#endif

	#ifdef ODE
		worldId=dWorldCreate();
	#endif

	#ifdef LOOP_DEBUG
		logs().main.write("START PHYSICS INIT SCRIPT");
	#endif

	LuaType* luaa=script().add("physics");
	
	LuaType* clua=script().add("gravity",luaa);

		script().add("x",&gravity.x,clua);
		script().add("y",&gravity.y,clua);
		script().add("z",&gravity.z,clua);

	script().add("collide",&collide,luaa);
	script().add("trace",luaTrace,luaa);


	#ifdef LOOP_DEBUG
		logs().main.write("START PHYSICS INIT END");
	#endif

	return true;
}

bool Physics::scriptInit(){
	//addforce
	//addtorque
	//addrelativeforce
	//addrelativetorque
	//addforceatpos

	return true;
}

void Physics::perframe(){
	//dWorldStep(worldId);

	return;	//not implemented yet

#ifdef ODE
	for(int i=0; i<level->colliders.size(); i++){
		Object* obj=level->colliders[i];

		if(level->colliders[i]->collideType=="box" || level->colliders[i]->collideType=="cube"){
			
			if(level->colliders[i]->oldCollideType!="box" && level->colliders[i]->oldCollideType!="cube"){
				obj->geomId=dCreateBox(spaceId,obj->collideParameters.x,obj->collideParameters.y,obj->collideParameters.z);
				dGeomSetBody(obj->geomId,obj->bodyId);
				obj->geomIdInit=true;
				level->colliders[i]->oldCollideType=level->colliders[i]->collideType;
			}

			if(obj->boxBuilt){
				float lx=level->colliders[i]->box.px-level->colliders[i]->box.nx;
				float ly=level->colliders[i]->box.py-level->colliders[i]->box.ny;
				float lz=level->colliders[i]->box.pz-level->colliders[i]->box.nz;

				dGeomBoxSetLengths (level->colliders[i]->geomId, lx,  ly, lz);
			}
		}else if(obj->collideType=="sphere"){
			if(obj->oldCollideType!="sphere"){
				obj->geomId=dCreateSphere(spaceId,obj->collideParameters.x);
				dGeomSetBody(obj->geomId,obj->bodyId);
				obj->geomIdInit=true;
			}

		}else if(obj->collideType=="plane"){
			obj->geomId=dCreatePlane(spaceId,obj->collideParameters.x,obj->collideParameters.x,obj->collideParameters.x,obj->collideParameters.x);

		}else if(obj->collideType=="cylindar"){
			obj->geomId=dCreateCCylinder(spaceId,obj->collideParameters.x,obj->collideParameters.y);

		}else if(obj->collideType=="triangle"){
			if(obj->oldCollideType!="triangle"){
				dTriMeshDataID tridat=dGeomTriMeshDataCreate();

				obj->geomId=dCreateTriMesh (spaceId, tridat,
				NULL,
				NULL,
				NULL);

				dGeomSetBody(obj->geomId,obj->bodyId);
				obj->geomIdInit=true;
				level->colliders[i]->oldCollideType=level->colliders[i]->collideType;
			}
		}

		if(obj->collide){
			dGeomEnable(obj->geomId);
		}else{
			dGeomDisable(obj->geomId);
		}

		for(int j=0; j<level->colliders.size(); j++){

			if(!level->colliders[i]->geomIdInit || !level->colliders[j]->geomIdInit){
				continue;
			}

			dContactGeom contactg[2];

			int cnt=dCollide(level->colliders[i]->geomId,level->colliders[j]->geomId,0,contactg,sizeof(dContactGeom));

			if(cnt>0){
				dSurfaceParameters surf;
	
				surf.mode=0;
				surf.mode=dContactBounce;
				surf.mu=level->colliders[i]->friction;
				surf.bounce=level->colliders[i]->bounce;
				surf.bounce_vel=level->colliders[i]->bounceThreshold;

				dContact contact;
				contact.geom=contactg[0];
				contact.surface=surf;

				dJointID joint=dJointCreateContact(worldId,0,&contact);

				if(!level->colliders[i]->dynamic){

					dJointAttach(joint,level->colliders[j]->bodyId,0);
				}else if(!level->colliders[j]->dynamic){

					dJointAttach(joint,level->colliders[i]->bodyId,0);
				}else{

					dJointAttach(joint,level->colliders[i]->bodyId,level->colliders[j]->bodyId);
				}
			}
		}
	}

	dWorldSetGravity(worldId,gravity.x,gravity.y,gravity.z);
	dWorldQuickStep(worldId,1);

	for(int i=0; i<level->dynamicObjects.size(); i++){

		if(!level->dynamicObjects[i]->bodyIdInit){
			continue;
		}

		if(level->dynamicObjects[i]->dynamic){
			dBodyEnable(level->dynamicObjects[i]->bodyId);
		}else{
			dBodyDisable(level->dynamicObjects[i]->bodyId);
		}


		const dReal* pos=new dReal[3];

		pos=dBodyGetPosition(level->dynamicObjects[i]->bodyId);

		level->dynamicObjects[i]->pos.x=pos[0];
		level->dynamicObjects[i]->pos.y=pos[1];
		level->dynamicObjects[i]->pos.z=pos[2];

		const dReal* rot=new dReal[3];
		rot=dBodyGetRotation(level->dynamicObjects[i]->bodyId);

		level->dynamicObjects[i]->rot.x=rot[0];
		level->dynamicObjects[i]->rot.y=rot[1];
		level->dynamicObjects[i]->rot.z=rot[2];
	}
#endif
}

bool Physics::registerObject(Object* obj){

#ifdef ODE
	obj->bodyId=dBodyCreate(worldId);

	dBodySetPosition(obj->bodyId, obj->pos.x, obj->pos.y, obj->pos.z);

	dMassAdjust(&obj->ODEmass,obj->mass);
	dBodySetMass(obj->bodyId, &obj->ODEmass);

	if(obj->dynamic){
		dBodyEnable(obj->bodyId);
	}else{
		dBodyDisable(obj->bodyId);
	}

	obj->bodyIdInit=true;
#endif

	return true;
}

bool Physics::removeObject(Object* obj){
	#ifdef ODE
		obj->geomIdInit=true;
	#endif

	return true;
}


Physics::~Physics(){

#ifdef ODE
	if(worldId!=NULL){
		dWorldDestroy(worldId);
	}

	dCloseODE();
#endif
}

