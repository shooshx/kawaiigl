#pragma once
#ifndef KRENDERABLE_H_INCLUDED
#define KRENDERABLE_H_INCLUDED

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
	WholeScreenQuad(GLWidget *glw) : Renderable(glw) {}
	virtual void render();
};



#endif // KRENDERABLE_H_INCLUDED