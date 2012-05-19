#include "Renderable.h"
#include "GLWidget.h"
#include "Document.h"
#include "OpenGL\Quadric.h"

void WholeScreenQuad::render(GLWidget *glw)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//gluOrtho2D(0, width(), height(), 0);
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glColor3fv(backCol.v);
	glBegin(GL_QUADS);

	glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 0.0f); 
	glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 0.0f); 
	if (texUnit > 1)
		glMultiTexCoord2f(GL_TEXTURE0 + texUnit, 0.0f, 0.0f); 
	glVertex2f(-1.0f, -1.0f);

	glMultiTexCoord2f(GL_TEXTURE0, 1.0f, 0.0f);
	glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 0.0f);
	if (texUnit > 1)
		glMultiTexCoord2f(GL_TEXTURE0 + texUnit, 1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);

	glMultiTexCoord2f(GL_TEXTURE0, 1.0f, 1.0f);
	glMultiTexCoord2f(GL_TEXTURE1, 1.0f, 1.0f);
	if (texUnit > 1)
		glMultiTexCoord2f(GL_TEXTURE0 + texUnit, 1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);

	glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 1.0f);
	glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 1.0f);
	if (texUnit > 1)
		glMultiTexCoord2f(GL_TEXTURE0 + texUnit, 0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);

	glEnd();

	glEnable(GL_DEPTH_TEST);
	if (glw->isUsingLight())
		glEnable(GL_LIGHTING);

	glPopMatrix(); // pop to the model view matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); // pop to the projection matrix
	glMatrixMode(GL_MODELVIEW);

}

// **************************************************************************************************************

ArrowRenderable::ArrowRenderable() {}

bool ArrowRenderable::init(const vector<string>& args, Document* doc)
{
	if (args.size() < 2)
		return false;
	// from
	IPoint *p = doc->m_kparser.creator()->lookupSymbol(args[0]);
	if (p == NULL)
		return false;
	from = p->getCoord();
	// to
	p = doc->m_kparser.creator()->lookupSymbol(args[1]);
	if (p == NULL)
		return false;
	to = p->getCoord();
	// color
	color = Vec3(doc->dispConf().materialCol);
	if (args.size() >= 3) {
		p = doc->m_kparser.creator()->lookupSymbol(args[2]);
		if (p != NULL)
			color = p->getCoord();
	}

	float len = from.to(to).length();

	barRadius = len*0.05;
	tipRadius = barRadius*2;
	slices = 20;
	tipLen = len*0.2;
	return true;
}

void ArrowRenderable::render(GLWidget *glw)
{
	glPushMatrix();
	//glLoadIdentity();
	glTranslatef(from.x, from.y, from.z);

	Vec3 direction = from.to(to);
	float len = direction.length();
	direction.unitize();

	Vec3 o(0,0,1);
	Vec3 c = Vec3::crossProd(o, direction);
	float l = Vec3::dotProd(o, direction);
	float ang = acos(l);
	glRotatef(degFromRad(ang), c.x, c.y, c.z);

	Quadric q;
	float barLen = len - tipLen;
	glColor3fv(color.v);
	gluCylinder(q.handle(), barRadius, barRadius, barLen, slices, 2);
	glTranslatef(0, 0, barLen);
	gluDisk(q.handle(), 0, tipRadius, slices, 1);
	gluCylinder(q.handle(), tipRadius, 0, tipLen, slices, 2);

	glPopMatrix();
}
