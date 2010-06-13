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

#ifndef __POLYGON_H__INCLUDED__
#define __POLYGON_H__INCLUDED__

#include "MyPoint.h"
#include "general.h"
#include "Texture.h"

typedef IVec2 ITexAnchor;
typedef Vec2 TexAnchor;


inline bool operator==(const TexAnchor& a, const TexAnchor& b)
{
	return (a.x == b.x) && (a.y == b.y);
}

class MyPolygon  
{
public:

	MyPolygon(MyPoint* inVtx[], TexAnchor *inAncs, Texture *_tex)
	 : he(NULL), f(NULL), pnum(0)
	  ,tex(_tex)
	{	// point are shallow and anchors are deep copied
		init(inVtx, inAncs, _tex);
		++g_ctorCount;
	}

	MyPolygon() { ++g_ctorCount; } // doesn't do anything. waste of time.
	~MyPolygon() { ++g_dtorCount; }

	// two versions, to help the branch prediction :)
	void init(MyPoint* inVtx[], TexAnchor *inAncs, Texture *_tex)
	{
		for(int i = 0; i < 4; ++i)
		{
			vtx[i] = inVtx[i];
			texAncs[i] = inAncs[i];
		}
		tex = _tex;
		pnum = (vtx[3] != NULL)?4:3;
	}

	void init(TexAnchor *inAncs, Texture *_tex)
	{
		if (inAncs != NULL)
		{
			for(int i = 0; i < 4; ++i)
			{
				texAncs[i] = inAncs[i];
			}
		}
		tex = _tex;
	}

	bool calcNorm();
	bool calcTangents();

	MyPoint *vtx[4]; // a vector list of it's points in their order
	int pnum; // number of points
	
	Vec3 center; // normal of polygon

	TexAnchor texAncs[4]; // texture ancors by the order of 0,0 - 1,0 - 1,1 - 0,1
	Texture *tex; 
	// relevant only to half-textured pieces per-polygon texture, use sort for this to be efficient
	// NULL to disable texturing
	Vec3 tangent, bitangent;

	// TBD- move away from here.
	HalfEdge *he;	
	MyPoint *f; // the center of polygon point. (temporary TBD?)

	static int g_ctorCount, g_dtorCount;

};



#endif // __POLYGON_H__INCLUDED__
