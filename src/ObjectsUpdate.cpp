#include "Types.hpp"

#include <iostream>
#include "HelperLibMath.hpp"
#include "conf.h"
#include "SceneGraph.hpp"
#include "texture.hpp"
#include "textureLibrary.h"

#ifndef XSICONVERT
	#include "level.hpp"

	#include "GlHeader.hpp"
	
	#include "Log.hpp"
	#include "os.h"
#endif

#ifdef XSICONVERT
	extern Array<String> jointMap;
#endif

#include "objects.hpp"
#include "picking.h"


void Object::rebuildUVs(){
	for(int v=0; v<getVertexCount(); v++){
		getVertexRaw(v).uv0=getVertexRaw(v).uvLightmap;
		getVertexRaw(v).uv1=getVertexRaw(v).uvLightmap;
		getVertexRaw(v).uv2=getVertexRaw(v).uvLightmap;
		getVertexRaw(v).uv3=getVertexRaw(v).uvLightmap;
		getVertexRaw(v).uv4=getVertexRaw(v).uvLightmap;
		getVertexRaw(v).uv5=getVertexRaw(v).uvLightmap;
		getVertexRaw(v).uv6=getVertexRaw(v).uvLightmap;
	}

	oldTex0pos=FloatVector2d(0,0);
	oldTex0scale=FloatVector2d(1,1);
	oldTex0rot=FloatVector2d(0,0);

	oldTex1pos=FloatVector2d(0,0);
	oldTex1scale=FloatVector2d(1,1);
	oldTex1rot=FloatVector2d(0,0);

	oldTex2pos=FloatVector2d(0,0);
	oldTex2scale=FloatVector2d(1,1);
	oldTex2rot=FloatVector2d(0,0);

	oldTex3pos=FloatVector2d(0,0);
	oldTex3scale=FloatVector2d(1,1);
	oldTex3rot=FloatVector2d(0,0);

	oldTex4pos=FloatVector2d(0,0);
	oldTex4scale=FloatVector2d(1,1);
	oldTex4rot=FloatVector2d(0,0);

	oldTex5pos=FloatVector2d(0,0);
	oldTex5scale=FloatVector2d(1,1);
	oldTex5rot=FloatVector2d(0,0);

	oldTex6pos=FloatVector2d(0,0);
	oldTex6scale=FloatVector2d(1,1);
	oldTex6rot=FloatVector2d(0,0);

	dirty=true;
}

void Object::update(){

	bool aChange=false;
	dirty=false;

	#ifndef XSICONVERT

	if(vertexCount!=oldVertexCount){
		dirty=true;
		oldVertexCount=vertexCount;
	}

	if(material!=oldMaterial){
		if(level->materials[material].nonstaticRefList.find(this)==-1){
			level->materials[material].nonstaticRefList.pushBack(this);
		}

		if(oldMaterial>=0){
			int mi=level->materials[oldMaterial].nonstaticRefList.find(this);

			if(mi!=-1){
				level->materials[oldMaterial].nonstaticRefList.erase(mi);
			}
		}

		dirty=true;
		oldMaterial=material;
	}

	if(material!=-1){
		if(level->materials[material].dirty){

			dirty=true;
		}
	}else{
	#ifdef _WIN32
		DebugBreak();
		#endif
	}

	#ifdef SOFTWARE_TRANSFORM_TEXCOORDS

	FloatVector2d* texpos;
	FloatVector2d* texrot;
	FloatVector2d* texscale;

	FloatVector2d* oldtexpos;
	FloatVector2d* oldtexrot;
	FloatVector2d* oldtexscale;

	FloatVector2d* uv;

	for(int texsource=0; true; texsource++){

		if(material<0){	//just in case...
			break;
		}

		if(texsource==0){
			texpos=&level->materials[material].tex0pos;
			texrot=&level->materials[material].tex0rot;
			texscale=&level->materials[material].tex0scale;

			oldtexpos=&oldTex0pos;
			oldtexrot=&oldTex0rot;
			oldtexscale=&oldTex0scale;
		}else if(texsource==1){
			

			texpos=&level->materials[material].tex1pos;
			texrot=&level->materials[material].tex1rot;
			texscale=&level->materials[material].tex1scale;

			oldtexpos=&oldTex1pos;
			oldtexrot=&oldTex1rot;
			oldtexscale=&oldTex1scale;
			
		}else if(texsource==2){
			

			texpos=&level->materials[material].tex2pos;
			texrot=&level->materials[material].tex2rot;
			texscale=&level->materials[material].tex2scale;

			oldtexpos=&oldTex2pos;
			oldtexrot=&oldTex2rot;
			oldtexscale=&oldTex2scale;
			
		}else if(texsource==3){
			

			texpos=&level->materials[material].tex3pos;
			texrot=&level->materials[material].tex3rot;
			texscale=&level->materials[material].tex3scale;

			oldtexpos=&oldTex3pos;
			oldtexrot=&oldTex3rot;
			oldtexscale=&oldTex3scale;
			
		}else if(texsource==4){
			

			texpos=&level->materials[material].tex4pos;
			texrot=&level->materials[material].tex4rot;
			texscale=&level->materials[material].tex4scale;

			oldtexpos=&oldTex4pos;
			oldtexrot=&oldTex4rot;
			oldtexscale=&oldTex4scale;
			
		}else if(texsource==5){
			

			texpos=&level->materials[material].tex5pos;
			texrot=&level->materials[material].tex5rot;
			texscale=&level->materials[material].tex5scale;

			oldtexpos=&oldTex5pos;
			oldtexrot=&oldTex5rot;
			oldtexscale=&oldTex5scale;
			
		}else if(texsource==6){
			texpos=&level->materials[material].tex6pos;
			texrot=&level->materials[material].tex6rot;
			texscale=&level->materials[material].tex6scale;

			oldtexpos=&oldTex6pos;
			oldtexrot=&oldTex6rot;
			oldtexscale=&oldTex6scale;
			
		}else if(texsource==7){
			texpos=&level->materials[material].texLightmapPos;
			texrot=&level->materials[material].texLightmapRot;
			texscale=&level->materials[material].texLightmapScale;

			oldtexpos=&oldTexLightmapPos;
			oldtexrot=&oldTexLightmapRot;
			oldtexscale=&oldTexLightmapScale;
			
		}else{
			break;
		}

		if(*oldtexpos!=*texpos){
			FloatVector2d change=*texpos-*oldtexpos;

			
			for(unsigned int i=0; i<vertexCount; i++){

				if(texsource==0){
					uv=&vertices[i].uv0;
				}else if(texsource==1){
					uv=&vertices[i].uv1;
				}else if(texsource==2){
					uv=&vertices[i].uv2;
				}else if(texsource==3){
					uv=&vertices[i].uv3;
				}else if(texsource==4){
					uv=&vertices[i].uv4;
				}else if(texsource==5){
					uv=&vertices[i].uv5;
				}else if(texsource==6){
					uv=&vertices[i].uv6;
				}else if(texsource==7){
					uv=&vertices[i].uvLightmap;
				}else{
					os().error("BAD TEX SOURCE");
				}

				*uv+=change;

			}

			*oldtexpos=*texpos;

			aChange=true;
		}

		if(*oldtexscale!=*texscale){
			FloatVector2d change;

			//this is tricky... to deal with 0 values, we'll replace 0 with a very small number. it should act basically the same and preserve the coords

			if(oldtexscale->x==0){oldtexscale->x=0.0000001;}
			if(oldtexscale->y==0){oldtexscale->y=0.0000001;}
			if(texscale->x==0){texscale->x=0.0000001;}
			if(texscale->y==0){texscale->y=0.0000001;}


			change=*texscale/(*oldtexscale);

			if( (change.x!=1 || change.y!=1) && (change.x!=0) && (change.y!=0)){

				for(unsigned int i=0; i<vertexCount; i++){

					if(texsource==0){
						uv=&vertices[i].uv0;
					}else if(texsource==1){
						uv=&vertices[i].uv1;
					}else if(texsource==2){
						uv=&vertices[i].uv2;
					}else if(texsource==3){
						uv=&vertices[i].uv3;
					}else if(texsource==4){
						uv=&vertices[i].uv4;
					}else if(texsource==5){
						uv=&vertices[i].uv5;
					}else if(texsource==6){
						uv=&vertices[i].uv6;
					}else if(texsource==7){
						uv=&vertices[i].uvLightmap;
					}else{
						os().error("BAD TEX SOURCE");
					}

					
					if(uv->x<0){

						uv->x*=-change.x;
					}else{

						uv->x*=change.x;
					}
		
					if(uv->y<0){

						uv->y*=-change.y;
					}else{

						uv->y*=change.y;
					}
					

				}
			}

			*oldtexscale=*texscale;

			aChange=true;
		}

		if(*oldtexrot!=*texrot){
			FloatVector2d change=*texrot-*oldtexrot;
			*oldtexrot=*texrot;
			aChange=true;
		}
	}
 
	#endif

	//check insidebb

	if(flags & OBJECT_INSIDEBB){


		if( (level->camera->pos.x>box.nx  &&  level->camera->pos.x<box.px) &&
				(level->camera->pos.y>box.ny  &&  level->camera->pos.y<box.py) &&
				(level->camera->pos.z>box.nz  &&  level->camera->pos.z<box.pz)){

			insidebb=true;
		}else{
			insidebb=false;
		}
	}

	#endif

	#ifdef SOFTWARE_TRANSFORMS

	if(pos!=oldPos){
		FloatVector3d change=pos-oldPos;

		for(unsigned int i=0; i<vertexCount; i++){
			vertices[i].pos+=change;

		}

		oldPos=pos;

		aChange=true;

	}

	if(scale!=oldScale){
		FloatVector3d change=scale-oldScale;

		for(unsigned int i=0; i<vertexCount; i++){
			//convert
			vertices[i].pos.x-=pos.x;
			vertices[i].pos.y-=pos.y;
			vertices[i].pos.z-=pos.z;

			vertices[i].pos.x*=change.x;
			vertices[i].pos.y*=change.y;
			vertices[i].pos.z*=change.z;

			//unconvert
			vertices[i].pos.x+=pos.x;
			vertices[i].pos.y+=pos.y;
			vertices[i].pos.z+=pos.z;
		}

		oldScale=scale;

		aChange=true;

	}

	if(rot!=oldRot){
		FloatVector3d changeRot=rot-oldRot;

		if(changeRot.x!=0){
			float dist;	
			float ndist;
			FloatVector2d v1;
			FloatVector2d v2;
			FloatVector2d nv1;
			FloatVector2d nv2;
			float normalAngleNew=0;
			float normalAngle=0;

			float angleNew=0;
			float angle=0;

			for(unsigned int i=0; i<vertexCount; i++){
				//convert
				vertices[i].pos.x-=pos.x;
				vertices[i].pos.y-=pos.y;
				vertices[i].pos.z-=pos.z;
				
				v1.x=0;
				v1.y=-1;	//must be -1 for some reason
				
				v2.x=vertices[i].pos.z;
				v2.y=vertices[i].pos.y;
					
				angle=math.vectorToAngle(v1,v2);
				angleNew=angle+changeRot.x;

				nv1.x=0;
				nv1.y=-1;	//must be -1 for some reason
				

				nv2.x=vertices[i].normal.z;
				nv2.y=vertices[i].normal.y;
					
				normalAngle=math.vectorToAngle(nv1,nv2);
				normalAngleNew=normalAngle+changeRot.x;

				dist=math.distance(0,0,vertices[i].pos.z,vertices[i].pos.y);

				ndist=math.distance(0,0,vertices[i].normal.z,vertices[i].normal.y);

				if(vertices[i].pos.z!=0 || vertices[i].pos.y!=0){
					vertices[i].pos.z=(- cos((angleNew-90) * PI/180) * dist);	
					vertices[i].pos.y=(sin((angleNew-90) * PI/180) * dist);	
				}

				if(vertices[i].normal.z!=0 || vertices[i].normal.y!=0){
					vertices[i].normal.z=(- cos((normalAngleNew-90) * PI/180) *ndist);
					vertices[i].normal.y=(sin((normalAngleNew-90) * PI/180) * ndist);
				}

				//unconvert
				vertices[i].pos.x+=pos.x;
				vertices[i].pos.y+=pos.y;
				vertices[i].pos.z+=pos.z;

				
			}

						
		}

		if(changeRot.y!=0){
				
			float dist;	
			float ndist;	
			FloatVector2d v1;
			FloatVector2d v2;
			FloatVector2d nv1;
			FloatVector2d nv2;
			float normalAngleNew=0;
			float normalAngle=0;

			float angleNew=0;
			float angle=0;

			for(unsigned int i=0; i<vertexCount; i++){
				//convert
				vertices[i].pos.x-=pos.x;
				vertices[i].pos.y-=pos.y;
				vertices[i].pos.z-=pos.z;
				
				v1.x=0;
				v1.y=-1;	//must be -1 for some reason
				
				v2.x=vertices[i].pos.x;
				v2.y=vertices[i].pos.z;
					
				angle=math.vectorToAngle(v1,v2);
				angleNew=angle+changeRot.y;

				nv1.x=0;
				nv1.y=-1;	//must be -1 for some reason
				

				nv2.x=vertices[i].normal.x;
				nv2.y=vertices[i].normal.z;
					
				normalAngle=math.vectorToAngle(nv1,nv2);
				normalAngleNew=normalAngle+changeRot.y;

				dist=math.distance(0,0,vertices[i].pos.x,vertices[i].pos.z);

				ndist=math.distance(0,0,vertices[i].normal.x,vertices[i].normal.z);

				if(vertices[i].pos.x!=0 || vertices[i].pos.z!=0){
					vertices[i].pos.x=(- cos((angleNew-90) * PI/180) * dist);
					vertices[i].pos.z=(sin((angleNew-90) * PI/180) * dist);
				}

				if(vertices[i].normal.x!=0 || vertices[i].normal.z!=0){
					vertices[i].normal.x=(- cos((normalAngleNew-90) * PI/180) * ndist);
					vertices[i].normal.z=(sin((normalAngleNew-90) * PI/180) * ndist);
				}

				//unconvert
				vertices[i].pos.x+=pos.x;
				vertices[i].pos.y+=pos.y;
				vertices[i].pos.z+=pos.z;
			}
		}

		if(changeRot.z!=0){
			float dist;	
			float ndist;
			FloatVector2d v1;
			FloatVector2d v2;
			FloatVector2d nv1;
			FloatVector2d nv2;
			float normalAngleNew=0;
			float normalAngle=0;

			float angleNew=0;
			float angle=0;

			for(unsigned int i=0; i<vertexCount; i++){
				//convert
				vertices[i].pos.x-=pos.x;
				vertices[i].pos.y-=pos.y;
				vertices[i].pos.z-=pos.z;
				
				v1.x=0;
				v1.y=-1;	//must be -1 for some reason
				
				v2.x=vertices[i].pos.y;
				v2.y=vertices[i].pos.x;
					
				angle=math.vectorToAngle(v1,v2);
				angleNew=angle+changeRot.z;

				nv1.x=0;
				nv1.y=-1;	//must be -1 for some reason
				

				nv2.x=vertices[i].normal.y;
				nv2.y=vertices[i].normal.x;
					
				normalAngle=math.vectorToAngle(nv1,nv2);
				normalAngleNew=normalAngle+changeRot.z;

				

				dist=math.distance(0,0,vertices[i].pos.x,vertices[i].pos.y);

				ndist=math.distance(0,0,vertices[i].normal.x,vertices[i].normal.y);

				if(vertices[i].pos.y!=0 || vertices[i].pos.x!=0){
					vertices[i].pos.y=(float)(- cos((angleNew-90) * PI/180) * dist);
					vertices[i].pos.x=(float)(sin((angleNew-90) * PI/180) * dist);
				}

				if(vertices[i].normal.y!=0 || vertices[i].normal.x!=0){
					vertices[i].normal.y=(float)(- cos((normalAngleNew-90) * PI/180) * ndist);
					vertices[i].normal.x=(float)(sin((normalAngleNew-90) * PI/180) * ndist);
				}

				//unconvert
				vertices[i].pos.x+=pos.x;
				vertices[i].pos.y+=pos.y;
				vertices[i].pos.z+=pos.z;
			}
		}
					
		oldRot=rot;
		aChange=true;
	}
	#else
		#ifndef XSICONVERT

		bool boxRebuilt=false;

		if(!boxBuilt){
			buildBox();
			boxBuilt=true;
			boxRebuilt=true;
		}

		for(int i=0; i<3; i++){
			FloatVector2d changePos;
			Box2d* box;
			FloatVector2d* old;
			FloatVector2d* newO;

			if(i==0){
				changePos=level->materials[material].tex0pos-oldTex0pos;
				box=&uv0Box;
				old=&oldTex0pos;
				newO=&level->materials[material].tex0pos;
			}else if(i==1){
				changePos=level->materials[material].tex1pos-oldTex1pos;
				box=&uv1Box;
				old=&oldTex1pos;
				newO=&level->materials[material].tex1pos;
			}else if(i==2){
				changePos=level->materials[material].tex2pos-oldTex2pos;
				box=&uv2Box;
				old=&oldTex2pos;
				newO=&level->materials[material].tex2pos;
			}

			if(*old!=*newO){
				box->nx=box->nx+changePos.x;
				box->px=box->px+changePos.x;

				box->ny=box->ny+changePos.y;
				box->py=box->py+changePos.y;

				*old=*newO;
			}
		}

		if(scale!=oldScale || pos!=oldPos || rot!=oldRot || boxRebuilt){
			Matrix4d4d m;

			m.loadIdentity();
			m.translate(pos.x,pos.y,pos.z);
			m.rotateZ(rot.z);
			m.rotateY(rot.y);
			m.rotateX(rot.x);
			m.scale(scale.x,scale.y,scale.z);

			box.nx=1000000;
			box.px=-1000000;

			box.ny=1000000;
			box.py=-1000000;

			box.nz=1000000;
			box.pz=-1000000;

			FloatVector4d p;
			for(int b=0; b<8; b++){

				if(b==0){
					p=m*FloatVector4d(rawBox.nx,rawBox.ny,rawBox.nz,1);
				}else if(b==1){
					p=m*FloatVector4d(rawBox.px,rawBox.ny,rawBox.nz,1);
				}else if(b==2){
					p=m*FloatVector4d(rawBox.nx,rawBox.ny,rawBox.pz,1);
				}else if(b==3){
					p=m*FloatVector4d(rawBox.px,rawBox.ny,rawBox.pz,1);
				}else if(b==4){
					p=m*FloatVector4d(rawBox.nx,rawBox.py,rawBox.nz,1);
				}else if(b==5){
					p=m*FloatVector4d(rawBox.px,rawBox.py,rawBox.nz,1);
				}else if(b==6){
					p=m*FloatVector4d(rawBox.nx,rawBox.py,rawBox.pz,1);
				}else if(b==7){
					p=m*FloatVector4d(rawBox.px,rawBox.py,rawBox.pz,1);
				}

				if(p.x<box.nx){box.nx=p.x;}
				if(p.x>box.px){box.px=p.x;}

				if(p.y<box.ny){box.ny=p.y;}
				if(p.y>box.py){box.py=p.y;}

				if(p.z<box.nz){box.nz=p.z;}
				if(p.z>box.pz){box.pz=p.z;}
			}
		}

		if(pos!=oldPos || scale!=oldScale || rot!=oldRot){
			rebuildTransformedVertexCache=true;
		}

		oldPos=pos;
		oldScale=scale;
		oldRot=rot;

		#endif
	#endif

	if(aChange){
		dirty=true;

		//http://www.terathon.com/code/tangent.html
		//2/26/05 - I'm pretty sure this works fine.  look up stuff about plane equasions to calculate tangent space
		//

		FloatVector3d tangent(0,0,0);
		FloatVector3d binormal(0,0,0);

		FloatVector3d tt;

		#ifdef SOFTWARE_TRANSFORMS
		for(unsigned int i=0; i<vertexCount; i+=3){
			
			const FloatVector3d& v1 = vertices[i].pos;
			const FloatVector3d& v2 = vertices[i+1].pos;
			const FloatVector3d& v3 = vertices[i+2].pos;
			
			const FloatVector2d& w1 = vertices[i].uv0;
			const FloatVector2d& w2 = vertices[i+1].uv0;
			const FloatVector2d& w3 = vertices[i+2].uv0;
			
			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;
			
			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;
			

			vertices[i].normal.normalize();
			vertices[i+1].normal.normalize();
			vertices[i+2].normal.normalize();

			float r = 1.0F / (s1 * t2 - s2 * t1);
			FloatVector3d sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
					(t2 * z1 - t1 * z2) * r);
			FloatVector3d tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
					(s1 * z2 - s2 * z1) * r);
			
	

			tt = (sdir - vertices[i].normal * (vertices[i].normal * sdir)).normalize();
			vertices[i].tangent=tt;
			tt = (sdir - vertices[i+1].normal * (vertices[i+1].normal * sdir)).normalize();
			vertices[i+1].tangent=tt;
			tt = (sdir - vertices[i+2].normal * (vertices[i+2].normal * sdir)).normalize();
			vertices[i+2].tangent=tt;
			


			//see nvidia bump_skin.pdf pg 20.  need to invert if world/texture opp, ie calculate handedness
			//TANGENT IS EVIL!!!!

			float hand=1;

			hand=(vertices[i].normal % sdir * tdir < 0.0F) ? -1.0F : 1.0F;

			vertices[i].binormal=math.crossProduct(vertices[i].normal,vertices[i].tangent)*hand;

			hand=(vertices[i+1].normal % sdir * tdir < 0.0F) ? -1.0F : 1.0F;

			vertices[i+1].binormal=math.crossProduct(vertices[i+1].normal,vertices[i+1].tangent)*hand;

			hand=(vertices[i+2].normal % sdir * tdir < 0.0F) ? -1.0F : 1.0F;

			vertices[i+2].binormal=math.crossProduct(vertices[i+2].normal,vertices[i+2].tangent)*hand;


			vertices[i].binormal.normalize();
			vertices[i+1].binormal.normalize();
			vertices[i+2].binormal.normalize();

			vertices[i].tangent.normalize();
			vertices[i+1].tangent.normalize();
			vertices[i+2].tangent.normalize();

			
		}
		#endif

		#ifdef SOFTWARE_TRANSFORMS
			buildBox();
		#endif
	}

	#ifndef XSICONVERT
	bool x=(flags & OBJECT_COLLIDE);

	if(x!=collide){
		flags=flags^OBJECT_COLLIDE;
	}



	if((flags & OBJECT_COLLIDE) && !oldCollide){
		int i=level->colliders.find(this);
		int j=level->dynamicObjects.find(this);

		if(j==-1){
			physics.registerObject(this);
		}

		if(i==-1){
			level->colliders.pushBack(this);
		}

		oldCollide=true;
	}

	if(!(flags & OBJECT_COLLIDE) && oldCollide){

		int i=level->colliders.find(this);
		int j=level->dynamicObjects.find(this);

		if(i>-1){
			level->colliders.erase(i);
		}

		if(j==-1){
			physics.removeObject(this);
		}


		oldCollide=false;
	}

	bool v=(flags & OBJECT_VISIBLE);

	if(v!=visible){
		flags=flags^OBJECT_VISIBLE;
	}

	
	x=(flags & OBJECT_DYNAMIC);

	if(x!=dynamic){
		flags=flags;
		flags=flags^OBJECT_DYNAMIC;
		flags=flags;
	}



	if((flags & OBJECT_DYNAMIC) && !oldDynamic){
		int i=level->dynamicObjects.find(this);
		int j=level->colliders.find(this);

		if(j==-1){
			physics.registerObject(this);
		}

		if(i==-1){
			level->dynamicObjects.pushBack(this);
		}

		oldDynamic=true;
	}

	if(!(flags & OBJECT_DYNAMIC) && oldDynamic){
		int i=level->dynamicObjects.find(this);
		int j=level->colliders.find(this);

		if(j==-1){
			physics.removeObject(this);
		}

		if(i>-1){

			level->dynamicObjects.erase(i);
		}

		oldDynamic=false;
	}

	if(material!=-1){
		if(level->materials[material].textureEnvironmentFront!=-1 && (level->materials[material].shader & MATERIAL_SHADER_ENVIRONMENT)){
			//environment mapped objects need to be redrawn every frame so that the texcoords update

			//dirty=true;
		}
	}

	if(envelopeVertices==NULL && envelopes.size()>0){

		if(envelopes.size()>ikSplit){
 
			Array<Array<Vertex*> > vertexLists;
			Array<Array<int> > vertexEnvLists;

			Array<int> envList;
			Array<int> notfound;

			for(int i=0; i<vertices.size(); i+=3){
				envList.clear();

				for(int j=0; j<envelopes.size(); j++){
					if(vertices[i].envelope[j]>0 && envList.find(j)==-1){
						envList.pushBack(j);
					}
				}

				for(int j=0; j<envelopes.size(); j++){
					if(vertices[i+1].envelope[j]>0 && envList.find(j)==-1){
						envList.pushBack(j);
					}
				}

				for(int j=0; j<envelopes.size(); j++){
					if(vertices[i+2].envelope[j]>0 && envList.find(j)==-1){
						envList.pushBack(j);
					}
				}

				
				int appendNoExpand=-1;
				int appendExpand=-1;

				for(int j=0; j<vertexEnvLists.size(); j++){

					notfound.clear();

					for(int l=0; l<envList.size(); l++){
						if(vertexEnvLists[j].find(envList[l])==-1){
							notfound.pushBack(envList[l]);
						}
					}

					if(notfound.size()==0){
						appendNoExpand=j;
						break;
					}else{

						if( (notfound.size()+vertexEnvLists[j].size()) <= ikSplit){

							appendExpand=j;
						}
					}
				}

				if(appendNoExpand!=-1){
					vertexLists[appendNoExpand].pushBack(&vertices[i]);
					vertexLists[appendNoExpand].pushBack(&vertices[i+1]);
					vertexLists[appendNoExpand].pushBack(&vertices[i+2]);
				}else if(appendExpand!=-1){
					vertexLists[appendExpand].pushBack(&vertices[i]);
					vertexLists[appendExpand].pushBack(&vertices[i+1]);
					vertexLists[appendExpand].pushBack(&vertices[i+2]);

					for(int r=0; r<notfound.size(); r++){
						vertexEnvLists[appendExpand].pushBack(notfound[r]);
					}
				}else{
					vertexLists.pushBack();
					vertexLists[vertexLists.size()-1].pushBack(&vertices[i]);
					vertexLists[vertexLists.size()-1].pushBack(&vertices[i+1]);
					vertexLists[vertexLists.size()-1].pushBack(&vertices[i+2]);
					vertexEnvLists.pushBack();

					for(int p=0; p<envList.size(); p++){
						vertexEnvLists[vertexEnvLists.size()-1].pushBack(envList[p]);
					}
				}
			}

			
			envelopeVertices=new Array<Vertex*>[vertexLists.size()];
			envelopeVerticesEnvelopes=new Array<int>[vertexLists.size()];
			envelopeVerticesCount=vertexLists.size();

			for(int i=0; i<vertexLists.size(); i++){
				vertexLists[i][0]->end=START_TRI;
				vertexLists[i][vertexLists[i].size()-1]->end=END_TRI;

				//do a stupid sort on the envLists

				bool done=false;

				while(!done){
					done=true;

					for(int k=1; k < vertexEnvLists[i].size(); k++){
						if(vertexEnvLists[i][k]<vertexEnvLists[i][k-1]){
							int t=vertexEnvLists[i][k-1];
							vertexEnvLists[i][k-1]=vertexEnvLists[i][k];
							vertexEnvLists[i][k]=t;
							done=false;
						}
					}
				}

				envelopeVertices[i]=vertexLists[i];
				envelopeVerticesEnvelopes[i]=vertexEnvLists[i];
			}
		}
	}

	#endif
}
