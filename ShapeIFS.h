// Happy Cube Solver - Building and designing models for the Happy cube puzzles
// Copyright (C) 1995-2006 Shy Shalom, shooshX@gmail.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef __SHAPEIFS_H__INCLUDED__
#define __SHAPEIFS_H__INCLUDED__

#include "MyPolygon.h"
#include "Pool.h"

class PicGroupDef;
class ShapeIFS;
struct MyAllocator;
struct IPoint;

class MyObject // should be called MyMesh
{
public:
	MyObject(MyAllocator* alloc = NULL) 
		:m_alloc(alloc), nPolys(0), nLines(0), nPoints(0), 
		 nakedLinesColor(0.0),  verterxNormals(false)
	{}

	~MyObject()
	{
	}


	QVector<MyPolygon*> poly; // array sized nPoly of pointers to polygons
	QVector<MyLine> lines;   // array sized nLines
	QVector<MyPoint*> points; // array sized numPoint of pointers to the points
	QVector<MyPolygon*> triangles; // polygons of poly that are triangles (a subset of poly)

	int nPolys, nLines, nPoints;

	float nakedLinesColor; // color of the lines when the object is on its own (not in grpdef)
	bool verterxNormals; // use normals for every vertex (belongs more in the ifs.. but object is the drawing unit)

	MyPolygon* AddPoly(Vec3 *inplst, TexAnchor *ancs = NULL, Texture *tex = NULL); //copy needed vertices, add poly
	MyPolygon* AddPoly(const IPoint *p1, const IPoint *p2, const IPoint *p3, const IPoint *p4,
					   TexAnchor *ancs = NULL, Texture *tex = NULL);

	void AddLine(Vec3 *inp1, Vec3 *inp2, double inR, double inG, double inB, MyLine::ELineType type);
	void setNakedLineColor(float color) { nakedLinesColor = color; }
	void vectorify();
	bool clacNormals(bool vtxNormals);
	bool subdivide(bool smooth);

	enum ESaveWhat { SaveTriangles, SaveQuads, SaveEdges };
	void saveAs(QTextStream& out, const QString& format, ESaveWhat saveWhat = SaveTriangles);
	void detachPoints();
	void makePointNei();
	Vec3 findVtxCenter();
	Vec3 findBoxCenter();
	
private:
	///////////////////// structures for startup
	MyPoint* CopyCheckPoint(const Vec3 *p, const string* name = NULL);

	typedef QList<MyPolygon*> TPolyList;
	typedef QLinkedList<MyLine> TLineList;
	typedef QList<MyPoint*> TPointsList;
	TPolyList plylst; 
	TLineList lnlst;
	TPointsList pntlst; // points can come either from the list or the map.

	MyAllocator *m_alloc; // used to allocate points, polygons and halfedges

	// needed for comparison of points (what you don't want is an operator=(MyPoint*, MyPoint*) )
	// shallow wrapper to the pointer
	struct MyPointWrapper 
	{
		explicit MyPointWrapper(MyPoint *p) :ptr(p) {}
		void detach(MyAllocator *m_alloc) const;
		mutable MyPoint *ptr; 
		// this is a workaround the constness of the set. we know the hash code is not
		// going to change due to the detach
	};

	typedef QSet<MyPointWrapper> TPointsSet;
	TPointsSet m_tmppoints; // used in startup to accumilate points. 

	friend uint qHash(const MyObject::MyPointWrapper &pnt);
	friend bool operator==(const MyObject::MyPointWrapper &p1, const MyObject::MyPointWrapper &p2);

private:
	// add a point directly to the points repository, without duplicacy check.
	inline void basicAddPoint(MyPoint *pnt);
	// add a polygon made of points in the repository, without duplicacy checks.
	inline void basicAddPoly(MyPoint *inparr[], TexAnchor *ancs = NULL, Texture *tex = NULL);

	typedef QVector<HalfEdge*> THalfEdgeList;
	bool buildHalfEdges(THalfEdgeList& lst);

};


struct MyAllocator
{
	MyAllocator() {}
	MyAllocator(int points, int poly, int he)
		:m_pointsPool(points), m_polyPool(poly), m_hePool(he) {}
	bool isNull()
	{
		return m_pointsPool.isNull();
	}
	void init(int points, int poly, int he);
	void clear()
	{
		m_pointsPool.clear();
		m_polyPool.clear();
		m_hePool.clear();
	}
	void clearMaxAlloc()
	{
		m_pointsPool.clearMaxAlloc();
		m_polyPool.clearMaxAlloc();
		m_hePool.clearMaxAlloc();
	}
	void checkMaxAlloc() const;

	Pool<MyPoint> m_pointsPool; //(671100); //168100);
	Pool<MyPolygon> m_polyPool; //(891000); // TBD - divide to levels, for each subdivision
	Pool<HalfEdge> m_hePool; //(5000);

};


class ShapeIFS  
{
public:
	ShapeIFS() {}
	virtual ~ShapeIFS() {}

	MyObject& operator[](uint i) 
	{ 
		TObjectsMap::iterator it = m_objects.find(i);
		if (it == m_objects.end())
			it = m_objects.insert(i, MyObject()); // does the find again...  TBD-sort of redundant by now
		return *it; 
	}


	void vectorify();

// used directly via const iterators by drawIFS
	// TBD: VECTOR!
	typedef QHash<uint, MyObject> TObjectsMap;
	TObjectsMap m_objects;

};


#endif // __SHAPEIFS_H__INCLUDED__
