#include "Quadric.h"


void Quadric::drawSphere(const Vec3& p, float radius) const
{
	glPushMatrix();
	glTranslatef(p.x, p.y, p.z);
	int res = 10; 
	gluSphere(m_qobj, radius, res, res);
	glPopMatrix();
}

