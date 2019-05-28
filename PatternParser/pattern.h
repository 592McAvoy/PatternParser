#pragma once
#include<iostream>
#include<vector>
#include <string>
#include<algorithm>
#include<math.h>
#include<map>
#include<sstream>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

enum TYPE {
	Border, Mountain, Valley, Cut, Triangulation, Hinge, NONE
};

class Vertice
{	
public:
	float x, y, z;
	int id;
	Vertice(float _x, float _y, float _z)
		:x(_x), y(_y), z(_z) {}
	Vertice(float _x, float _y)
		:x(_x), y(_y), z(0) {}
	Vertice()
		:x(0), y(0), z(0) {}
	Vertice(const Vertice &other)
		:x(other.x), y(other.y), z(other.z), id(other.id){}
	bool operator==(const Vertice &other) const {
		return (other.x == this->x) && (other.y == this->y) && (other.z == this->z);
	}
};

class Edge
{
public:
	Vertice v1, v2;
	float angle;
	TYPE type;
	Edge(Vertice &_v1, Vertice &_v2, TYPE t)
		:v1(_v1), v2(_v2), angle(0), type(t) {}
	Edge(Vertice &_v1, Vertice &_v2, float a, TYPE t)
		:v1(_v1), v2(_v2), angle(a), type(t) {}
	bool operator==(const Edge &other) const{
		return (other.v1 == this->v1) && (other.v2 == this->v2) && (other.type == this->type);
	}
};

class Face
{
public:
	vector<Vertice> vts;
	Face(){}
	Face(vector<Vertice> v) :vts(v) {}
};



const float	VERT_TOL = 3.0f;	//vertex merge tolerance

class Pattern {
private:
	string SVGfilename;

	vector<Vertice> verticesRaw;
	vector<Edge> edgesRaw;
	vector<vector<Vertice>> verticeNeighbors;	// vertex id - neighbor ids
	vector<Face> facesRaw;

	void getElementList(vector<XMLElement*> &vec, XMLElement *root, string name);
	float getOpacityAngle(XMLElement* e);
	const string getStroke(XMLElement* e);
	TYPE typeForStroke(const string stroke);

	void parseLine(vector<XMLElement*> &vec);
	void parseRect(vector<XMLElement*> &vec);

	void findIntersections();
	void findVerticeNeighbors();
	void findFaces();

	void sortVerticeNeighbors();

	void loadSVG();
	void parseSVG();

public:
	vector<Edge> mountains;
	vector<Edge> valleys;
	vector<Edge> borders;
	vector<Edge> hinges;
	vector<Edge> triangulations;

	Pattern(string filename)
		:SVGfilename(filename){}
	
	void parse();
};





