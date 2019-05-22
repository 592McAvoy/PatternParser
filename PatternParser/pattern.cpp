#include "pattern.h"

/* Helper function */
static void debugElementList(vector<XMLElement*> &vec) {
	for (XMLElement* e : vec) {
		XMLPrinter p;
		e->Accept(&p);
		cout << p.CStr();
	}
}

static void debugEdgeList(vector<Edge> &vec) {
	cout << "length: " << vec.size() << endl;
	string types[] = {
	"Border", "Mountain", "Valley", "Cut", "Triangulation", "Hinge", "NONE"
	};
	for (Edge e : vec) {
		Vertice v1 = e.v1;
		Vertice v2 = e.v2;
		float angle = e.angle;
		cout << "type: " << types[e.type] << "\t"
			<< "v1 (" << v1.x << "," << v1.y << "," << v1.z << ")\t"
			<< "v2 (" << v2.x << "," << v2.y << "," << v2.z << ")\t"
			<< "angle: " << angle << endl;
	}
}

static void debugVerticeList(vector<Vertice> &vec) {
	cout << "length: " << vec.size() << endl;
	for (Vertice v : vec) {
		cout << "v (" << v.x << "," << v.y << "," << v.z << ")\n";
	}
}

static char easytolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

static bool compareVertice(Vertice v1, Vertice v2) {
	if (v1.x != v2.x)
		return v1.x < v2.x;
	if (v1.y != v2.y)
		return v1.y < v2.y;
	return v1.z < v2.z;
}

static bool compareEdge(Edge e1, Edge e2) {
	return compareVertice(e1.v1, e2.v1);
}

static void UniqueVertices(vector<Vertice> &vec) {
	sort(vec.begin(), vec.end(), compareVertice);
	vec.erase(unique(vec.begin(), vec.end()), vec.end());
}

static void UniqueEdges(vector<Edge> &vec) {
	sort(vec.begin(), vec.end(), compareEdge);
	vec.erase(unique(vec.begin(), vec.end()), vec.end());
}


// http://paulbourke.net/geometry/pointlineplane/
static void line_intersect(Vector2 &v1, Vector2 &v2, Vector2 &v3, Vector2 &v4, Vector2 &intersection, float &t1, float &t2) {
	float x1 = v1.x;
	float y1 = v1.y;
	float x2 = v2.x;
	float y2 = v2.y;
	float x3 = v3.x;
	float y3 = v3.y;
	float x4 = v4.x;
	float y4 = v4.y;

	float denom = (y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1);
	if (denom == 0) {
		return;
	}
	t1 = ((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / denom;
	t2 = ((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / denom;

	intersection.x = x1 + t1 * (x2 - x1);
	intersection.y = y1 + t1 * (y2 - y1);
	return;
}

static float getDistFromEnd(float t, float length, float tol) {
	float  dist = t * length;
	if (dist < -tol) return 0;
	if (dist > length + tol) return 0;
	return dist;
}






/* Class Methods*/

void Pattern::getElementList(vector<XMLElement*> &vec, XMLElement *root, string name) {
	XMLElement *tmp;
	if (root) {
		tmp = root->FirstChildElement(name.c_str());
		while (tmp) {
			vec.push_back(tmp);
			tmp = tmp->NextSiblingElement(name.c_str());
		}
	}
}

const string Pattern::getStroke(XMLElement* e) {
	const char* s = e->Attribute("stroke");
	if (!s) {
		XMLElement *t = e->FirstChildElement("style");
		if (!t || (s = t->Attribute("stroke"))) 
			return NULL;
	}
	string tmp(s);
	transform(tmp.begin(), tmp.end(), tmp.begin(), ::easytolower);
	return tmp;
}

float Pattern::getOpacityAngle(XMLElement* e) {
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

TYPE Pattern::typeForStroke(const string stroke) {
	if (stroke == "#000000" || stroke == "#000" || stroke == "black" || stroke == "rgb(0,0,0)") return TYPE::Border;
	if (stroke == "#ff0000" || stroke == "#f00" || stroke == "red" || stroke == "rgb(255,0,0)") return TYPE::Mountain;
	if (stroke == "#0000ff" || stroke == "#00f" || stroke == "blue" || stroke == "rgb(0,0,255)") return TYPE::Valley;
	if (stroke == "#00ff00" || stroke == "#0f0" || stroke == "green" || stroke == "rgb(0,255,0)") return TYPE::Cut;
	if (stroke == "#ffff00" || stroke == "#ff0" || stroke == "yellow" || stroke == "rgb(255,255,0)") return TYPE::Triangulation;
	if (stroke == "#ff00ff" || stroke == "#f0f" || stroke == "magenta" || stroke == "rgb(255,0,255)") return TYPE::Hinge;
	return TYPE::NONE;
}

void Pattern::parseLine(vector<XMLElement*> &vec) {
	for (XMLElement *e : vec) {
		float x1, y1, x2, y2;
		x1 = e->FloatAttribute("x1");
		y1 = e->FloatAttribute("y1");
		x2 = e->FloatAttribute("x2");
		y2 = e->FloatAttribute("y2");

		Vertice v1(x1, y1);
		Vertice v2(x2, y2);

		verticesRaw.push_back(v1);
		verticesRaw.push_back(v2);

		TYPE type = typeForStroke(getStroke(e));
		switch (type) {
		case Border:
		case Cut:
		case Triangulation:
		case Hinge:
			edgesRaw.push_back(Edge(v1, v2, type));
			break;
		case Mountain: {
			edgesRaw.push_back(Edge(v1,v2,-getOpacityAngle(e),type));
			break;
		}			
		case Valley: {
			edgesRaw.push_back(Edge(v1, v2, -getOpacityAngle(e), type));
			break;
		}
		case NONE:
			break;
		}
	}
}

void Pattern::parseRect(vector<XMLElement*> &vec) {
	for (XMLElement *e : vec) {
		float x, y, w, h;
		x = e->FloatAttribute("x");
		y = e->FloatAttribute("y");
		w = e->FloatAttribute("width");
		h = e->FloatAttribute("height");

		Vertice v1(x, y);
		Vertice v2(x + w, y);
		Vertice v3(x, y + h);
		Vertice v4(x + w, y + h);

		TYPE type = TYPE::Border;
		Edge e1(v1, v2, type);
		Edge e2(v1, v3, type);
		Edge e3(v2, v4, type);
		Edge e4(v3, v4, type);

		verticesRaw.push_back(v1);
		verticesRaw.push_back(v2);
		verticesRaw.push_back(v3);
		verticesRaw.push_back(v4);

		edgesRaw.push_back(e1);
		edgesRaw.push_back(e2);
		edgesRaw.push_back(e3);
		edgesRaw.push_back(e4);
	}
}

void Pattern::findIntersections() {
	int N = edgesRaw.size();
	for (int i = N - 1; i >= 0; i--) {
		for (int j = i - 1; j >= 0; j--) {
			Edge e1 = edgesRaw[i];
			Edge e2 = edgesRaw[j];
			Vector2 v1 = Vector2(e1.v1);
			Vector2 v2 = Vector2(e1.v2);
			Vector2 v3 = Vector2(e2.v1);
			Vector2 v4 = Vector2(e2.v2);

			Vector2 intersection;
			Vector2 empty;
			float t1, t2;
			line_intersect(v1, v2, v3, v4, intersection, t1, t2);

			if (intersection == empty) continue;

			float length1 = v1.distance(v2);
			float length2 = v3.distance(v4);

			float d1 = getDistFromEnd(t1, length1, VERT_TOL);
			float d2 = getDistFromEnd(t2, length2, VERT_TOL);

			if (d1 == 0 || d2 == 0) continue;	//no crossing

			bool seg1Int = d1 > VERT_TOL && d1 < length1 - VERT_TOL;
			bool seg2Int = d2 > VERT_TOL && d2 < length2 - VERT_TOL;

			if (!seg1Int && !seg2Int) continue;	//intersects at endpoints only

			Vertice *point = NULL;
			if (seg1Int && seg2Int) {
				point = &Vertice(intersection.x, intersection.y);
				verticesRaw.push_back(*point);
			}
			else if (seg1Int) {
				if (d2 <= VERT_TOL) point = &Vertice(v3.x, v3.y);
				else point = &Vertice(v4.x, v4.y);
			}
			else {
				if (d1 <= VERT_TOL) point = &Vertice(v1.x, v1.y);
				else point = &Vertice(v2.x, v2.y);
			}

			if (seg1Int) {
				edgesRaw.erase(edgesRaw.begin() + i);
				edgesRaw.insert(edgesRaw.begin() + i, Edge(*point, e1.v1, e1.angle, e1.type));
				edgesRaw.insert(edgesRaw.begin() + i + 1, Edge(*point, e1.v2, e1.angle, e1.type));
				i++;
			}
			if (seg2Int) {
				edgesRaw.erase(edgesRaw.begin() + j);
				edgesRaw.insert(edgesRaw.begin() + j, Edge(*point, e2.v1, e2.angle, e2.type));
				edgesRaw.insert(edgesRaw.begin() + j + 1, Edge(*point, e2.v2, e2.angle, e2.type));
				i++;
				j++;
			}
		}
	}
}

void Pattern::loadSVG() {
	XMLDocument svg;
	svg.LoadFile(SVGfilename.c_str());

	if (svg.ErrorID() != 0) {
		cout << "Load svg: " << SVGfilename << " ERROR!" << endl;
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
}

void Pattern::parseSVG() {
	// remove duplicate vertices and edges
	UniqueVertices(verticesRaw);
	UniqueEdges(edgesRaw);

		//debugEdgeList(edgesRaw);
		//debugVerticeList(verticesRaw);
	findIntersections();

	// remove duplicate vertices and edges
	UniqueVertices(verticesRaw);
	UniqueEdges(edgesRaw);

		//debugEdgeList(edgesRaw);
		//debugVerticeList(verticesRaw);

}

void Pattern::parse() {
	loadSVG();

	
	parseSVG();	
	
}

int main() {
	Pattern p("../assets/Bases/birdBase.svg");
	p.parse();
	cin.get();
}