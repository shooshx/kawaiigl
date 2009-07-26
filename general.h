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

#ifndef GENERAL_DOT_H_CUBE_FILE
#define GENERAL_DOT_H_CUBE_FILE

#include <QtGlobal>
#include <QVector>
#include <string>
using namespace std;

#include "MyLib/Vec.h"

enum EPlane
{
	PLANE_NONE = -1,
	YZ_PLANE = 0,
	XZ_PLANE = 1,
	XY_PLANE = 2 
};

enum EAxis
{
	AXIS_NONE = -1,
	X_AXIS = 1,
	Y_AXIS = 2,
	Z_AXIS = 3
};


QString humanCount(qint64 n);
QString humanCount(double n, int pers);
void flushAllEvents(); // use this when visuals need to update

inline bool hXor(bool a, bool b)
{
	return ((a && !b) || (!a && b));
}
inline uint bXor(uint a, uint b)
{
	return (a & ~b) | (~a & b);
}

typedef QVector<int> TTransformVec;

// profile command line VC6
// /SF ?CubeEngineProc@@YAIPAX@Z

#endif