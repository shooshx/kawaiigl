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
	WholeScreenQuad(GLWidget *glw, const Vec& _backCol = Vec(1.0, 1.0, 1.0)) 
		: Renderable(glw), backCol(_backCol) {}
	virtual void render();

	Vec backCol;
};



#endif // KRENDERABLE_H_INCLUDED