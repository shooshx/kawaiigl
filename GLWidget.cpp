
#include "GLWidget.h"

#include <QtGui>
#include <QtOpenGL>
#include <QActionGroup>

using namespace boost;

QGLFormat g_format;

// used to promote stuff to public
// an instance of this is never really constructed. used only for case. that's QT for you.
class PatchQGLContext :public QGLContext
{
public:
	PatchQGLContext() :QGLContext(QGLFormat()) {}
	void generateFontDisplayLists(const QFont & fnt, int listBase)
	{
		QGLContext::generateFontDisplayLists(fnt, listBase);
	}
	bool initialized () const
	{
		return QGLContext::initialized();
	}
};

double zoomFactor(double perc)
{
	if (perc <= 1)
		return perc*perc;
	else
		return pow(3, perc) - 2;
}


struct PaintTransformations
{
	PaintTransformations(GLWidget* glw)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		float mat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		glLoadIdentity();
		
		//Vec3 center = (glw->aqmax + glw->aqmin)/2;
		//glTranslated(-center.x, -center.y, -center.z); // ??

		if (glw->m_renderFromLight == -1)
		{
			Vec3 camera = Vec3(0,0,4);
			gluLookAt(camera.x, camera.y, camera.z, 0, 0, 0, 0, 1, 0);

			glMultMatrixf(mat);

			double zv = zoomFactor(glw->m_zoomVal / 100.0);
			glScaled(zv, zv, zv);
		}
		else
		{
			gluLookAt(glw->m_lightPos.x, glw->m_lightPos.y, glw->m_lightPos.z, 0, 0, 0, 0, 1, 0);
		}
	}

	~PaintTransformations()
	{
		glPopMatrix();
	}
};

GLWidget::GLWidget(QWidget *parent, QGLWidget *sharefrom)
	: QGLWidget(g_format, parent, sharefrom), m_minScaleReset(0), m_cxClient(1), m_cyClient(1)
{
	m_viewState = Ortho;
	m_mouseAct = Rotate;
	m_axis = XYaxis;
	m_transformType = WorldSpace;

	m_bBackFaceCulling = true; 
	m_fUseLight = false;
	m_bSkewReset = true;

	m_nRotSens = 25;
	m_nScaleSens = 50;
	m_nTransSens = 100;

	m_bTimerMode = false;
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(rotateTimeout()));
	m_bContinuous = false;

	m_osf = 1.0;

	// continous mode control
	m_nXDelt = 0;
	m_nYDelt = 0;

	// PATCH
	aqmin = Vec3(-3, -3, -3);
	aqmax = Vec3(3, 3, 3);

	m_zoomVal = 100;
	m_lightPos = Vec3(0.0f, 0.0f, 100.0f);
	m_pointLight = false;

	m_lightAmbient = Vec3(0.1f, 0.1f, 0.1f);
	m_lightDiffuse = Vec3(0.9f, 0.9f, 0.9f);
	m_lightSpecular = Vec3(0.0f, 0.0f, 0.0f);
	m_materialShininess = 120;

	m_clipView = 0.0f;
	m_renderFromLight = -1;
}

void GLWidget::resetLight()
{
	m_lightPos = Vec3(0.0f, 0.0f, 100.0f);
	reCalcLight();
	updateGL();
}

void GLWidget::setClipValue(int percent) 
{ 
	//m_clipView = percent / 100.0f; 
	//reCalcProj(); 
	//updateGL();
}


void GLWidget::doBindTexture(int index, QImage* img)
{
	makeCurrent();

	while (index >= m_textures.size())
		m_textures.append(shared_ptr<GlTexture>());

	if (m_textures[index].get() != NULL)
	{
		m_textures[index]->unbind(); // it might not be deleted because it is help by someone else, unbind it anyway.
		m_textures[index].reset(); // deletes the old one
	}

	if (img != NULL && !img->isNull())
	{
		// this also binds the texture
		m_textures[index].reset(new GlTexture(context(), &img->copy(), GL_TEXTURE_2D));
		// not using my GlTexture because we want mipmaps
		// copy is ugly patch due to QT image caching bug
	}
	
}

// I made it for you.
void GLWidget::doTakeTexture(int index, shared_ptr<GlTexture> tex)
{
	makeCurrent();

	while (index >= m_textures.size())
		m_textures.append(shared_ptr<GlTexture>());

	m_textures[index].reset(); // delete old one
	m_textures[index] = tex;
	glBindTexture(tex->target(), tex->handle());
}



#define GL_BGRA 0x80E1

void GLWidget::doUpdateTexture(int index, QImage* img)
{
	makeCurrent();
	QImage cimg = *img;
	GlTexture &tex = *m_textures[index].get();

	if (!tex.isValid() || !glIsTexture(tex.handle()))
	{
		qDebug("invalid texture id");
		return; // ha?? shouldn't happen.
	}

	tex.bind();
	
	// this is going arount QT to interface directly with the textures, causing an inversion of the image
	// there is currently no way to do this with QT
	glTexSubImage2D(tex.target(), 0, 0, 0, cimg.width(), cimg.height(), GL_BGRA, GL_UNSIGNED_BYTE, cimg.bits());


	updateGL();
}

void GLWidget::Update3dActs()
{
	//selectGroupAct(m_main->m_viewFrustrumActs, m_viewState)->trigger();
	//selectGroupAct(m_main->m_actionsActs, m_mouseAct)->trigger();
	//selectGroupAct(m_main->m_axisActs, m_axis)->trigger();
	//selectGroupAct(m_main->m_spaceActs, m_transformType)->trigger();
	//m_main->m_continueModeAct->setChecked(
}



void GLWidget::initializeGL()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
	}


	// specify black as clear color
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	// specify the back of the buffer as clear depth (0 closest, 1 farmost)
	glClearDepth( 1.0f );
	// enable depth testing (Enable zbuffer - hidden surface removal)
	glEnable(GL_DEPTH_TEST);
	if (m_bBackFaceCulling) 
	{
		glEnable(GL_CULL_FACE);
	}

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(0x809D);  //GL_MULTISAMPLE
//	glEnable(0x809F);  //GL_SAMPLE_ALPHA_TO_ONE
//	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); has no effect


	DoReset();

}


GLWidget::~GLWidget()
{
}

void GLWidget::resizeGL(int width, int height)
{
	m_cxClient = width;
	m_cyClient = height;

	reCalcProj(true);
}


void GLWidget::reCalcProj(bool fFromScratch) // = true default
{

	if (fFromScratch)
	{
		double figX = qMax(aqmax[0] - aqmin[0], (float)m_minScaleReset),
			   figY = qMax(aqmax[1] - aqmin[1], (float)m_minScaleReset),
			   figZ = qMax(aqmax[2] - aqmin[2], (float)m_minScaleReset);

		scrScale = qMin(qMin(4/figX, 4/figY), 4/figZ)*0.7;
		realScale = qMin(qMin(m_cxClient/figX, m_cyClient/figY), m_cxClient/figZ)*0.7; // for translate

		m_AspectRatio = (GLdouble)m_cxClient/(GLdouble)m_cyClient;

		::glViewport(0, 0, m_cxClient, m_cyClient);
		::glMatrixMode(GL_PROJECTION);
		::glLoadIdentity();
	}

	if (m_viewState == Perspective)
	{
		float znear = 0.1f;
		float zfar = 8.0f;

		float clipznear = znear; //(zfar - znear) * m_clipView + znear;

		gluPerspective(60.0, m_AspectRatio, clipznear, zfar);

		//glTranslated(0.0, 0.0, -4);

	}
	else
	{
		// This viewing projection gives us a constant aspect ration. This is done by
		// increasing the corresponding size of the ortho cube.

		double windowSize = 4;	// the size of the window in GL coord system.
		float znear = -windowSize*m_AspectRatio;
		float zfar = windowSize*m_AspectRatio;

		float clipznear = (zfar - znear) * m_clipView + znear;

		if ( m_AspectRatio > 1 ) 
		{	
			glOrtho(-windowSize * m_AspectRatio / 2.0, windowSize * m_AspectRatio / 2.0,
				-windowSize/2.0, windowSize/2.0,
				clipznear, zfar);
		} 
		else 
		{
			glOrtho(-windowSize / 2.0, windowSize / 2.0, 
				-windowSize / m_AspectRatio / 2.0, windowSize/m_AspectRatio / 2.0,
				clipznear, zfar);
		}


	}
	glScaled(scrScale, scrScale, scrScale);

	glMatrixMode(GL_MODELVIEW);

}



void GLWidget::setUsingLight(bool use) 
{ 
	if (m_fUseLight == use)
		return;
	m_fUseLight = use; 

	if (!((PatchQGLContext*)(context()))->initialized())
		return; // can happen if we go to options before going to the solution view
	makeCurrent();
	reCalcLight(); 
	updateGL();
}

void GLWidget::setLightPos(const Vec3 &c)
{
	if (Vec3::exactEqual(m_lightPos, c))
		return;
	m_lightPos = c; 

	if (!((PatchQGLContext*)(context()))->initialized())
		return; // can happen if we go to options before going to the solution view
	makeCurrent();
	reCalcLight(); 
	updateGL();
}

// void GLWidget::setLightColor(const Vec3& c)
// {
// 	if (m_lightColor == c)
// 		return;
// 	m_lightColor = c; 
// 
// 	if (!((PatchQGLContext*)(context()))->initialized())
// 		return; // can happen if we go to options before going to the solution view
// 
// 	makeCurrent();
// 	reCalcLight(); 
// 	updateGL();
// 
// }


void GLWidget::reCalcLight()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (m_fUseLight)
	{
		glEnable(GL_LIGHTING);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHT0);
	//	glEnable(GL_LIGHT1);
		//glEnable(GL_NORMALIZE);
		glDisable(GL_NORMALIZE);
		glEnable(0x803A); // GL_RESCALE_NORMAL needed due to possible zoom

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		float no_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};

		//float ly_ambient[] = {m_lightAmbient, m_lightAmbient, m_lightAmbient, 1.0f};
		//float ly_diffuse[] = {m_lightColor[0] * m_lightDiffuse, m_lightColor[1] * m_lightDiffuse, m_lightColor[2] * m_lightDiffuse, 1.0f};
		//float ly_specular[] = {m_lightSpecular, m_lightSpecular, m_lightSpecular, 1.0f};

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, no_ambient);

		glLightfv(GL_LIGHT0, GL_AMBIENT, Vec4(m_lightAmbient, 1.0f).v);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Vec4(m_lightDiffuse, 1.0f).v);
		glLightfv(GL_LIGHT0, GL_SPECULAR, Vec4(m_lightSpecular, 1.0f).v);

		//glLightfv(GL_LIGHT1, GL_AMBIENT, ly_ambient);
		//glLightfv(GL_LIGHT1, GL_DIFFUSE, ly_diffuse);
		//glLightfv(GL_LIGHT1, GL_SPECULAR, ly_specular);

		float mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // no specular.
		
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_materialShininess);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Vec4(m_materialDiffAmb, 1.0f).v);


		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glLoadIdentity();
		//glTranslatef(0, 0, -4);
		//PaintTransformations ptran(this);  don't want this here - includes rotations

		float position[] = {m_lightPos.x, m_lightPos.y, m_lightPos.z, m_pointLight?1.0f:0.0f}; 
		glLightfv(GL_LIGHT0, GL_POSITION, position);

		//float position2[] = {-m_lightPos[0], -m_lightPos[1], -m_lightPos[2], 1.0f};
		//glLightfv(GL_LIGHT1, GL_POSITION, position2);

		glPopMatrix();

	}
	else
	{
		glDisable(GL_LIGHTING);
	}

}

void GLWidget::DoReset()
{
	m_osf = 1.0;
	makeCurrent();	
	m_zoomVal = 100;

	reCalcProj(true);
	reCalcLight();
	// now select the modelview matrix and clear it
	// this is the mode we do most of our calculations in
	// so we leave it as the default mode.

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	

	// this is controvercial.
	if (m_bSkewReset)
	{
		rotate(XYaxis, -20, 20);
		rotate(Zaxis, -5, 0);
	}
	
	emit zoomChanged(m_zoomVal);
	emit callReset();
} 

void GLWidget::setNewMinMax(const Vec3& min, const Vec3& max, bool scale)
{
	aqmin = min; aqmax = max;
	if (scale)
	{
		reCalcProj();
	}

}



void GLWidget::paintGL()
{
	PaintTransformations ptran(this);
	myPaintGL();
}

void GLWidget::callDrawTargets()
{
	PaintTransformations ptran(this);
	drawTargets(true);	// the virtual function
}


void GLWidget::myPaintGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glVertex3d(1, 1, 0);
	glVertex3d(1, 0, 0);

	glNormal3d(0, -1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);
	glVertex3d(1, 0, 1);
	glVertex3d(1, 0, 0);
	glEnd();
}

void GLWidget::setCullFace(bool b, bool update) 
{ 
	m_bBackFaceCulling = b; 
	if (m_bBackFaceCulling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	if (update)
		updateGL(); 
}

void GLWidget::setViewFrustrum(QAction *act) 
{ 
	setViewFrustrum((EViewState)act->data().toInt()); 
}

void GLWidget::setViewFrustrum(GLWidget::EViewState newState)
{
	if (m_viewState == newState)
		return; // nothing to do.
	m_viewState = newState;
	reCalcProj();
	reCalcLight();
	updateGL(); // Invalidates the window, posts WM_PAINT
}

void GLWidget::setAction(QAction *act)
{
/*	m_mouseAct = (EMouseAction)act->data().toInt();
	if ((m_mouseAct == Scale) != (!m_main->m_axisActs->isEnabled()))
	{
		m_main->m_axisActs->setEnabled(m_mouseAct != Scale);
	}*/
}

void GLWidget::setAxis(QAction *act) 
{ 
	m_axis = (EAxis)act->data().toInt(); 
}
void GLWidget::setSpace(QAction *act) 
{ 
	m_transformType = (ETransformType)act->data().toInt();
}

void GLWidget::rotateTimeout()
{
	if ((!m_bTimerMode) || (!m_bContinuous))
		return; // just to be safe...
	if (QGLContext::currentContext() != context())
		return; // the current context isn't this views context

	ActMouseMove(Rotate, m_nXDelt, m_nYDelt);
	updateGL();

}

void GLWidget::setContinuous(bool val)
{ 
	m_bContinuous = val; 
	if (m_bTimerMode)
	{
		m_timer->stop();
		m_bTimerMode = false;
	}
}

void GLWidget::zoom(int v) 
{ 
	if (v == m_zoomVal)
		return;
	m_zoomVal = v; 
	reCalcLight();
	updateGL(); 
	// don't emit back. no one needs this now.
}  




// MOUSE HANDLERS ///////////////////////////////////////////


int GLWidget::DoChoise(int chX, int chY)
{
	GLuint	buffer[512];										
	GLint	hits;												

	// The Size Of The Viewport. [0] Is <x>, [1] Is <y>, [2] Is <length>, [3] Is <width>
	GLint	viewport[4];

	// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, buffer);								

	(void) glRenderMode(GL_SELECT);
	
	glInitNames();												
	glPushName(0);												
	
	glMatrixMode(GL_PROJECTION);								
	glPushMatrix();												
	glLoadIdentity();											

	
	// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen, Where The Mouse Is.
	gluPickMatrix((GLdouble)chX, (GLdouble)(viewport[3]-chY), 1.0f, 1.0f, viewport);
	
	reCalcProj(false);
	
	glMatrixMode(GL_MODELVIEW);	
	
	callDrawTargets();
		
	glMatrixMode(GL_PROJECTION);								
	glPopMatrix();	

	glMatrixMode(GL_MODELVIEW);									
	hits=glRenderMode(GL_RENDER);								
		
	int choose = -1;
	if (hits > 0)												
	{
		choose = buffer[3];									
		int depth = buffer[1];									
	
		for (int loop = 1; loop < hits; loop++)					
		{
			// If This Object Is Closer To Us Than The One We Have Selected
			if (buffer[loop*4+1] < GLuint(depth))
			{
				choose = buffer[loop*4+3];						
				depth = buffer[loop*4+1];						
			}
		}
	}
	return choose;
}


void GLWidget::mousePressEvent(QMouseEvent *event)
{
	m_lastPos = event->pos();
	
	if ((m_bTimerMode) && (m_mouseAct == Rotate))
	{
		m_timer->stop();
		m_nXDelt = 0;
		m_nYDelt = 0;
		m_bTimerMode = false;
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_bContinuous && (m_mouseAct == Rotate) && (m_nXDelt != 0) || (m_nYDelt != 0))
	{
		m_timer->start(25);
		m_bTimerMode = true;
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() == 0)
		return;

	EMouseAction act = m_mouseAct;
	if ((event->buttons() == Qt::RightButton))
	{
		act = Translate;
	}

	m_axis = XYaxis;
	if (event->modifiers() == Qt::ControlModifier)
		m_axis = XZaxis;


	int dx = event->x() - m_lastPos.x();
	int dy = event->y() - m_lastPos.y();

	m_nXDelt = dx;
	m_nYDelt = dy; // for rotate timeout

	makeCurrent();
	
	ActMouseMove(act, dx, dy);
	m_lastPos = event->pos();

	updateGL();					// redraw scene
}


PointMover::PointMover(GLWidget *glw) :m_glw(glw)
{
	{
		PaintTransformations ptran(glw);
		glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMat); // need to be doubles
		glGetDoublev(GL_PROJECTION_MATRIX, m_projMat);
	}
	glGetIntegerv(GL_VIEWPORT, m_viewport);
}

void PointMover::setDelta(float dx, float dy)
{	
	m_dx = dx;
	m_dy = dy;
}


Vec3 PointMover::movePointXY(const Vec3& in) const
{
	double wx, wy, wz, ox, oy, oz;
	gluProject(in.x, in.y, in.z, m_modelMat, m_projMat, m_viewport, &wx, &wy, &wz);
	gluUnProject(wx + m_dx, wy - m_dy, wz, m_modelMat, m_projMat, m_viewport, &ox, &oy, &oz);
	return Vec3(ox, oy, oz);
}
Vec3 PointMover::movePointXZ(const Vec3& in) const
{
	double wx, wy, wz, ox, oy, oz;
	gluProject(in.x, in.y, in.z, m_modelMat, m_projMat, m_viewport, &wx, &wy, &wz);
	gluUnProject(wx + m_dx, wy, wz - (float)m_dy/m_glw->m_cyClient/100.0f, m_modelMat, m_projMat, m_viewport, &ox, &oy, &oz);
	return Vec3(ox, oy, oz);
}




void GLWidget::wheelEvent(QWheelEvent *event)
{
	m_zoomVal += event->delta() / 13;
	m_zoomVal = qMin(qMax(m_zoomVal, ZOOM_MIN), ZOOM_MAX);
	emit zoomChanged(m_zoomVal);
	updateGL();
}

void GLWidget::ActMouseMove(EMouseAction action, int dx, int dy)
{
	switch(action) 
	{			// act based on the current action
	case Rotate:    rotate(m_axis, dx, dy); break;
	case Translate: translate(dx, dy); break;
	case Scale:     scale(dx, dy); break;
	}
	// do not call updateGL, called does this
}


void GLWidget::rotate(EAxis axis, int x, int y) 
{

	float mat[16];
	double fx = (double)x, fy = (double)y;

	if (m_transformType == WorldSpace)
	{
		glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		glLoadIdentity();
	}
	//else // objectspace, translate it to 0,0,0
		//glTranslated((aqmax[0] + aqmin[0])/2, (aqmax[1] + aqmin[1])/2, (aqmax[2] + aqmin[2])/2);

	switch (axis)
	{
	case Xaxis:	glRotated(fx, 1, 0, 0);	break;
	case Yaxis:	glRotated(fx, 0, 1, 0);	break;
	case Zaxis:	glRotated(fx, 0, 0, 1);	break;
	case XYaxis: glRotated(fx, 0, 1, 0); glRotated(fy, 1, 0, 0); break;
	case XZaxis: glRotated(fx, 0, 0, 1); glRotated(fy, 1, 0, 0); break;
	case YZaxis: glRotated(fx, 0, 1, 0); glRotated(fy, 0, 0, 1); break;
	}

	if (m_transformType == WorldSpace)
		glMultMatrixf(mat);
	//else // translate it back
	//	glTranslated(-(aqmax[0] + aqmin[0])/2, -(aqmax[1] + aqmin[1])/2, -(aqmax[2] + aqmin[2])/2);

	emit rotated(axis, x, y);
}

void GLWidget::translate(int xValue, int yValue) 
{
	double fx = ((double)xValue / realScale);
	double fy = ((double)yValue / realScale);
	double mat[16];

	if (m_transformType == ObjectSpace) 
	{
		fx /= m_osf;
		fy /= m_osf;
	}

	glMatrixMode(GL_MODELVIEW);

	if (m_transformType == WorldSpace)
	{
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		glLoadIdentity();
	}

	switch (m_axis)
	{
	case Xaxis: glTranslated(fx, 0, 0);	break;
	case Yaxis: glTranslated(0, fx, 0);	break;
	case Zaxis: glTranslated(0, 0, fx);	break;
	case XYaxis: glTranslated(fx, 0, 0); glTranslated(0, -fy, 0); break;
	case XZaxis: glTranslated(fx, 0, 0); glTranslated(0, 0, fy); break;
	case YZaxis: glTranslated(0, 0, fx); glTranslated(0, -fy, 0); break;
	}

	if (m_transformType == WorldSpace)
	{
		glMultMatrixd(mat);
	}
}

void GLWidget::scale(int xValue, int yValue) 
{
	double f = 1 + (double)(xValue + yValue) / 150.0;	// object may flip if we do it too fast
	double mat[16];
	
	glMatrixMode(GL_MODELVIEW);

	if (m_transformType == WorldSpace)	// damn right it's needed
	{
		glGetDoublev(GL_MODELVIEW_MATRIX, mat);
		glLoadIdentity();
	}

	glScaled(f, f, f);

	m_osf = m_osf * f;
	if (m_transformType == WorldSpace)
	{
		glMultMatrixd(mat);	
	}
}

//int m_fontBase;

void GLWidget::setFont(int fontIndex, QFont font)
{
	makeCurrent();
	int base = glGenLists(256);
	if (m_fontBases.size() <= fontIndex)
		m_fontBases.resize(fontIndex + 1);

	m_fontBases[fontIndex] = FontEntry(base, font);
	((PatchQGLContext*)(context()))->generateFontDisplayLists(font, base);
}
QFont GLWidget::getFont(int fontIndex)
{
	return m_fontBases[fontIndex].font;
}

void GLWidget::killFont(int fontIndex)	//TBD-call this?
{
	glDeleteLists(m_fontBases[fontIndex].base, 256);
}

void GLWidget::mglPrint(const QString &str, int fontIndex)
{
	if (str.isEmpty())									// If There's No Text
		return;											// Do Nothing

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glDisable(GL_TEXTURE_2D);
	glListBase(m_fontBases[fontIndex].base);					// Sets The Base Character to 32
	glCallLists(str.length(), GL_UNSIGNED_BYTE, str.toAscii());	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}





const char* errorText(uint code)
{
	switch(code)
	{
	case GL_NO_ERROR: return "GL_NO_ERROR";
	case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
	default: return "-Unknown Error Code-";
	}
}


void mglCheckErrors(const char* place = NULL)
{
	QString s;
	GLenum code;
	while ((code = glGetError()) != GL_NO_ERROR)
		s += QString("  0x%1: %2\n").arg(code, 0, 16).arg(errorText(code));
	if (!s.isNull())
	{
		QString ps =  "\n";
		if (place != NULL)
			ps = QString(place) + "\n";
		s.sprintf("GLError: %s%s", ps.toAscii().data(), s.toAscii().data());
		int ret = QMessageBox::critical(NULL, "GLError", s, "Continue", "Exit", "Break");
		if (ret == 1)
			TerminateProcess(GetCurrentProcess(), 1);
		if (ret == 2)
			__asm int 3;
	}
}
void mglCheckErrors(const QString& s)
{
	mglCheckErrors(s.toAscii().data());
}


void mglCheckErrorsC(const char* place = NULL)
{
	QString s;
	GLenum code;
	while ((code = glGetError()) != GL_NO_ERROR)
		s += QString("  0x%1: %2\n").arg(code, 0, 16).arg(errorText(code));
	if (!s.isNull())
	{
		QString ps =  "\n";
		if (place != NULL)
			ps = QString(place) + "\n";
		printf("GLError: %s%s", ps.toAscii().data(), s.toAscii().data());
	}
}
void mglCheckErrorsC(const QString& s)
{
	mglCheckErrorsC(s.toAscii().data());
}

void GLWidget::checkErrors(const char* place)
{
	mglCheckErrors(place);
}

