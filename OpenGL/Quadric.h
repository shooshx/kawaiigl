#pragma once

#include "GL/glew.h"
#include "MyLib/Vec.h"

class Quadric
{
public:
	Quadric() :m_qobj(NULL)
	{
		init();
	}
	void init()
	{
		m_qobj = gluNewQuadric();
	}
	~Quadric()
	{
		if (m_qobj != NULL)
			gluDeleteQuadric(m_qobj);
	}

	GLUquadric* handle() { return m_qobj; }

	void drawSphere(const Vec3& p, float radius) const;

private:
	Q_DISABLE_COPY(Quadric);
	GLUquadric* m_qobj;

};
