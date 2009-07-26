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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__


#include <QtOpenGL>
#include <QImage>

class Texture									
{
public:
	Texture() :ind(-1) {}
	Texture(int _ind, const QImage& _img) :ind(_ind), img(_img) {}

	QImage img;
	int ind;	// index of for the textures table in the glwidget where we should find the proper id.
};	


#endif