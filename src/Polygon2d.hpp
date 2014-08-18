#ifndef POLYGON_H
#define POLYGON_H 

#include "script.hpp"
#include "Containers.hpp"
#include "objects.hpp"

class SceneGraphNode;

enum Polygon2dMode{
	P2D_POLYGON,
	P2D_LINE,
	P2D_POINT
};

class Polygon2d{
	private:
		Array<FloatVector3d> vertices;

		Array<Vertex>* vertexList;

		Polygon2dMode mode;

		bool vboset;

		unsigned int buffer;

	public:
		float pointSize;
		FloatVector3d pos;
		FloatVector3d scale;
		FloatVector4d color;
		
		String name;
		bool visible;
		
		bool percent;

		SceneGraphNode* sceneGraphNode;

		void draw();
		void clearVertices();
		void addVertex(FloatVector3d pos);
		bool stipple;

		void addLineVertexList(Array<Vertex>*);
		void addPointVertexList(Array<Vertex>*);
		
		Polygon2d();
};


#endif

