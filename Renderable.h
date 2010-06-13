#pragma once
#ifndef KRENDERABLE_H_INCLUDED
#define KRENDERABLE_H_INCLUDED

#include "MyLib/Vec.h"

class GLWidget;


class Renderable
{
public:
	Renderable(GLWidget *glw) : m_glw(glw) {}
	virtual ~Renderable() {}
	void setEnv(GLWidget *glw) { m_glw = glw; }

	virtual void render() = 0;

protected:
	GLWidget *m_glw;
};


class WholeScreenQuad : public Renderable
{
public:
	WholeScreenQuad(GLWidget *glw, int _texUnit, const Vec3& _backCol = Vec3(1.0, 1.0, 1.0)) 
		: Renderable(glw), backCol(_backCol), texUnit(_texUnit) {}

	WholeScreenQuad(GLWidget *glw, const Vec3& _backCol = Vec3(1.0, 1.0, 1.0), int _texUnit = 1) 
		: Renderable(glw), backCol(_backCol), texUnit(_texUnit) {}
	virtual void render();

	Vec3 backCol;
	uint texUnit;
};



#endif // KRENDERABLE_H_INCLUDED