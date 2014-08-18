#include "GlHeader.hpp"

#include <math.h>			// Math Library Header File
#include <stdio.h>			// Header File For Standard Input/Output

#include "HelperLibMath.hpp"

Math math;

float tcost(float i){
	return cos(i);
}

float tsint(float i){
	return sin(i);
}

float ttant(float i){
	return tan(i);
}

float tabst(float i){
	return abs(i);
}
float tpowt(float i,float p){
	return pow(i,p);
}

float tceilt(float i){
	return ceil(i);
}

float tfloort(float i){
	return floor(i);
}
float tsqrtt(float i){
	return sqrt(i);
}

float tlnt(float i){
	return log(i);
}

float Math::area(FloatVector3d v1,FloatVector3d v2, FloatVector3d v3){
	float f=0.5*crossProduct( v2-v1, v3-v1).magnitude();
	return f;
}

float Math::cos(float i){
	return tcost(i * (PI/180));
}

float Math::sin(float i){
	return tsint(i * (PI/180));
}

float Math::cosDegrees(float i){
	return tcost(i * (PI/180));
}

float Math::sinDegrees(float i){
	return tsint(i * (PI/180));
}

float Math::tanDegrees(float i){
	return ttant(i * (PI/180));
}


float Math::cosRadians(float i){
	return tcost(i);
}

float Math::sinRadians(float i){
	return tsint(i);
}


float Math::arccosDegrees(float i){
	return acos(i) * (180/PI);
}

float Math::arcsinDegrees(float i){
	return asin(i) * (180/PI);
}

float Math::arctanDegrees(float i){
	return atan(i) * (180/PI);
}

float Math::arctan2Degrees(float i,float j){
	return atan2(i,j) * (180/PI);
}

float Math::arccosRadians(float i){
	return acos(i);
}

float Math::arcsinRadians(float i){
	return asin(i);
}

float Math::arctanRadians(float i){
	return atan(i);
}

float Math::arctan2Radians(float i,float j){
	return atan2(i,j);
}

float Math::abs(float i){
	return tabst(i);
}
float Math::pow(float i,float p){
	return tpowt(i,p);
}

float Math::floor(float i){
	return tfloort(i);
}

float Math::ceil(float i){
	return tceilt(i);
}
float Math::sqrt(float i){
	return tsqrtt(i);
}

float Math::ln(float i){
	return tlnt(i);
}
// 3D - Gets the distance between the first 3 cords and the 2nd 3 cords
float Math::distance(POS_3D one,  POS_3D two){
    float dif_x = one.x-two.x;
    float dif_y = one.y-two.y;
    float dif_z = one.z-two.z;
    float dist1 = (dif_x*dif_x)+(dif_y*dif_y);
    dist1 = sqrt(dist1);
    float dist2 = (dist1*dist1)+(dif_z*dif_z);
    dist2 = sqrt(dist2);

    return dist2;
}

float Math::distance(POS_3D one,  float x, float y, float z){
    float dif_x = one.x-x;
    float dif_y = one.y-y;
    float dif_z = one.z-z;
    float dist1 = (dif_x*dif_x)+(dif_y*dif_y);
    dist1 = sqrt(dist1);
    float dist2 = (dist1*dist1)+(dif_z*dif_z);
    dist2 = sqrt(dist2);

    return dist2;
}

float Math::distance(float x1, float y1, float z1, float x2, float y2, float z2){
    float dif_x = x1-x2;
    float dif_y = y1-y2;
    float dif_z = z1-z2;
    float dist1 = (dif_x*dif_x)+(dif_y*dif_y);
    dist1 = sqrt(dist1);
    float dist2 = (dist1*dist1)+(dif_z*dif_z);
    dist2 = sqrt(dist2);

    return dist2;
}

// 2D - Gets the distance between the first 2 cords and the 2nd 2 cords
float Math::distance(POS_2D one,  POS_2D two){
    float dif_x = one.x-two.x;
    float dif_y = one.y-two.y;
    float dist1 = (dif_x*dif_x)+(dif_y*dif_y);
    dist1 = sqrt(dist1);

    return dist1;
}

float Math::distance(float x1, float y1,  float x2, float y2){
    float dif_x = x1-x2;
    float dif_y = y1-y2;
    float dist1 = (dif_x*dif_x)+(dif_y*dif_y);
    dist1 = sqrt(dist1);

    return dist1;
}

float Math::length(FloatVector2d i){
    //finds length of a 2d vector
    
    float dist1 = (i.x*i.x)+(i.y*i.y);
    dist1 = sqrt(dist1);

    return dist1;
}


float Math::dotProduct(FloatVector2d v1, FloatVector2d v2){
	float final=(v1.x*v2.x)+(v1.y*v2.y);
	
	return final;
}

float Math::dotProduct(FloatVector3d v1, FloatVector3d v2){
	float final=(v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z);
	
	return final;
}

float Math::dotProduct(FloatVector4d v1, FloatVector4d v2){
	float final=(v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z)+(v1.w*v2.w);
	
	return final;
}


float Math::magnitude(FloatVector2d input){
    float mag = (input.x*input.x)+(input.y*input.y);
    mag = sqrt(mag);
    
    return mag;
}

float Math::magnitude(FloatVector3d input){
    float mag = (input.x*input.x)+(input.y*input.y)+(input.z*input.z);
    mag = sqrt(mag);
    
    return mag;
}

float Math::magnitude(GLubyteVector3d input){
    float mag = (float(input.x)*float(input.x))+(float(input.y)*float(input.y))+(float(input.z)*float(input.z));
    mag = sqrt(mag);
    
    return mag;
}



FloatVector3d Math::crossProduct(FloatVector3d v1,FloatVector3d v2){
	FloatVector3d final;
	
	final.x=(v1.y*v2.z)-(v1.z*v2.y);
	final.y=(v1.z*v2.x)-(v1.x*v2.z);
	final.z=(v1.x*v2.y)-(v1.y*v2.x);
	
	return final;
}


FloatVector3d Math::normalize(FloatVector3d input){
	float mag=1/magnitude(input);
	FloatVector3d final;
	final.x=input.x*mag;
	final.y=input.y*mag;
	final.z=input.z*mag;
	
	return final;
}

FloatVector3d Math::normalize(int max, FloatVector3d input){


	
	float mag=max/magnitude(input);
	FloatVector3d final;
	final.x=input.x*mag;
	final.y=input.y*mag;
	final.z=input.z*mag;
	
	return final;
}

FloatVector3d Math::normalize(float max, FloatVector3d input){


	
	float mag=max/magnitude(input);
	FloatVector3d final;
	final.x=input.x*mag;
	final.y=input.y*mag;
	final.z=input.z*mag;
	
	return final;
}



GLubyteVector3d Math::normalize(GLubyteVector3d input){
	float mag=1/magnitude(input);
	
	GLubyteVector3d final;
	final.x=input.x*(GLubyte)mag;
	final.y=input.y*(GLubyte)mag;
	final.z=input.z*(GLubyte)mag;
	
	return final;
}

GLubyteVector3d Math::normalize(int max, GLubyteVector3d input){
	float mag=max/magnitude(input);
	
	GLubyteVector3d final;
	final.x=GLubyte(input.x*mag);
	final.y=GLubyte(input.y*mag);
	final.z=GLubyte(input.z*mag);
	
	return final;
}




FloatVector3d Math::toNormal(FloatVector3d p1, FloatVector3d p2, FloatVector3d p3){
	FloatVector3d v1;
	FloatVector3d v2;
	
	v1=p1-p2;
	v2=p3-p2;

	FloatVector3d final=crossProduct(v1,v2);
	
	final=normalize(final);
	
	final.x=-final.x;
	final.y=-final.y;
	final.z=-final.z;
	
	return final;
}



FloatVector3d Math::toNormal(FloatVector3d p1, FloatVector3d p2){
	
	FloatVector3d p3;
	
	FloatVector3d v1;
	FloatVector3d v2;
	
	v1=p1-p2;
	
	v2=p3-p2;
	
	FloatVector3d final=crossProduct(v1,v2);
	
	final=normalize(final);
	
	final.x=-final.x;
	final.y=-final.y;
	final.z=-final.z;
	
	return final;
}

FloatVector2d Math::toNormal(FloatVector2d p1, FloatVector2d p2){
	
	FloatVector2d final;	

	float angle=math.vectorToAngle(p2,p1);
	angle+=90;

	final.x=(- cos((angle-90) * PI/180));	
	final.y=(sin((angle-90) * PI/180));	

	final.normalize();

	return final;
	
	

}


float Math::vectorToAngle(FloatVector2d from, FloatVector2d to){
	//hack - probably a better way to do this
	//corrects it if it's in another sector

	float cos_theta=dotProduct(from,to)/(magnitude(from)*magnitude(to));
	
	float final=acos(cos_theta) * 180 / PI;
	
	if( (to.x<0)){
		final=180-final;
		final+=180;
	}
	
	if( (from.y<0)){
		final=180-final;
		final+=180;
	}
	
	
	return final;
}

float Math::vectorToAngleDegrees(FloatVector2d a){
	//not really tested but should work.  probably a better way, this is my hack code.

	a.normalize();

	if(a.x>0 && a.y>0){
		//1st quadrent

		return arccosDegrees(a.x);
	}else if(a.x<0 && a.y>0){
		//2nd quadrent
		return 180-arcsinDegrees(a.y);
	}else if(a.x<0 && a.y<0){
		//3rd quadrent
		return arcsinDegrees(-a.y)+180;
	}else if(a.x>0 && a.y<0){
		//3rd quadrent
		return 360-arccosDegrees(a.x);
	}

	//weird stuff

	if(a.x>0 && a.y==0){
		return 0;
	}

	if(a.y>0 && a.x==0){
		return 90;
	}

	if(a.y==0 && a.x<0){
		return 180;
	}

	if(a.y<0 && a.x==0){
		return 270;
	}

	return 0;
}


FloatVector3d Math::rayToAngle(FloatVector3d start, FloatVector3d end){
	return FloatVector3d();
}

float Math::rayToAngle(FloatVector2d from, FloatVector2d to){
	FloatVector3d blah;

	blah=to-from;

	blah.normalize();

	float sinangle=asin(blah.y)*180/PI;
	//270-88
	if(blah.x>=0 && blah.y>=0){
		//QI
		return sinangle;
	}else if(blah.x<=0 && blah.y>=0){
		//QII
		return sinangle+180;
	}else if(blah.x<=0 && blah.y<=0){
		//QIII
		return sinangle+240;
	}else if(blah.x>=0 && blah.y<=0){
		//QIV
		return 360-sinangle;
	}

	return 0;
}



FloatVector2d Math::rotToNormal(float i){

	/*
http://kali.qcas.gu.edu.au/~tbv2/week6.html
function angleToVector(faceAngle) {
        radians = faceAngle * (Math.PI / 180);
        xAxis = Math.cos(radians);
        yAxis = Math.sin(radians);
        
        return [xAxis, yAxis];
}
*/
	//FloatVector2d radians;
	i-=90;  //have to correct it for some reason?
	
	FloatVector2d final;
	
	float radians=i*(PI/180);
	
	final.x=cos(radians);
	final.y=sin(radians);
	
	
	return final;
}
float Math::clampAngleDegrees(float i){

	if(i>0 && i<360){
		return i;
	}

	while(i>360){
		i-=360;

		if(i>0 && i<360){
			return i;
		}
	}

	

	while(i<0){
		i+=360;

		if(i>0 && i<360){
			return i;
		}
	}

	return 0;
}

FloatVector3d Math::degreesToNormal(FloatVector3d angle){

	FloatVector3d final;

    float dist=1;

	float oax=clampAngleDegrees(angle.x);

	if(oax>90 || oax<-90){
		dist=-1;
		angle.x=angle.x-180;
	}

	final.z=cosDegrees(-angle.x+180)*dist;
	final.y=sinDegrees(-angle.x+180)*dist;



	

	dist=math.abs(final.z);

	float c=180;

	if(oax>90 && oax<270){
		angle.y+=c;
	}

	final.x=cosDegrees(270-angle.y)*dist;
	final.z=sinDegrees(270-angle.y)*dist;

	return final.normalized();
}

// 2D - finds if the first rect intersects the 2nd rect, uses the top left and bottom right cords as input
bool Math::point_in_rect(){
	return true;
}

float Math::distance(Plane3d plane,FloatVector3d point){

	float d=-math.dotProduct(plane.normal,FloatVector3d(plane.x,plane.y,plane.z));

	float t=plane.normal.x*point.x+plane.normal.y*point.y+plane.normal.z*point.z+d;


	t=math.abs(t);
	float b=math.sqrt(plane.normal.x*plane.normal.x+plane.normal.y*plane.normal.y+plane.normal.z*plane.normal.z);
	return t/b;
}

float Math::distanceSigned(Plane3d plane,FloatVector3d point){

	float d=-math.dotProduct(plane.normal,FloatVector3d(plane.x,plane.y,plane.z));

	float t=plane.normal.x*point.x+plane.normal.y*point.y+plane.normal.z*point.z+d;

	float b=math.sqrt(plane.normal.x*plane.normal.x+plane.normal.y*plane.normal.y+plane.normal.z*plane.normal.z);
	return t/b;
}

FloatVector2d Math::closestPointOnALine(FloatVector2d linePoint1,FloatVector2d linePoint2,FloatVector2d searchPoint){

	
	FloatVector2d A=linePoint1;
	FloatVector2d B=linePoint2;

	if(A.x-B.x==0){
		return FloatVector2d(A.x,searchPoint.y);
	}else if(A.y-B.y==0){
		return FloatVector2d(searchPoint.x,A.y);
	}else{
		float Slope1 = (A.y - B.y) / (A.x - B.x);
		float Slope2 = float(-1) / Slope1;

		float x = ( -1 * Slope2 * searchPoint.x + searchPoint.y - A.y + Slope1 * A.x) / ( Slope1 - Slope2 );
		float y = Slope1 * ( x - A.x ) + A.y;
		
		FloatVector2d result;

		result.x = x;
		result.y = y;

		return result;
	}
}

FloatVector2d Math::lineIntersection(FloatVector2d lineAPoint1,FloatVector2d lineAPoint2,FloatVector2d lineBPoint1,FloatVector2d lineBPoint2){

	float x1=lineAPoint1.x;
	float y1=lineAPoint1.y;

	float x2=lineAPoint2.x;
	float y2=lineAPoint2.y;

	float x3=lineBPoint1.x;
	float y3=lineBPoint1.y;

	float x4=lineBPoint2.x;
	float y4=lineBPoint2.y;

	float a1 = y2-y1;
	float b1 = x1-x2;
	float c1 = x2*y1 - x1*y2;  //{ a1*x + b1*y + c1 = 0 is line 1 }

	float a2 = y4-y3;
	float b2 = x3-x4;
	float c2 = x4*y3 - x3*y4;  //{ a2*x + b2*y + c2 = 0 is line 2 }

	float denom = a1*b2 - a2*b1;
	if(denom == 0){
		return FloatVector2d();
	}

	float x =(b1*c2 - b2*c1)/denom;
	float y =(a2*c1 - a1*c2)/denom;

	return FloatVector2d(x,y);
}

float Math::maximum(float a,float b){
	if(a>b){return a;}
	return b;
}

float Math::minimum(float a,float b){
	if(a<b){return a;}
	return b;
}

Math::Math() : pi(3.14159265){

}
