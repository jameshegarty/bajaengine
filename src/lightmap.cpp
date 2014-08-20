
#ifdef DEVELOPER

#include "lightmap.h"
#include "bmp.hpp"
#include "level.hpp"
#include "Helperlib/HelperLibMath.hpp"
#include "texture.hpp"
#include "conf.h"
#include "collision/vectormath.hpp"
#include "draw.hpp"

Lightmap lightmap;

Map<String,LightShadowVolume> shadowVolumes;	//<light name, volume obj>

void generateShadowVolumes(){

	for(int i=0; i<level->lights.size(); i++){
		if(!shadowVolumes[level->lights[i]->name].built){

			for(int j=0; j<level->objects.size(); j++){
				Array<Ray3d> edges;

				for(int t=0; t<level->objects[j]->getVertexCount(); t+=3){

					

					if(math.dotProduct( (level->lights[i]->pos-level->objects[j]->getVertex(t).pos).normalized()  ,level->objects[j]->getVertex(t).normal)>0 ||
						math.dotProduct( (level->lights[i]->pos-level->objects[j]->getVertex(t+1).pos).normalized()  ,level->objects[j]->getVertex(t+1).normal)>0 ||
						math.dotProduct( (level->lights[i]->pos-level->objects[j]->getVertex(t+2).pos).normalized()  ,level->objects[j]->getVertex(t+2).normal)>0){

						//this tri is illuminated, so add it to the stack
						//check 0,1 1,2 2,0
						int f=-1;
						for(int iter=0; iter<edges.size(); iter++){
							if(edges[iter].start==level->objects[j]->getVertex(t).pos && 
								edges[iter].end==level->objects[j]->getVertex(t+1).pos){
								f=iter;
								break;
							}

							if(edges[iter].start==level->objects[j]->getVertex(t+1).pos && 
								edges[iter].end==level->objects[j]->getVertex(t).pos){
								f=iter;
								break;
							}
						}

						if(f!=-1){
							edges.erase(f);
						}else{
							Ray3d r;
							r.start=level->objects[j]->getVertex(t).pos;
							r.end=level->objects[j]->getVertex(t+1).pos;
							edges.pushBack(r);
						}	

						////////////////////////////////////////////////////////////////////
						f=-1;
						for(int iter=0; iter<edges.size(); iter++){
							if(edges[iter].start==level->objects[j]->getVertex(t+1).pos && 
								edges[iter].end==level->objects[j]->getVertex(t+2).pos){
								f=iter;
								break;
							}

							if(edges[iter].start==level->objects[j]->getVertex(t+2).pos && 
								edges[iter].end==level->objects[j]->getVertex(t+1).pos){
								f=iter;
								break;
							}
						}

						if(f!=-1){
							edges.erase(f);
						}else{
							Ray3d r;
							r.start=level->objects[j]->getVertex(t+1).pos;
							r.end=level->objects[j]->getVertex(t+2).pos;
							edges.pushBack(r);
						}	

						////////////////////////////////////////////////////////////////////
						f=-1;
						for(int iter=0; iter<edges.size(); iter++){
							if(edges[iter].start==level->objects[j]->getVertex(t+2).pos && 
								edges[iter].end==level->objects[j]->getVertex(t).pos){
								f=iter;
								break;
							}

							if(edges[iter].start==level->objects[j]->getVertex(t).pos && 
								edges[iter].end==level->objects[j]->getVertex(t+2).pos){
								f=iter;
								break;
							}
						}

						if(f!=-1){
							edges.erase(f);
						}else{
							Ray3d r;
							r.start=level->objects[j]->getVertex(t+2).pos;
							r.end=level->objects[j]->getVertex(t).pos;
							edges.pushBack(r);
						}	
					}
				}

				//ok, now we have our silouette edges in 'edges'

				shadowVolumes[level->lights[i]->name].objects.pushBack();

				shadowVolumes[level->lights[i]->name].objects[j].name=level->objects[j]->name;
				shadowVolumes[level->lights[i]->name].objects[j].edgesStore=edges;

				for(int e=0; e<edges.size(); e++){

					Plane3d pl;
					pl.x=edges[e].start.x;
					pl.y=edges[e].start.y;
					pl.z=edges[e].start.z;
					FloatVector3d r1=edges[e].end-edges[e].start;
					r1.normalize();
					FloatVector3d r2=edges[e].start-level->lights[i]->pos;
					r2.normalize();
					pl.normal=math.crossProduct(r1,r2);

					shadowVolumes[level->lights[i]->name].objects[j].planes.pushBack(pl);
				}
			}

			shadowVolumes[level->lights[i]->name].built=true;
		}
	}

}

float castRay(FloatVector3d& normal, FloatVector3d& point,Vertex& tp1, Vertex& tp2, Vertex& tp3){

	FloatVector3d pnormal=math.crossProduct((tp1.pos-tp2.pos).normalized(),(tp3.pos-tp2.pos).normalized()).normalized();

	float d=intersectRayPlane(point,normal,tp1.pos,pnormal);

	if(d>0){
		FloatVector3d ipoint=point+normal*d;
		
		if(CheckPointInTriangle(ipoint,tp1.pos,tp2.pos,tp3.pos)){
			return d;
		}
	}

	return 0;
}


bool castRayF(FloatVector3d& normal, FloatVector3d& point,FloatVector3d tp1, FloatVector3d tp2, FloatVector3d tp3){

	FloatVector3d pnormal=math.crossProduct((tp1-tp2).normalized(),(tp3-tp2).normalized()).normalized();

	float d=intersectRayPlane(point,normal,tp1,pnormal);

	if(d>0){
		FloatVector3d ipoint=point+normal*d;
		if(CheckPointInTriangle(ipoint,tp1,tp2,tp3)){
			return true;
		}
	}

	return false;
}

bool boxIntersect(Object* o,FloatVector3d& p,FloatVector3d& rn){

	if(o->getVertexCount()<24){return true;}	//it'll just be faster to test the verts lol

	Matrix4d4d m;
	m.loadIdentity();
	m.translate(o->pos.x,o->pos.y,o->pos.z);
	m.rotateZ(o->rot.z);
	m.rotateY(o->rot.y);
	m.rotateX(o->rot.x);
	m.scale(o->scale.x,o->scale.y,o->scale.z);
	
	FloatVector3d v1;
	FloatVector4d z1=FloatVector4d(o->rawBox.nx,o->rawBox.py,o->rawBox.pz,1);
	v1.x=math.dotProduct(z1,m.column(0));
	v1.y=math.dotProduct(z1,m.column(1));
	v1.z=math.dotProduct(z1,m.column(2));

	FloatVector3d v2;
	FloatVector4d z2=FloatVector4d(o->rawBox.px,o->rawBox.py,o->rawBox.pz,1);
	v2.x=math.dotProduct(z2,m.column(0));
	v2.y=math.dotProduct(z2,m.column(1));
	v2.z=math.dotProduct(z2,m.column(2));

	FloatVector3d v3;
	FloatVector4d z3=FloatVector4d(o->rawBox.px,o->rawBox.py,o->rawBox.nz,1);
	v3.x=math.dotProduct(z3,m.column(0));
	v3.y=math.dotProduct(z3,m.column(1));
	v3.z=math.dotProduct(z3,m.column(2));

	FloatVector3d v4;
	FloatVector4d z4=FloatVector4d(o->rawBox.nx,o->rawBox.py,o->rawBox.nz,1);
	v4.x=math.dotProduct(z4,m.column(0));
	v4.y=math.dotProduct(z4,m.column(1));
	v4.z=math.dotProduct(z4,m.column(2));

	FloatVector3d v5;
	FloatVector4d z5=FloatVector4d(o->rawBox.nx,o->rawBox.ny,o->rawBox.pz,1);
	v5.x=math.dotProduct(z5,m.column(0));
	v5.y=math.dotProduct(z5,m.column(1));
	v5.z=math.dotProduct(z5,m.column(2));

	FloatVector3d v6;
	FloatVector4d z6=FloatVector4d(o->rawBox.px,o->rawBox.ny,o->rawBox.pz,1);
	v6.x=math.dotProduct(z6,m.column(0));
	v6.y=math.dotProduct(z6,m.column(1));
	v6.z=math.dotProduct(z6,m.column(2));

	FloatVector3d v7;
	FloatVector4d z7=FloatVector4d(o->rawBox.px,o->rawBox.ny,o->rawBox.nz,1);
	v7.x=math.dotProduct(z7,m.column(0));
	v7.y=math.dotProduct(z7,m.column(1));
	v7.z=math.dotProduct(z7,m.column(2));

	FloatVector3d v8;
	FloatVector4d z8=FloatVector4d(o->rawBox.nx,o->rawBox.ny,o->rawBox.nz,1);
	v8.x=math.dotProduct(z8,m.column(0));
	v8.y=math.dotProduct(z8,m.column(1));
	v8.z=math.dotProduct(z8,m.column(2));

	if(castRayF(rn,p,v1,v2,v3)){return true;}
	if(castRayF(rn,p,v1,v3,v4)){return true;}

	if(castRayF(rn,p,v2,v6,v7)){return true;}
	if(castRayF(rn,p,v2,v7,v3)){return true;}

	if(castRayF(rn,p,v3,v7,v8)){return true;}
	if(castRayF(rn,p,v3,v8,v4)){return true;}

	if(castRayF(rn,p,v4,v8,v5)){return true;}
	if(castRayF(rn,p,v4,v5,v1)){return true;}

	if(castRayF(rn,p,v5,v2,v1)){return true;}
	if(castRayF(rn,p,v5,v6,v2)){return true;}

	if(castRayF(rn,p,v6,v7,v8)){return true;}
	if(castRayF(rn,p,v6,v8,v5)){return true;}

	return false;
}

#define OPTI_LIST 1

#ifdef OPTI_LIST
int objOptiListCnt;
Object** objOptiList;
int* vertOptiListCnt;
Vertex** vertOptiList;

void genOptiList(Light* l,String name){
	objOptiListCnt=0;

	for(int i=0; i<level->objects.size(); i++){
		if(level->objects[i]->visible && level->objects[i]->name!=name){
			if((level->objects[i]->pos-l->pos).magnitude()-level->objects[i]->box.magnitude() <l->falloffEnd){
				objOptiListCnt++;
			}
		}
	}

	objOptiList=new Object*[objOptiListCnt];
	vertOptiListCnt=new int[objOptiListCnt];
	vertOptiList=new Vertex*[objOptiListCnt];

	int currObj=0;

	for(int i=0; i<level->objects.size(); i++){
		if(level->objects[i]->visible && level->objects[i]->name!=name){
			if((level->objects[i]->pos-l->pos).magnitude()-level->objects[i]->box.magnitude() <l->falloffEnd){
				objOptiList[currObj]=level->objects[i];
				vertOptiListCnt[currObj]=0;
				Array<Vertex> tvs;

				for(int v=0; v<level->objects[i]->getVertexCount(); v+=3){

					FloatVector3d dot1=(l->pos - level->objects[i]->getVertex(v).pos).normalize();
					FloatVector3d dot2=(l->pos - level->objects[i]->getVertex(v+1).pos).normalize();
					FloatVector3d dot3=(l->pos - level->objects[i]->getVertex(v+2).pos).normalize();

					float final1=math.dotProduct(dot1,level->objects[i]->getVertex(v).normal);
					float final2=math.dotProduct(dot2,level->objects[i]->getVertex(v+1).normal);
					float final3=math.dotProduct(dot3,level->objects[i]->getVertex(v+2).normal);

					if(final1>0 || final2>0 || final3>0){
						tvs.pushBack(level->objects[i]->getVertex(v));
						tvs.pushBack(level->objects[i]->getVertex(v+1));
						tvs.pushBack(level->objects[i]->getVertex(v+2));
						vertOptiListCnt[currObj]+=3;
					}
				}

				vertOptiList[currObj]=new Vertex[vertOptiListCnt[currObj]];

				for(int v=0; v<vertOptiListCnt[currObj]; v++){
					vertOptiList[currObj][v]=tvs[v];
				}

				currObj++;
			}

			
		}
	}
}

void delOptiList(){

	
	delete[] objOptiList;

	for(int i=0; i<objOptiListCnt; i++){
		delete[] vertOptiList[i];
	}

	delete[] vertOptiList;
}

#endif

float getShadowOcclusion(Light* l,FloatVector3d& p,String name){	//returns a 0-1 value to multiply illumination with

	FloatVector3d rn=l->pos-p;

	rn.normalize();

	float dist=0;

#ifndef OPTI_LIST
	for(int i=0; i<level->objects.size(); i++){
		if(level->objects[i]->visible && level->objects[i]->name!=name){
			if((level->objects[i]->pos-l->pos).magnitude()-level->objects[i]->box.magnitude() <l->falloffEnd){

				if(boxIntersect(level->objects[i],p,rn)){
					for(int j=0; j<level->objects[i]->getVertexCount(); j+=3){
					
						dist=castRay(rn,p,level->objects[i]->getVertex(j),level->objects[i]->getVertex(j+1),level->objects[i]->getVertex(j+2));

						if(dist>0){
							//we hit something, the game is up
							
							float distToLight=(l->pos-p).magnitude();
							if(dist<distToLight){	//check if the ray has passed the light
								return 0;
							}
						}

					}
				}
			}
		}else{

		}
	}

#else
	for(int i=0; i<objOptiListCnt; i++){
		if(boxIntersect(objOptiList[i],p,rn)){

			for(int v=0; v<vertOptiListCnt[i]; v+=3){
		
				dist=castRay(rn,p,vertOptiList[i][v],vertOptiList[i][v+1],vertOptiList[i][v+2]);

				if(dist>0){
					//we hit something, the game is up
					
					float distToLight=(l->pos-p).magnitude();
					if(dist<distToLight){	//check if the ray has passed the light
						return 0;
					}
				}
			}
		
		}
	}
#endif

	return 1;
}

#define SHADOW_SCALE 4
float getShadowOcclusion(int& lindex,IntVector2d& p,int sx, int sy,unsigned char** imagedataL){	//returns a 0-1 value to multiply illumination with

	sx/=SHADOW_SCALE;
	sy/=SHADOW_SCALE;

	int nx=math.floor(float(p.x)/float(SHADOW_SCALE));
	int ny=math.floor(float(p.y)/float(SHADOW_SCALE));
	int px=math.ceil(float(p.x)/float(SHADOW_SCALE));
	int py=math.ceil(float(p.y)/float(SHADOW_SCALE));

	float xw=(float(p.x)/float(SHADOW_SCALE))-nx;
	float yw=(float(p.y)/float(SHADOW_SCALE))-ny;

	float p1=float(imagedataL[lindex][ny*sx+nx])/float(255);
	float p2=float(imagedataL[lindex][ny*sx+px])/float(255);
	float p3=float(imagedataL[lindex][py*sx+nx])/float(255);
	float p4=float(imagedataL[lindex][py*sx+px])/float(255);

	float f=p1*(1-xw)*(1-yw)+p2*(xw)*(1-yw)+p3*(1-xw)*(yw)+p4*(xw)*(yw);
	
	f=f*(float(1)-level->lights[lindex]->umbra);
	f+=level->lights[lindex]->umbra;
	return f;
}

void drawPoint(FloatVector3d pos, FloatVector3d normal,IntVector2d apos,int sx, int sy,unsigned char* imagedata,unsigned char** imagedataL){
	FloatVector3d c;

	for(int l=0; l<level->lights.size(); l++){
		float final=1;

		if(level->lights[l]->type==LIGHT_POINT){
			FloatVector3d dot=level->lights[l]->pos-pos;

			dot.normalize();

			final=math.dotProduct(dot,normal.normalized());
			if(final<0){final=0;}

			float dist=FloatVector3d(level->lights[l]->pos-pos).magnitude();

			if(dist<level->lights[l]->falloffStart){
				
			}else{
				dist-=level->lights[l]->falloffStart;

				dist=dist/(level->lights[l]->falloffEnd-level->lights[l]->falloffStart);

				if(dist>1){
					final=0;
				}else{
					final*=(float(1)-dist);
				}
			}

		}else if(level->lights[l]->type==LIGHT_SPOT){
			FloatVector3d dot=level->lights[l]->pos-pos;

			dot.normalize();

			final=math.dotProduct(dot,normal.normalized());

			if(final<0){final=0;}

			FloatVector3d r=math.degreesToNormal(level->lights[l]->rot);

			if(math.dotProduct(-dot,r) > math.cosDegrees(level->lights[l]->coneAngle*0.5)){
				final*=math.pow(math.dotProduct(-dot,r),level->lights[l]->coneSpread/float(10));
			}else{
				final=0;
			}

			float dist=FloatVector3d(level->lights[l]->pos-pos).magnitude();

			if(dist<level->lights[l]->falloffStart){
				
			}else{
				dist-=level->lights[l]->falloffStart;

				dist=dist/(level->lights[l]->falloffEnd-level->lights[l]->falloffStart);

				if(dist>1){
					final=0;
				}else{
					final*=(float(1)-dist);
				}
			}
		}

		if(final!=0){
			float sh=1;
			if(level->lights[l]->shadow){sh=getShadowOcclusion(l,apos,sx,sy,imagedataL);}
			c+=level->lights[l]->color*final*sh;
		}
		
	}
	if(c.x>=1){c.x=1;}
	if(c.y>=1){c.y=1;}
	if(c.z>=1){c.z=1;}

	imagedata[apos.y*sx*3+apos.x*3]=c.x*255;
	imagedata[apos.y*sx*3+apos.x*3+1]=c.y*255;
	imagedata[apos.y*sx*3+apos.x*3+2]=c.z*255;
	//we use 0 to indicate an area not drawn on

	if(imagedata[apos.y*sx*3+apos.x*3]==0){imagedata[apos.y*sx*3+apos.x*3]=1;}
	if(imagedata[apos.y*sx*3+apos.x*3+1]==0){imagedata[apos.y*sx*3+apos.x*3+1]=1;}
	if(imagedata[apos.y*sx*3+apos.x*3+2]==0){imagedata[apos.y*sx*3+apos.x*3+2]=1;}
}


void drawPointShadow(Light* l,FloatVector3d pos, FloatVector3d normal,IntVector2d apos,int sx,int sy, unsigned char* imagedata,String name){
	
	float final=1;
	
	if(l->type==LIGHT_POINT){
		final=getShadowOcclusion(l,pos,name);
	}else if(l->type==LIGHT_SPOT){
		final=getShadowOcclusion(l,pos,name);
	}
	
	imagedata[apos.y*sx+apos.x]=final*255;
	
	//we use 0 to indicate an area not drawn on
	if(imagedata[apos.y*sx+apos.x]==0){imagedata[apos.y*sx+apos.x]=1;}
}


FloatVector3d posOnTri(Vertex* p1, Vertex* p2, Vertex* p3, float px, float py){
	FloatVector2d p=math.lineIntersection(p1->uvLightmap,p2->uvLightmap,p3->uvLightmap,FloatVector2d(px,py));

	float weight;

	if(p1->uvLightmap.x!=p2->uvLightmap.x){
		weight=(p.x-p1->uvLightmap.x)/(p2->uvLightmap.x-p1->uvLightmap.x);
	}else{
		weight=(p.y-p1->uvLightmap.y)/(p2->uvLightmap.y-p1->uvLightmap.y);
	}

	FloatVector3d final=(weight*p2->pos)+((1-weight)*p1->pos);

	FloatVector2d searchPos=FloatVector2d(px,py);

	if(p3->uvLightmap.x!=p.x){
		weight=math.abs((searchPos.x-p.x)/(p3->uvLightmap.x-p.x));
	}else{
		weight=math.abs((searchPos.y-p.y)/(p3->uvLightmap.y-p.y));
	}
		
	final=(weight * p3->pos)+((1-weight)*final);

	return final;
}


FloatVector3d normalOnTri(Vertex* p1, Vertex* p2, Vertex* p3, float px, float py){
	FloatVector2d p=math.lineIntersection(p1->uvLightmap,p2->uvLightmap,p3->uvLightmap,FloatVector2d(px,py));

	float weight;

	if(p1->uvLightmap.x!=p2->uvLightmap.x){
		weight=(p.x-p1->uvLightmap.x)/(p2->uvLightmap.x-p1->uvLightmap.x);
	}else{
		weight=(p.y-p1->uvLightmap.y)/(p2->uvLightmap.y-p1->uvLightmap.y);
	}

	FloatVector3d final=(weight*p2->normal)+((1-weight)*p1->normal);

	FloatVector2d searchPos=FloatVector2d(px,py);

	if(p3->uvLightmap.x!=p.x){
		weight=math.abs((searchPos.x-p.x)/(p3->uvLightmap.x-p.x));
	}else{
		weight=math.abs((searchPos.y-p.y)/(p3->uvLightmap.y-p.y));
	}
		
	final=(weight * p3->normal)+((1-weight)*final);

	return final;	
}

float getPosX(Vertex* v1, Vertex* v2,float y){
	y-=v1->uvLightmap.y;

	float percent=y/(v2->uvLightmap.y-v1->uvLightmap.y);

	percent=(v2->uvLightmap.x-v1->uvLightmap.x)*percent;
	percent+=v1->uvLightmap.x;
	return percent;
}

float returnTriangleArea(FloatVector2d p0,FloatVector2d  p1,FloatVector2d  p2){
	return (.5)*(p1.x*p2.y - p1.y*p2.x -p0.x*p2.y + p0.y*p2.x + p0.x*p1.y - p0.y*p1.x);
}

//on insideTriangle checkPoint, trianglePointOne, trianglePointTwo, trianglePointThree
bool CheckPointInTriangleR(FloatVector2d& checkPoint,FloatVector2d& trianglePointOne, FloatVector2d& trianglePointTwo,FloatVector2d& trianglePointThree){
	float areaThree = returnTriangleArea (checkPoint, trianglePointOne, trianglePointTwo);
	float areaOne = returnTriangleArea (checkPoint, trianglePointTwo, trianglePointThree);
	float areaTwo = returnTriangleArea (checkPoint, trianglePointThree, trianglePointOne);

	float minT=-0.0001;
	float maxT=0.0001;


	if(areaOne>minT && areaTwo>minT && areaThree>minT){
		return true;
	}else if(areaOne<maxT && areaTwo<maxT && areaThree<maxT){
		return true;
	}

	return false;

}

void rasterize(Vertex& p1,Vertex& p2,Vertex& p3,int sx,int sy,unsigned char* imagedata,unsigned char** imagedataL){

	float nx;
	float px;
	float ny;
	float py;

	nx=p1.uvLightmap.x;
	px=p1.uvLightmap.x;
	ny=p1.uvLightmap.y;
	py=p1.uvLightmap.y;

	if(p2.uvLightmap.x>px){px=p2.uvLightmap.x;}
	if(p2.uvLightmap.x<nx){nx=p2.uvLightmap.x;}
	if(p2.uvLightmap.y>py){py=p2.uvLightmap.y;}
	if(p2.uvLightmap.y<ny){ny=p2.uvLightmap.y;}

	if(p3.uvLightmap.x>px){px=p3.uvLightmap.x;}
	if(p3.uvLightmap.x<nx){nx=p3.uvLightmap.x;}
	if(p3.uvLightmap.y>py){py=p3.uvLightmap.y;}
	if(p3.uvLightmap.y<ny){ny=p3.uvLightmap.y;}

	float yinc=float(1)/float(sy);
	float xinc=float(1)/float(sx);

	if(nx<0){nx=0;}
	if(px>1){px=1;}

	if(ny<0){ny=0;}
	if(py>1){py=1;}

	for(float y=ny; y<py; y+=yinc){

		for(float x=nx; x<px; x+=xinc){

			FloatVector2d fv(x,y);
			if(CheckPointInTriangleR(fv,p1.uvLightmap,p2.uvLightmap,p3.uvLightmap)){
				drawPoint(posOnTri(&p1,&p2,&p3,x,y),normalOnTri(&p1,&p2,&p3,x,y),IntVector2d((sx-1)*x,(sy-1)*y),sx,sy,imagedata,imagedataL);
			}else{

			}

		}
	}
}


void rasterizeShadow(Light* l,Vertex& p1,Vertex& p2,Vertex& p3,int sx,int sy,unsigned char* imagedata,String& name){
	float nx;
	float px;
	float ny;
	float py;

	nx=p1.uvLightmap.x;
	px=p1.uvLightmap.x;
	ny=p1.uvLightmap.y;
	py=p1.uvLightmap.y;

	if(p2.uvLightmap.x>px){px=p2.uvLightmap.x;}
	if(p2.uvLightmap.x<nx){nx=p2.uvLightmap.x;}
	if(p2.uvLightmap.y>py){py=p2.uvLightmap.y;}
	if(p2.uvLightmap.y<ny){ny=p2.uvLightmap.y;}

	if(p3.uvLightmap.x>px){px=p3.uvLightmap.x;}
	if(p3.uvLightmap.x<nx){nx=p3.uvLightmap.x;}
	if(p3.uvLightmap.y>py){py=p3.uvLightmap.y;}
	if(p3.uvLightmap.y<ny){ny=p3.uvLightmap.y;}

	float yinc=float(1)/float(sy);
	float xinc=float(1)/float(sx);

	
	if(nx<0){nx=0;}
	if(px>1){px=1;}

	if(ny<0){ny=0;}
	if(py>1){py=1;}


	for(float y=ny; y<py; y+=yinc){

		for(float x=nx; x<px; x+=xinc){

			FloatVector2d fv(x,y);
			if(CheckPointInTriangleR(fv,p1.uvLightmap,p2.uvLightmap,p3.uvLightmap)){
				drawPointShadow(l,posOnTri(&p1,&p2,&p3,x,y),normalOnTri(&p1,&p2,&p3,x,y),IntVector2d((sx-1)*x,(sy-1)*y),sx,sy,imagedata,name);
			}else{

			}

		}
	}
}


void postprocess(unsigned char* imagedata,int sx,int sy){

	for(int y=0; y<sy; y++){
	
		for(int x=0; x<(sx-1); x++){

			if(imagedata[y*sx*3+x*3]==0 && imagedata[y*sx*3+x*3+3]!=0){
				imagedata[y*sx*3+x*3]=imagedata[y*sx*3+x*3+3];
				imagedata[y*sx*3+x*3+1]=imagedata[y*sx*3+x*3+4];
				imagedata[y*sx*3+x*3+2]=imagedata[y*sx*3+x*3+5];
			}
		}
	}

	for(int y=1; y<sy; y++){
	
		for(int x=0; x<sx; x++){

			if(imagedata[y*sx*3+x*3]==0 && imagedata[(y-1)*sx*3+x*3]!=0){

				imagedata[y*sx*3+x*3]=imagedata[(y-1)*sx*3+x*3];
				imagedata[y*sx*3+x*3+1]=imagedata[(y-1)*sx*3+x*3+1];
				imagedata[y*sx*3+x*3+2]=imagedata[(y-1)*sx*3+x*3+2];
			}
		}
	}

	unsigned char* tempbuff=new unsigned char[sx*sy*3];

	for(int y=0; y<sy; y++){
	
		for(int x=0; x<sx; x++){

			if(x==0){
				tempbuff[y*sx*3+x*3]=float(0.666)*imagedata[y*sx*3+x*3]+float(0.333)*imagedata[y*sx*3+x*3+3];
				tempbuff[y*sx*3+x*3+1]=float(0.666)*imagedata[y*sx*3+x*3+1]+float(0.333)*imagedata[y*sx*3+x*3+3+1];
				tempbuff[y*sx*3+x*3+2]=float(0.666)*imagedata[y*sx*3+x*3+2]+float(0.333)*imagedata[y*sx*3+x*3+3+2];
			}else if(x==sx-1){
				tempbuff[y*sx*3+x*3]=float(0.333)*imagedata[y*sx*3+x*3-3]+float(0.666)*imagedata[y*sx*3+x*3];
				tempbuff[y*sx*3+x*3+1]=float(0.333)*imagedata[y*sx*3+x*3-3+1]+float(0.666)*imagedata[y*sx*3+x*3+1];
				tempbuff[y*sx*3+x*3+2]=float(0.333)*imagedata[y*sx*3+x*3-3+2]+float(0.666)*imagedata[y*sx*3+x*3+2];
			}else{
				tempbuff[y*sx*3+x*3]=float(0.25)*imagedata[y*sx*3+x*3-3]+float(0.5)*imagedata[y*sx*3+x*3]+float(0.25)*imagedata[y*sx*3+x*3+3];
				tempbuff[y*sx*3+x*3+1]=float(0.25)*imagedata[y*sx*3+x*3-3+1]+float(0.5)*imagedata[y*sx*3+x*3+1]+float(0.25)*imagedata[y*sx*3+x*3+3+1];
				tempbuff[y*sx*3+x*3+2]=float(0.25)*imagedata[y*sx*3+x*3-3+2]+float(0.5)*imagedata[y*sx*3+x*3+2]+float(0.25)*imagedata[y*sx*3+x*3+3+2];
			}
		}
	}

	unsigned char* tempbuff2=new unsigned char[sx*sy*3];

	for(int y=0; y<sy; y++){
	
		for(int x=0; x<sx; x++){

			if(y==0){
				tempbuff2[y*sx*3+x*3]=float(0.666)*tempbuff[y*sx*3+x*3]+float(0.333)*tempbuff[(y+1)*sx*3+x*3];
				tempbuff2[y*sx*3+x*3+1]=float(0.666)*tempbuff[y*sx*3+x*3+1]+float(0.333)*tempbuff[(y+1)*sx*3+x*3+1];
				tempbuff2[y*sx*3+x*3+2]=float(0.666)*tempbuff[y*sx*3+x*3+2]+float(0.333)*tempbuff[(y+1)*sx*3+x*3+2];
			}else if(y==sy-1){
				tempbuff2[y*sx*3+x*3]=float(0.333)*tempbuff[(y-1)*sx*3+x*3]+float(0.666)*tempbuff[y*sx*3+x*3];
				tempbuff2[y*sx*3+x*3+1]=float(0.333)*tempbuff[(y-1)*sx*3+x*3+1]+float(0.666)*tempbuff[y*sx*3+x*3+1];
				tempbuff2[y*sx*3+x*3+2]=float(0.333)*tempbuff[(y-1)*sx*3+x*3+2]+float(0.666)*tempbuff[y*sx*3+x*3+2];
			}else{
				tempbuff2[y*sx*3+x*3]=float(0.25)*tempbuff[(y-1)*sx*3+x*3]+float(0.5)*tempbuff[y*sx*3+x*3]+float(0.25)*tempbuff[(y+1)*sx*3+x*3];
				tempbuff2[y*sx*3+x*3+1]=float(0.25)*tempbuff[(y-1)*sx*3+x*3+1]+float(0.5)*tempbuff[y*sx*3+x*3+1]+float(0.25)*tempbuff[(y+1)*sx*3+x*3+1];
				tempbuff2[y*sx*3+x*3+2]=float(0.25)*tempbuff[(y-1)*sx*3+x*3+2]+float(0.5)*tempbuff[y*sx*3+x*3+2]+float(0.25)*tempbuff[(y+1)*sx*3+x*3+2];
			}
		}
	}

	for(int i=0; i<sx*sy*3; i++){
		imagedata[i]=tempbuff2[i];
	}

	delete[] tempbuff;
	delete[] tempbuff2;
}


void postprocessShadow(unsigned char* imagedata,int sx,int sy){

	for(int y=0; y<sy; y++){
		for(int x=0; x<(sx-1); x++){
			if(imagedata[y*sx+x]==0 && imagedata[y*sx+x+1]!=0){
				imagedata[y*sx+x]=imagedata[y*sx+x+1];
			}
		}
	}

	for(int y=1; y<sy; y++){
		for(int x=0; x<sx; x++){
			if(imagedata[y*sx+x]==0 && imagedata[(y-1)*sx+x]!=0){
				imagedata[y*sx+x]=imagedata[(y-1)*sx+x];
			}
		}
	}

	unsigned char* tempbuff=new unsigned char[sx*sy];

	for(int y=0; y<sy; y++){
	
		for(int x=0; x<sx; x++){

			if(x==0){
				tempbuff[y*sx+x]=float(0.666)*imagedata[y*sx+x]+float(0.333)*imagedata[y*sx+x+1];
			}else if(x==sx-1){
				tempbuff[y*sx+x]=float(0.333)*imagedata[y*sx+x-1]+float(0.666)*imagedata[y*sx+x];
			}else{
				tempbuff[y*sx+x]=float(0.25)*imagedata[y*sx+x-1]+float(0.5)*imagedata[y*sx+x]+float(0.25)*imagedata[y*sx+x+1];
			}
		}
	}

	unsigned char* tempbuff2=new unsigned char[sx*sy];

	for(int y=0; y<sy; y++){
	
		for(int x=0; x<sx; x++){

			if(y==0){
				tempbuff2[y*sx+x]=float(0.666)*tempbuff[y*sx+x]+float(0.333)*tempbuff[(y+1)*sx+x];
			}else if(y==sy-1){
				tempbuff2[y*sx+x]=float(0.333)*tempbuff[(y-1)*sx+x]+float(0.666)*tempbuff[y*sx+x];
			}else{
				tempbuff2[y*sx+x]=float(0.25)*tempbuff[(y-1)*sx+x]+float(0.5)*tempbuff[y*sx+x]+float(0.25)*tempbuff[(y+1)*sx+x];
			}
		}
	}

	for(int i=0; i<sx*sy; i++){
		imagedata[i]=tempbuff2[i];
	}

	delete[] tempbuff;
	delete[] tempbuff2;
}

bool Lightmap::build(Object* obj,int sx,int sy){
	
	

	unsigned char* imagedata=new unsigned char[sx*sy*3];

	for(int i=0; i<sx*sy*3; i++){
		imagedata[i]=0;
	}

	unsigned char** imagedataL=new unsigned char*[level->lights.size()];

	int lsx=sx/SHADOW_SCALE;
	int lsy=sy/SHADOW_SCALE;

	for(int i=0; i<level->lights.size(); i++){
		if(level->lights[i]->shadow){
			#ifdef OPTI_LIST
			genOptiList(level->lights[i],obj->name);
			#endif
			imagedataL[i]=new unsigned char[lsx*lsy];
			
			for(int m=0; m<lsx*lsy; m++){
				imagedataL[i][m]=0;
			}

			for(int v=0; v<obj->getVertexCount(); v+=3){
				rasterizeShadow(level->lights[i],obj->getVertex(v),obj->getVertex(v+1),obj->getVertex(v+2),lsx,lsy,imagedataL[i],obj->name);
			}
			postprocessShadow(imagedataL[i],lsx,lsy);
			#ifdef OPTI_LIST
			delOptiList();
			#endif

		}
	}

	for(int i=0; i<obj->getVertexCount(); i+=3){
		rasterize(obj->getVertex(i),obj->getVertex(i+1),obj->getVertex(i+2),sx,sy,imagedata,imagedataL);
	}

	postprocess(imagedata,sx,sy);

	ImageData id;
	id.bitsPP=24;
	id.bytesPP=3;
	id.data=imagedata;
	id.height=sy;
	id.width=sx;
	id.size=sx*sy*3;
	
	level->file;

	String fdir=level->file.getRelativeNoExtension();

	Path fname;
	fname.setRelative(fdir+"_"+obj->name+".bmp");
	bmp.save(fname,id);

	delete[] imagedata;


	for(int i=0; i<level->lights.size(); i++){
		if(level->lights[i]->shadow){
			delete[] imagedataL[i];
		}
	}
	
	//update the lightmap in the engine
	TextureImage* temp=new TextureImage[level->lightmapCount];
	for(int i=0; i<level->lightmapCount; i++){
		temp[i]=level->lightmaps[i];
	}

	level->lightmaps.clear();
	
	level->lightmapCount++;

	for(int i=0; i<level->lightmapCount; i++){level->lightmaps.pushBack();}

	for(int i=0; i<level->lightmapCount-1; i++){
		level->lightmaps[i]=temp[i];
	}

	unsigned int t=texture.load(fname);
	level->lightmaps[level->lightmapCount-1].glName(t);
	level->lightmaps[level->lightmapCount-1].file=fname.getRelative();
	obj->listBuilt=false;
	obj->lightmap=level->lightmapCount-1;

	drawRefreshAll();	//in case it is static

	return true;
}

static int luaRenderLightmap(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=3){
		console().write("Lightmap error: bad arguments");
		return 0;
	}

	String object;
	int sx;
	int sy;

	if(args[0].type==SCRIPT_STRING_STORED){
		object=args[0].value.ss;
	}else{
		console().write("Lightmap error: bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){
		sx=args[1].value.ds;
	}else{
		console().write("Lightmap error: bad arguments");
		return 0;
	}

	if(args[2].type==SCRIPT_DOUBLE_STORED){
		sy=args[2].value.ds;
	}else{
		console().write("Lightmap error: bad arguments");
		return 0;
	}

	if(level->objectKey.find(object)){
		Object* o=level->objectKey[object];

		sx=targetPowerOf2(sx);
		sy=targetPowerOf2(sy);

		lightmap.build(o,sx,sy);

		return 0;

	}else{
		console().write("Lightmap error: the object '"+object+"' doesn't exist or isn't a polymesh.");
		return 0;
	}	

	return 0;
}


static int luaDeleteLightmap(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Lightmap error: bad arguments");
		return 0;
	}

	String name;

	if(args[0].type==SCRIPT_STRING_STORED){
		name=args[0].value.ss;
	}else{
		console().write("Lightmap error: bad arguments");
		return 0;
	}

	int i;

	for(i=0; i<level->lightmaps.size(); i++){
		if(level->lightmaps[i].name==name){
			break;
		}
	}

	if(i!=level->lightmaps.size()){
		level->lightmaps.erase(i);
		level->lightmapCount--;
	}else{
		console().write("error, lightmap '"+name+"' not found to delete");
	}

	return 0;
}


bool Lightmap::scriptInit(){
	LuaType* luaa=script().add("lightmap");
	script().add("render",luaRenderLightmap,luaa);
	script().add("delete",luaDeleteLightmap,luaa);
	return true;
}

#endif