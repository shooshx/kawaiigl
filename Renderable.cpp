#include "Renderable.h"
#include "GLWidget.h"

void WholeScreenQuad::render()
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
	if (m_glw->isUsingLight())
		glEnable(GL_LIGHTING);

	glPopMatrix(); // pop to the model view matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); // pop to the projection matrix
	glMatrixMode(GL_MODELVIEW);

}