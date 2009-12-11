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

#ifndef __POINT_H__INCLUDED__
#define __POINT_H__INCLUDED__

#include "general.h"

class MyPoint;
class MyPolygon;

// a half edge originates from a point and points to a point
class HalfEdge
{
public:
	HalfEdge() :point(NULL), pair(NULL), poly(NULL), next(NULL), edgePoint(NULL) {}
	HalfEdge(MyPolygon *_poly, MyPoint *_pnt, HalfEdge *_next)
		:point(_pnt), pair(NULL), poly(_poly), next(_next), edgePoint(NULL) 
	{}
	void init(MyPolygon *_poly, MyPoint *_pnt, HalfEdge *_next)
	{ 
		point = _pnt; 
		pair = NULL; 
		poly = _poly; 
		next = _next; 
		edgePoint = NULL; 
	}

	MyPoint* point; // originates from
	HalfEdge* pair;
	MyPolygon* poly; 
	HalfEdge* next;

	MyPoint* edgePoint; // always shared between two edges.
};

class DefPoint; // ref to point symbol

class MyPoint  
{
public:
	MyPoint(float inX = 0.0, float inY = 0.0, float inZ = 0.0)
		:p(inX, inY, inZ), he(NULL), touched(false), col(INVALID_COLOR), name(NULL) { ++g_ctorCount; }
	MyPoint(const MyPoint& a)
		:p(a.p), he(NULL), touched(false), col(INVALID_COLOR), name(a.name) { ++g_ctorCount; }
	explicit MyPoint(const Vec& c) 
		:p(c), he(NULL), touched(false), col(INVALID_COLOR), name(NULL) { ++g_ctorCount; }

	~MyPoint() { ++g_dtorCount; }

	void copyTo(MyPoint& to) const
	{
		to.p = p; to.n = n; to.col = col; 
		to.index = index;
		to.he = NULL;
		to.touched = false;
		to.name = name;
	}
	
	void clear() { p.clear(); n.clear(); name = NULL; }
	void setp(const Vec &c) { p = c; }
	uint hash() const { const uint *up = reinterpret_cast<const uint*>(p.ptr()); return (bXor(up[0], bXor(up[1] >> 8, up[2] >> 16))); }

	MyPoint &operator+=(const MyPoint& a) { p += a.p; return *this; }
	MyPoint &operator/=(float s) { p /= s; return *this; }
	MyPoint &operator*=(float s) { p *= s; return *this; }
	MyPoint &operator=(const MyPoint& a)
	{ 
		p = a.p;
		name = a.name;
		return *this; 
	}

	Vec p;
	Vec n; // normal.
	Vec tangent, bitangent;

	int index;
	Vec col;
	const std::string* name; // a non owning pointer to the ast parsed tree

	// managment
	static int g_ctorCount, g_dtorCount;


	// temporarys
	HalfEdge *he;
	bool touched; // used in subdivide (was this point adjusted in the current subdiv iteration?)
};

inline MyPoint operator+(const MyPoint& a, const MyPoint& b) { MyPoint r(a); r += b; return r; }
inline MyPoint operator/(const MyPoint& a, float s) { MyPoint r(a); r /= s; return r; }
inline MyPoint operator*(float s, const MyPoint& a) { MyPoint r(a); r *= s; return r; }
inline MyPoint operator*(const MyPoint& a, float s) { MyPoint r(a); r *= s; return r; }
inline bool operator==(const MyPoint &p1, const MyPoint &p2) { return (Vec::exactEqual(p1.p, p2.p)); }

class MyLine  
{
public:
	Vec color;
	MyPoint *p1, *p2; 

	enum ELineType
	{
		LINE_ALWAYS, // line which is always shown
		LINE_ONLY_WHOLE, // line which appears only when the whole thing is shown
		LINE_ONLY_LONE // line which appears only when the lone piece is shown
	} type;

	MyLine(MyPoint *_p1, MyPoint *_p2, float clR, float clG, float clB, ELineType _type)
	: p1(_p1), p2(_p2), color(clR, clG, clB), type(_type)	{}
	MyLine() {} // doesn't do anything. waste of time.
};

#endif // __POINT_H__INCLUDED__
