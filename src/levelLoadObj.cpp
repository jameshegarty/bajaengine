#include "level.hpp"
#include "script.hpp"
#include <fstream>
#include "editor.h"

void createObjDefaultObj(ObjectAddress* obj){

		String name="unnamed";

		if(level->objectExists(name)){

			int i=0;

			while(level->objectExists(name+String(i))){

				i++;
			}
		}

		*obj=level->add(name,"polymesh");

		obj->object->material=0;
		obj->object->visible=true;

		level->defaultMaterial();
}

bool Level::loadObj(Path file){

	if(!os().fileExists(file)){

		console().write("obj file '"+file.getAbsolute()+"' not found");

#ifdef DEVELOPER
		
#endif

		return false;
	}

	std::ifstream in;
	in.open(file.getAbsolute().c_str(),std::ios::in);

	std::string line;
	String s;
	ObjectAddress obj;
	obj.type=NULLOBJECT;

	Array<FloatVector3d> ivertices;
	Array<FloatVector3d> inor;
	Array<FloatVector2d> iuv;

	int vertexcnt=0;

	int lnnum=0;

	while(getline(in, line)){
		lnnum++;

		s=line;
		if(s.substrIndex(0,2)=="g " || s.substrIndex(0,2)=="o "){

			if(obj.type!=NULLOBJECT){
				obj.object->setVertexCount(vertexcnt);
				obj.object->getVertexRaw(0).end=START_TRI;
				obj.object->getVertexRaw(vertexcnt-1).end=END_TRI;
			}

			vertexcnt=0;

			String name=s.substrIndex(2,s.size());
			obj=level->add(name,"polymesh");

			if(obj.type==NULLOBJECT){
				console().write("error, coulden't open obj because object '"+name+"' already exists");
				return false;
			}

			obj.object->material=0;
			obj.object->visible=true;

			defaultMaterial();
		}else if(s.substrIndex(0,2)=="v "){

			if(obj.type==NULLOBJECT){createObjDefaultObj(&obj);}
			float x,y,z;
			sscanf(s.c_str(),"v %f %f %f",&x,&y,&z);

			ivertices.pushBack(FloatVector3d(x,y,z));

		}else if(s.substrIndex(0,3)=="vn "){

			if(obj.type==NULLOBJECT){createObjDefaultObj(&obj);}

			float x,y,z;
			sscanf(s.c_str(),"vn %f %f %f",&x,&y,&z);

			inor.pushBack(FloatVector3d(x,y,z));
		}else if(s.substrIndex(0,3)=="vt "){

			if(obj.type==NULLOBJECT){createObjDefaultObj(&obj);}

			float x,y,z;
			sscanf(s.c_str(),"vt %f %f %f",&x,&y,&z);

			iuv.pushBack(FloatVector2d(x,y));
		}else if(s.substrIndex(0,2)=="f "){

			if(obj.type==NULLOBJECT){createObjDefaultObj(&obj);}

			String sub=s.substrIndex(2,s.size());
			Array<String> fc=sub.explode(" ");

			if(fc.size()==3 || fc.size()==4){
				int iter=1;
				if(fc.size()==4){iter=2;}

				for(int j=0; j<iter; j++){
					for(int i=0; i<3; i++){

						int realIndex=i;

						if(j==1){
							if(i==1){
								realIndex=2;
							}else if(i==2){
								realIndex=3;
							}
						}

						Array<String> f1=fc[realIndex].explode("/");

						if(f1.size()==1){
							Vertex v;

							if(f1[0]!=""){
								int ind=-1;
								sscanf(f1[0].c_str(),"%d",&ind);
								v.pos=ivertices[ind-1];
							}

							vertexcnt++;
							obj.object->addVertex(v);
							obj.object->getVertexRaw(vertexcnt-1).end=0;

						}else if(f1.size()==3){
							Vertex v;

							if(f1[0]!=""){
								int ind=-1;
								sscanf(f1[0].c_str(),"%d",&ind);
								v.pos=ivertices[ind-1];
							}

							if(f1[1]!=""){
								int ind=-1;
								sscanf(f1[1].c_str(),"%d",&ind);
								v.uvLightmap=iuv[ind-1];
								v.uv0=iuv[ind-1];
								v.uv1=iuv[ind-1];
								v.uv2=iuv[ind-1];
								v.uv3=iuv[ind-1];
								v.uv4=iuv[ind-1];
								v.uv5=iuv[ind-1];
								v.uv6=iuv[ind-1];
							}

							if(f1[2]!=""){
								int ind=-1;
								sscanf(f1[2].c_str(),"%d",&ind);
								v.normal=inor[ind-1];
							}

							vertexcnt++;
							obj.object->addVertex(v);
							obj.object->getVertexRaw(vertexcnt-1).end=0;
						}else{
							console().write("face dropped, incorrect index size, should only include vertex,uv, and normal indices");
						}
					}
				}
			}else{
				console().write("Error: non-triangle polygon, has "+String(fc.size())+" sides");
				return false;
			}

		}else if(s.substrIndex(0,7)=="mtllib "){

		}else if(s.substrIndex(0,7)=="usemtl "){
			
		}else if(s.substrIndex(0,2)=="s "){

		}else if(s.substrIndex(0,1)=="#"){

		}else{

			console().write("unknown .obj file content line "+String(lnnum));
			return false;
		}
	}

	if(obj.type!=NULLOBJECT){
		obj.object->setVertexCount(vertexcnt);
		obj.object->getVertexRaw(0).end=START_TRI;
		obj.object->getVertexRaw(vertexcnt-1).end=END_TRI;
	}

	in.close();

	return true;
}