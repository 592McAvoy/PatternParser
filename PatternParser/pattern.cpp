#include<iostream>
#include<vector>
#include <string>
#include<algorithm>
#include<math.h>

#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

enum TYPE {
	Border, Mountain, Valley, Cut, Triangulation, Hinge, NONE
};

struct Vertice
{
	int x, y, z;
	Vertice(int _x, int _y, int _z)
		:x(_x),y(_y),z(_z){}
	Vertice(int _x, int _y)
		:x(_x), y(_y), z(0) {}
};
struct Edge
{
	Vertice v1, v2;
	float angle;
	Edge(Vertice _v1, Vertice _v2)
		:v1(_v1), v2(_v2),angle(0){}
	Edge(Vertice _v1, Vertice _v2, float a)
		:v1(_v1), v2(_v2), angle(a) {}
};

static void getElementList(vector<XMLElement*> &vec, XMLElement *root, string name);
static void debugElementList(vector<XMLElement*> &vec);
static const string getStroke(XMLElement* e);
static TYPE typeForStroke(const string stroke);


vector<Vertice> verticesRaw;
//refs to vertex indices
vector<Edge> mountainsRaw;
vector<Edge> valleysRaw;
vector<Edge> bordersRaw;
vector<Edge> cutsRaw;
vector<Edge> triangulationsRaw;
vector<Edge> hingesRaw;

const float  PI_F = 3.14159265358979f;

static void getElementList(vector<XMLElement*> &vec, XMLElement *root, string name) {
	XMLElement *tmp;
	if (root) {
		tmp = root->FirstChildElement(name.c_str());
		while (tmp) {
			vec.push_back(tmp);
			tmp = tmp->NextSiblingElement(name.c_str());
		}
	}
}

static void debugElementList(vector<XMLElement*> &vec) {	
	for (XMLElement* e : vec) {
		XMLPrinter p;
		e->Accept(&p);
		cout << p.CStr();
	}
}

static void debugEdgeList(vector<Edge> &vec) {
	for (Edge e : vec) {
		Vertice v1 = e.v1;
		Vertice v2 = e.v2;
		float angle = e.angle;
		cout << "v1 (" << v1.x << ","<<  v1.y << ","<< v1.z << ")\t"
			<< "v2 (" << v2.x << "," << v2.y << "," << v2.z << ")\t"
			<< "angle: " << angle << endl;
	}
}

char easytolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}
static const string getStroke(XMLElement* e) {
	const char* s = e->Attribute("stroke");
	if (!s) {
		XMLElement *t = e->FirstChildElement("style");
		if (!t || (s = t->Attribute("stroke"))) 
			return NULL;
	}
	string tmp(s);
	transform(tmp.begin(), tmp.end(), tmp.begin(), ::easytolower);
	cout << tmp << endl;
	return tmp;
}

static float getOpacityAngle(XMLElement* e) {
	float opa = e->FloatAttribute("opacity");
	if (opa <= 0) {
		XMLElement *t = e->FirstChildElement("style");
		if (t)
			opa = t->FloatAttribute("opacity");	
	}
	if (opa <= 0) {
		opa = e->FloatAttribute("stroke-opacity");
		if (opa <= 0) {
			XMLElement *t = e->FirstChildElement("style");
			if (t)
				opa = t->FloatAttribute("stroke-opacity");
		}
	}
	return opa * PI_F;
}

static TYPE typeForStroke(const string stroke) {
	if (stroke == "#000000" || stroke == "#000" || stroke == "black" || stroke == "rgb(0,0,0)") return TYPE::Border;
	if (stroke == "#ff0000" || stroke == "#f00" || stroke == "red" || stroke == "rgb(255,0,0)") return TYPE::Mountain;
	if (stroke == "#0000ff" || stroke == "#00f" || stroke == "blue" || stroke == "rgb(0,0,255)") return TYPE::Valley;
	if (stroke == "#00ff00" || stroke == "#0f0" || stroke == "green" || stroke == "rgb(0,255,0)") return TYPE::Cut;
	if (stroke == "#ffff00" || stroke == "#ff0" || stroke == "yellow" || stroke == "rgb(255,255,0)") return TYPE::Triangulation;
	if (stroke == "#ff00ff" || stroke == "#f0f" || stroke == "magenta" || stroke == "rgb(255,0,255)") return TYPE::Hinge;
	return TYPE::NONE;
}

static void parseLine(vector<XMLElement*> &vec) {
	for (XMLElement *e : vec) {
		int x1, y1, x2, y2;
		x1 = e->IntAttribute("x1");
		y1 = e->IntAttribute("y1");
		x2 = e->IntAttribute("x2");
		y2 = e->IntAttribute("y2");

		Vertice v1(x1, y1);
		Vertice v2(x2, y2);
		Edge edge(v1, v2);

		verticesRaw.push_back(v1);
		verticesRaw.push_back(v2);

		switch (typeForStroke(getStroke(e))) {
		case Border:
			bordersRaw.push_back(edge);
			break;
		case Cut:
			cutsRaw.push_back(edge);
			break;
		case Triangulation:
			triangulationsRaw.push_back(edge);
			break;
		case Hinge:
			hingesRaw.push_back(edge);
			break;
		case Mountain: {
			Edge e1(v1, v2, -getOpacityAngle(e));
			mountainsRaw.push_back(e1);
			break;
		}			
		case Valley: {
			Edge e1(v1, v2, getOpacityAngle(e));
			valleysRaw.push_back(e1);
			break;
		}
		case NONE:
			break;
		}
	}
}

static void parseRect(vector<XMLElement*> &vec) {
	for (XMLElement *e : vec) {
		int x, y, w, h;
		x = e->IntAttribute("x");
		y = e->IntAttribute("y");
		w = e->IntAttribute("width");
		h = e->IntAttribute("height");

		Vertice v1(x, y);
		Vertice v2(x + w, y);
		Vertice v3(x, y + h);
		Vertice v4(x + w, y + h);
		Edge e1(v1, v2);
		Edge e2(v1, v3);
		Edge e3(v2, v4);
		Edge e4(v3, v4);

		verticesRaw.push_back(v1);
		verticesRaw.push_back(v2);
		verticesRaw.push_back(v3);
		verticesRaw.push_back(v4);

		bordersRaw.push_back(e1);
		bordersRaw.push_back(e2);
		bordersRaw.push_back(e3);
		bordersRaw.push_back(e4);
	}
}

void loadSVG(string filename) {
	XMLDocument svg;
	svg.LoadFile(filename.c_str());

	if (svg.ErrorID() != 0) {
		cout << "Load svg: " << filename << " ERROR!" << endl;
		return;
	}

	XMLElement *root = svg.FirstChildElement("svg");

	vector<XMLElement*> paths, lines, rects, polygens, polylines;
	getElementList(paths, root, "path");
	getElementList(lines, root, "line");
	getElementList(rects, root, "rect");
	getElementList(polygens, root, "polygen");
	getElementList(polylines, root, "polyline");
	
	parseLine(lines);
	parseRect(rects);

	cout << "borders:\n";
	debugEdgeList(bordersRaw);
	cout << "mountains:\n";
	debugEdgeList(mountainsRaw);
	cout << "valleys:\n";
	debugEdgeList(valleysRaw);
}

int main() {
	loadSVG("../assets/Bases/birdBase.svg");
	cin.get();
}