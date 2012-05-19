#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <gl/glew.h>
#include <QGLWidget>
#include "OpenGL/Texture.h"

#include "MyPoint.h"

#define GL_BEGIN_TEXT() {	glMatrixMode(GL_PROJECTION); glPushMatrix();glLoadIdentity(); \
							glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity(); \
							glDisable(GL_DEPTH_TEST); }
#define GL_END_TEXT() {	glEnable(GL_DEPTH_TEST); glPopMatrix(); glMatrixMode(GL_PROJECTION); \
						glPopMatrix(); glMatrixMode(GL_MODELVIEW); }

#define ZOOM_MIN (10) 
#define ZOOM_MAX (700)


class QActionGroup;

class GLWidget : public QGLWidget
{
	Q_OBJECT
public:
	GLWidget(QWidget *parent = NULL, QGLWidget *sharefrom = NULL);
	virtual ~GLWidget();

	enum EViewState { Ortho, Perspective };
	enum EMouseAction { Rotate, Translate, Scale };
	enum EAxis { Xaxis, Yaxis, Zaxis, XYaxis, XZaxis, YZaxis };
	enum ETransformType { WorldSpace, ObjectSpace };

	void Update3dActs();
	bool isUsingLight() { return m_fUseLight; }
 
public slots:
	void rotateTimeout();
	// GUI actions
	void resetState() { DoReset(); 	updateGL(); }
	void resetLight();
	void setContinuous(bool val);
	void setViewFrustrum(QAction *act);
	void setViewFrustrum(GLWidget::EViewState newState);
	void setAction(QAction *act);
	void setAxis(QAction *act);
	void setSpace(QAction *act);
	void setCullFace(bool b, bool update = false);

	virtual void updateView(int hint) { Q_UNUSED(hint); }; // updates the opengl view according to the hints
	void doBindTexture(int index, QImage* img);
	void doTakeTexture(int index, shared_ptr<GlTexture> tex); // same thing as do bind but take an external texture
	void doUpdateTexture(int index, QImage* img);

	void rotate(EAxis axis, int x, int y);
	void zoom(int v); // v between 0 and 100

	void setUsingLight(bool use);
	Vec3 lightPos() { return m_lightPos; }
	void setLightPos(const Vec3& c);
	//void setLightColor(const Vec3& c);
	void reLight() { makeCurrent(); reCalcLight(); updateGL(); }
	void setClipValue(int percent);

signals:
	void rotated(GLWidget::EAxis axis, int x, int y);
	void callReset(); // means my reset was just called
	void zoomChanged(int v); // the view found it appropriate to change the zoom, notify interested parties

public:
	int DoChoise(int chX, int chY);
	void DoReset();
	void setNewMinMax(const Vec3& min, const Vec3& max, bool scale); 

	void mglPrint(const QString &str, int fontIndex);			// Custom GL "Print" Routine
	void setFont(int fontIndex, QFont font);
	QFont getFont(int fontIndex);
	void killFont(int fontIndex);
	int getZoomVal() { return m_zoomVal; }

	void checkErrors(const char* place);
	QSize clientRect() { return QSize(m_cxClient, m_cyClient); }

protected:

	// this is a call back to whoever subclasses this widget and calls DoChoise
	virtual void drawTargets(bool inChoise) { Q_UNUSED(inChoise); } // default implementation does nothing
	// bool is only helper. GLWidget always calls with 'true'

	virtual void myPaintGL();

	/// overrides must call this one before doing anything.
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);

// events
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);

protected:
	double getScrDiv() const { return realScale * (m_zoomVal / 100.0); }

	const int m_minScaleReset;  //a view can set this member for to limit the maximal size of an object at reset. default is 0 for no effect
	
	bool m_bBackFaceCulling;		// is Back Face Culling enabled
	bool m_bContinuous; // the Continuous option is on. this says nothing about the current state of the timer

	int m_nRotSens, m_nScaleSens, m_nTransSens;

	Vec3 aqmin, aqmax; // bbox for everything, floats as an optinimazation, no conversion 
	QPoint m_lastPos;					// hold last mouse x,y coord

	EViewState m_viewState;
	EMouseAction m_mouseAct;
	EAxis m_axis;
	ETransformType m_transformType;
	int m_zoomVal;
	float m_viewAngle;
	float m_clipView; // 1.0 no clipping, 0.0 all clipping.
	int m_renderFromLight; // -1 for normal rendering;

	bool m_fUseLight;
	Vec3 m_lightPos;
	bool m_pointLight;
	Vec3 m_lightAmbient, m_lightDiffuse, m_lightSpecular;
	int m_materialShininess;
	Vec3 m_materialDiffAmb;

	bool m_bSkewReset; // should the reset be slighty rotated for better view or not

public: //PATCH

	QList<shared_ptr<GlTexture> > m_textures; 
	bool isTextureValid(int index)
	{
		return (m_textures.size() > index && m_textures[index].get() != NULL &&
			    m_textures[index]->isValid());
	}

private:
	virtual void paintGL(); // (event) demote it to private from protected
	void callDrawTargets(); 

	// continuos rotate
	int m_nXDelt, m_nYDelt; // used for continuous rotation, last mouse delta.
	bool m_bTimerMode; // if true, timer is On and rotating.
	QTimer *m_timer;   // every GLWidget has its own timer

	struct FontEntry
	{
		FontEntry() : base(0) {}
		FontEntry(int _base, QFont _font) :base(_base), font(_font) {}
		int base;
		QFont font;
	};
	QVector<FontEntry> m_fontBases;

	int m_cxClient;			// hold the windows width
	int m_cyClient;			// hold the windows height
	double m_AspectRatio;		// hold the fixed Aspect Ration
	double scrScale, realScale;
	double m_osf; // moving scale
	
	Vec3 m_exTranslate; // external translate, from gui translate

	void reCalcProj(bool fFromScratch = true);
	void reCalcLight();

	void ActMouseMove(EMouseAction action, int dx, int dy);
	void translate(int xValue, int yValue);
	void scale(int xValue, int yValue);
	// rotate is a slot

	friend struct PaintTransformations;
	friend class PointMover;
};

class PointMover
{
public:
	PointMover(GLWidget *glw);
	void setDelta(float dx, float dy);

	Vec3 movePointXY(const Vec3& in) const;
	Vec3 movePointXZ(const Vec3& in) const;

private:
	double m_modelMat[16], m_projMat[16];
	int m_viewport[4];
	float m_dx, m_dy;
	GLWidget* m_glw;
};


#endif
