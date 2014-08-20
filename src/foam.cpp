#ifdef FEATURE_FOAM

#ifdef _WIN32
#include <windows.h>	// Header File For Windows
#endif

#include "GlHeader.hpp"

#include "Types.hpp"
#include "Helperlib/Containers.hpp"
#include "Helperlib/HelperLibMath.hpp"
#include "random.h"
#include "level.hpp"
#include "conf.h"
#include "shader.hpp"
#include "texture.hpp"
#include "water.hpp"
#include "timeing.hpp"

unsigned int foamTex;

bool foamInit(){
	int lol=texture.load("foam.bmp");
	if(lol<0){return false;}

	foamTex=lol;
	
	return true;
}

class FoamLine{

	public:
		Ray2d r;
		Ray2d n;
		float l;	//length
};

class FoamParticle{

	public:
		FloatVector2d pos;	//x=x, y=z (water used for y)
		float age;	//length
		FoamParticle();
};

FoamParticle::FoamParticle(){
	age=0;
}

Array<FoamLine> fLines;
FoamParticle* fParticles;	//array
int foamParticles;
float fMaxLength;


void plane_imp_triangle_int_add_3d ( double x1, double y1, double z1, 
  double x2, double y2, double z2, double dist1, double dist2, 
  int *num_int, double x[], double y[], double z[] )

//********************************************************************
//
//  Purpose:
//
//    PLANE_IMP_TRIANGLE_INT_ADD_3D is a utility for PLANE_IMP_TRIANGLE_INT_3D.
//
//  Discussion:
//
//    This routine is called to consider the value of the signed distance
//    from a plane of two nodes of a triangle.  If the two values
//    have opposite signs, then there is a point of intersection between
//    them.  The routine computes this point and adds it to the list.
//
//  Modified:
//
//    19 April 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X1, Y1, Z1, X2, Y2, Z2, X3, Y3, Z3, the coordinates
//    of two vertices of a triangle.
//
//    Input, double DIST1, DIST2, the signed distances of the two vertices
//    from a plane.  
//
//    Input/output, int *NUM_INT, the number of intersection points.
//
//    Input/output, double X[], Y[], Z[], the coordinates
//    of the intersection points.
//
{
  double alpha;

  if ( dist1 == 0.0E+00 )
  {
    x[*num_int] = x1;
    y[*num_int] = y1;
    z[*num_int] = z1;
    *num_int = *num_int + 1;
  }
  else if ( dist2 == 0.0E+00 )
  {
    x[*num_int] = x2;
    y[*num_int] = y2;
    z[*num_int] = z2;
    *num_int = *num_int + 1;
  }
  else if ( dist1 * dist2 < 0.0E+00 )
  {
    alpha = dist2 / ( dist2 - dist1 );
    x[*num_int] = alpha * x1 + ( 1.0E+00 - alpha ) * x2;
    y[*num_int] = alpha * y1 + ( 1.0E+00 - alpha ) * y2;
    z[*num_int] = alpha * z1 + ( 1.0E+00 - alpha ) * z2;
    *num_int = *num_int + 1;
  }

  return;
}


int plane_norm_triangle_int_3d ( double x1, double y1, double z1, double x2, 
  double y2, double z2, double x3, double y3, double z3, double xp, double yp, 
  double zp, double xn, double yn, double zn, double x[], double y[], double z[] )

//******************************************************************************
//
//  Purpose:
//
//    PLANE_NORM_TRIANGLE_INT_3D: intersection ( normal plane, triangle ) in 3D.
//
//  Definition:
//
//    The normal form of a plane in 3D is:
//
//      (Xp,Yp,Zp) is a point on the plane,
//      (Xn,Yn,Zn) is a normal vector to the plane.
//
//  Discussion:
//
//    There may be 0, 1, 2 or 3 points of intersection returned.
//
//    If two intersection points are returned, then the entire line
//    between them comprises points of intersection.
//
//    If three intersection points are returned, then all points of
//    the triangle intersect the plane.
//
//  Modified:
//
//    12 October 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X1, Y1, Z1, X2, Y2, Z2, X3, Y3, Z3, the coordinates
//    of the vertices of the triangle.
//
//    Input, double XP, YP, ZP, a point on the plane.
//
//    Input, double XN, YN, ZN, a normal vector to the plane.
//
//    Output, double X[3], Y[3], Z[3], the coordinates of the intersection points.
//
//    Output, int PLANE_NORM_TRIANGLE_INT_3D, the number of intersection 
//    points returned.
//
{
  double d;
  double dist1;
  double dist2;
  double dist3;
  int num_int;

  num_int = 0;
//
//  Compute the signed distances between the vertices and the plane.
//
  d = - xn * xp - yn * yp - zn * zp;
  dist1 = xn * x1 + yn * y1 + zn * z1 + d;
  dist2 = xn * x2 + yn * y2 + zn * z2 + d;
  dist3 = xn * x3 + yn * y3 + zn * z3 + d;
//
//  Consider any zero distances.
//
  if ( dist1 == 0.0E+00 )
  {
    x[num_int] = x1;
    y[num_int] = y1;
    z[num_int] = z1;
    num_int = num_int + 1;
  }

  if ( dist2 == 0.0E+00 )
  {
    x[num_int] = x2;
    y[num_int] = y2;
    z[num_int] = z2;
    num_int = num_int + 1;
  }

  if ( dist3 == 0.0E+00 )
  {
    x[num_int] = x3;
    y[num_int] = y3;
    z[num_int] = z3;
    num_int = num_int + 1;
  }
//
//  If 2 or 3 of the nodes intersect, we're already done.
//
  if ( 2 <= num_int )
  {
    return num_int;
  }
//
//  If one node intersects, then we're done unless the other two
//  are of opposite signs.
//
  if ( num_int == 1 )
  {
    if ( dist1 == 0.0E+00 )
    {
      plane_imp_triangle_int_add_3d ( x2, y2, z2, x3, y3, z3, 
        dist2, dist3, &num_int, x, y, z );
    }
    else if ( dist2 == 0.0E+00 )
    {
      plane_imp_triangle_int_add_3d ( x1, y1, z1, x3, y3, z3, 
        dist1, dist3, &num_int, x, y, z );
    }
    else if ( dist3 == 0.0E+00 )
    {
      plane_imp_triangle_int_add_3d ( x1, y1, z1, x2, y2, z2, 
        dist1, dist2, &num_int, x, y, z );
    }

    return num_int;
  }
//
//  All nodal distances are nonzero, and there is at least one
//  positive and one negative.
//
  if ( dist1 * dist2 < 0.0E+00 && dist1 * dist3 < 0.0E+00 )
  {
    plane_imp_triangle_int_add_3d ( x1, y1, z1, x2, y2, z2, 
      dist1, dist2, &num_int, x, y, z );

    plane_imp_triangle_int_add_3d ( x1, y1, z1, x3, y3, z3, 
      dist1, dist3, &num_int, x, y, z );
  }
  else if ( dist2 * dist1 < 0.0E+00 && dist2 * dist3 < 0.0E+00 )
  {
    plane_imp_triangle_int_add_3d ( x2, y2, z2, x1, y1, z1, 
      dist2, dist1, &num_int, x, y, z );

    plane_imp_triangle_int_add_3d ( x2, y2, z2, x3, y3, z3, 
      dist2, dist3, &num_int, x, y, z );
  }
  else if ( dist3 * dist1 < 0.0E+00 && dist3 * dist2 < 0.0E+00 )
  {
    plane_imp_triangle_int_add_3d ( x3, y3, z3, x1, y1, z1, 
      dist3, dist1, &num_int, x, y, z );

    plane_imp_triangle_int_add_3d ( x3, y3, z3, x2, y2, z2, 
      dist3, dist2, &num_int, x, y, z );
  }

  return num_int;
}

int hitcount=0;

double xo[3];
double yo[3];
double zo[3];

//return 0,0 if none found
Ray2d findFoamIntersect(FloatVector3d a,FloatVector3d b,FloatVector3d c,float y){
	Ray2d i;
	
	int num=plane_norm_triangle_int_3d ( a.x, a.y, a.z,  b.x, 
										b.y, b.z, c.x, c.y, c.z, 0,y, 
										0, 0, 1, 0, xo, yo, zo );
	
	if(num==2){
		i.start.x=xo[0];
		i.start.y=zo[0];
		
		i.end.x=xo[1];
		i.end.y=zo[1];
		
		hitcount++;
	}
	
	return i;
}

bool foamBuilt=false;

class FS{
public:
	int l;	//index of line
	FloatVector2d n;
	bool used;
	bool start;	//start or end?

	FS(){
		used=false;
		start=false;
	}

};

SlowMap<FloatVector2d,FS> nMap;

void buildFoam(){
	if(conf->waterFoam){

		fLines.clear();

		Array<Object*>* objects=&level->staticObjects;

		Ray2d cr;
		FoamLine cf;


		FloatVector2d currentNormalStart;
		FloatVector2d currentNormalEnd;
		FS currentFS;

		unsigned int vcount;

		for(int i=0; i<objects->size(); i++){

			if((*objects)[i]->type==0  && ((*objects)[i]->flags & OBJECT_VISIBLE)){
				vcount=(*objects)[i]->getVertexCount();

					

				for(unsigned int v=0; v<vcount; v+=3){



					cr=findFoamIntersect(
						(*objects)[i]->getVertex(v).pos,
						(*objects)[i]->getVertex(v+1).pos,
						(*objects)[i]->getVertex(v+2).pos,
						level->water->pos.y);

					if(cr.start!=cr.end){

						cf.r=cr;
						cf.l=cr.length();

						FloatVector3d mo=math.toNormal((*objects)[i]->getVertex(v).pos,(*objects)[i]->getVertex(v+1).pos,(*objects)[i]->getVertex(v+2).pos);

							currentNormalStart.x=mo.x;
							currentNormalStart.y=mo.z;
							currentNormalEnd=currentNormalStart;

							if(nMap.find(cf.r.start)){
								if(nMap[cf.r.start].used){
									cf.n.start=nMap[cf.r.start].n;
								}else{
									currentNormalStart=nMap[cf.r.start].n+currentNormalStart;
									currentNormalStart.normalize();
									nMap[cf.r.start].n=currentNormalStart;
									nMap[cf.r.start].used=true;
								}
								
								if(nMap[cf.r.start].start){
									fLines[nMap[cf.r.start].l].n.start=currentNormalStart;
								}else{
									fLines[nMap[cf.r.start].l].n.end=currentNormalStart;
								}
							}else{


								currentFS.n=currentNormalStart;
								currentFS.l=fLines.size();
								currentFS.start=true;
								nMap[cf.r.start]=currentFS;
							}

							cf.n.start=currentNormalStart;


							if(nMap.find(cf.r.end)){
								if(nMap[cf.r.end].used){
									cf.n.end=nMap[cf.r.end].n;
								}else{
									currentNormalEnd=nMap[cf.r.end].n+currentNormalEnd;
									currentNormalEnd.normalize();
									
									nMap[cf.r.end].n=currentNormalEnd;
									nMap[cf.r.end].used=true;
								}
								
								if(nMap[cf.r.end].start){
									fLines[nMap[cf.r.end].l].n.start=currentNormalEnd;
								}else{
									fLines[nMap[cf.r.end].l].n.end=currentNormalEnd;
								}
							}else{
								currentFS.n=currentNormalEnd;
								currentFS.l=fLines.size();
								currentFS.start=false;
								nMap[cf.r.end]=currentFS;
							}

							
							cf.n.end=currentNormalEnd;
						
							fLines.pushBack(cf);
					}
				}

				nMap.clear();

			}
		}
	}

	FloatVector2d temp;
	FloatVector3d checkN;
	FloatVector3d n1;
	FloatVector3d n2;
	FloatVector3d n3;

	for(int m=0; m<fLines.size(); m++){
		n1.x=fLines[m].r.start.x;
		n1.y=level->water->pos.y;
		n1.z=fLines[m].r.start.y;

		n2.x=fLines[m].r.end.x;
		n2.y=level->water->pos.y;
		n2.z=fLines[m].r.end.y;

		n3.x=fLines[m].r.end.x+fLines[m].n.end.x;
		n3.y=level->water->pos.y;
		n3.z=fLines[m].r.end.y+fLines[m].n.end.y;

		checkN=math.toNormal(n1,n2,n3);

		if(checkN.y<0){

			temp=fLines[m].r.end;
			fLines[m].r.end=fLines[m].r.start;
			fLines[m].r.start=temp;

			temp=fLines[m].n.end;
			fLines[m].n.end=fLines[m].n.start;
			fLines[m].n.start=temp;
			
		}
	}


	foamBuilt=true;

}


bool particlesBuilt=false;
float fAge=5;
float fSize=5;
float fAlpha=0.1;
float fDecay=0.01;
float fSpeed=1;
float fLayers=2;

void buildParticles(){
	
	foamParticles=5000;
	fParticles=new FoamParticle[foamParticles];
	particlesBuilt=true;
}

void updateParticles(){

	if(!particlesBuilt){
		buildParticles();
	}

	for(int i=0; i<foamParticles; i++){

		if(fParticles[i].age==0 || fParticles[i].age>fAge){
			//remake
			float t=randomGen.Random();

			fParticles[i].age=1;

			long line=randomGen.IRandom(0,fLines.size()-1);

			

			if(randomGen.Random()<(fLines[line].r.length()/fMaxLength)){
				fParticles[i].age=0;
				break;
			}

			fParticles[i].pos=fLines[line].r.posAlong(randomGen.Random());

		}else{
			fParticles[i].pos.x+=waterDirection.x*engineTime.time;
			fParticles[i].pos.y+=waterDirection.y*engineTime.time;
			fParticles[i].age+=fDecay*engineTime.time;

		}


	}
}

void drawFoamParticle(){

	if(conf->waterFoam && foamBuilt){

		updateParticles();
	



		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE3_ARB);
		glDisable(GL_TEXTURE_2D);


		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,foamTex);


		glDisable(GL_LIGHTING);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glEnable(GL_DEPTH_TEST);

		shader.reset();

		glColor4f(1,1,1,fAlpha);

		float s;

		float y=level->water->pos.y+1;

		glBegin(GL_QUADS);

			for(int i=0; i<foamParticles; i++){
				y+=0.0002;

				s=(fSize-fParticles[i].age);

				glTexCoord2f(0,0);
				glVertex3f(	fParticles[i].pos.x-s,
						y,
						fParticles[i].pos.y+s);

				glTexCoord2f(1,0);
				glVertex3f(	fParticles[i].pos.x+s,
						y,
						fParticles[i].pos.y+s);

				glTexCoord2f(1,1);
				glVertex3f(	fParticles[i].pos.x+s,
						y,
						fParticles[i].pos.y-s);

				glTexCoord2f(0,1);
				glVertex3f(	fParticles[i].pos.x-s,
						y,
						fParticles[i].pos.y-s);
			}

		glEnd();
	}
}



float texPos=0;

void drawFoam(){

	if(conf->waterFoam && foamBuilt){

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE3_ARB);
		glDisable(GL_TEXTURE_2D);


		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,foamTex);

		glDisable(GL_LIGHTING);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

		glEnable(GL_DEPTH_TEST);

		shader.reset();

		float y=level->water->pos.y+1;

		glBegin(GL_QUADS);

		texPos+=engineTime.time*fSpeed;

		for(int i=0; i<fLines.size(); i++){

			glColor4f(1,1,1,fAlpha);

			glTexCoord2f(0,texPos);
			glVertex3f(	fLines[i].r.start.x-fLines[i].n.start.x,
						y,
						fLines[i].r.start.y-fLines[i].n.start.y);


			glTexCoord2f(1,texPos);
			glVertex3f(	fLines[i].r.end.x-fLines[i].n.end.x,
						y,
						fLines[i].r.end.y-fLines[i].n.end.y);

			glColor4f(1,1,1,0);

			glTexCoord2f(1,texPos+1);
			glVertex3f(	fLines[i].r.end.x+fLines[i].n.end.x*fSize,
						y,
						fLines[i].r.end.y+fLines[i].n.end.y*fSize);

			glTexCoord2f(0,texPos+1);
			glVertex3f(	fLines[i].r.start.x+fLines[i].n.start.x*fSize,
						y,
						fLines[i].r.start.y+fLines[i].n.start.y*fSize);
		}

		glEnd();
	}
}

void drawFoamEdges(){
	if(conf->waterFoam && foamBuilt){

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE3_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE4_ARB);
		glDisable(GL_TEXTURE_2D);


		glActiveTextureARB(GL_TEXTURE0_ARB);

		glLineWidth(5);

		glBegin(GL_LINES);

		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		shader.reset();

		glColor4f(1,0,0,1);


		bool sw=false;
		for(int i=0; i<fLines.size(); i++){

			sw=!sw;

			if(sw){
				continue;
			}

			glColor4f(1,1,1,1);

			glVertex3f(	fLines[i].r.start.x,
						level->water->pos.y,
						fLines[i].r.start.y);

			glVertex3f(	fLines[i].r.end.x,
						level->water->pos.y,
						fLines[i].r.end.y);

			glColor4f(1,0,0,1);

			glVertex3f(	fLines[i].r.start.x,
						level->water->pos.y,
						fLines[i].r.start.y);

			glVertex3f(	fLines[i].r.start.x+fLines[i].n.start.x,
						level->water->pos.y,
						fLines[i].r.start.y+fLines[i].n.start.y);

			glVertex3f(	fLines[i].r.end.x,
						level->water->pos.y,
						fLines[i].r.end.y);

			glVertex3f(	fLines[i].r.end.x+fLines[i].n.end.x,
						level->water->pos.y,
						fLines[i].r.end.y+fLines[i].n.end.y);
		}

		glEnd();
	}
}


#endif