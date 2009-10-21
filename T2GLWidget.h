#ifndef __T2GLWIDGET__
#define __T2GLWIDGET__

#include "GLWidget.h"
#include "ShapeIFS.h"
#include "DisplayConf.h"
#include "KwParser.h"
#include "Pass.h"

#include <boost/weak_ptr.hpp>

class T2GLWidget;
class Document;
class RMesh;


typedef QSet<IPoint*> TIPoints; // indexed by the pointer of course

class KawaiiGL;
class AddTracker;
class MyFramebufferObject;
class QRubberBand;


class T2GLWidget : public GLWidget // no context menu! right click drags!
{
	Q_OBJECT
public:
	T2GLWidget(KawaiiGL *parent, Document* doc);
	Document* doc() { return m_doc; }

protected:
	void myPaintGLx();
	virtual void myPaintGL();
	virtual void paint3DScene(bool crearBack = true);
	virtual void drawTargets(bool inChoise);
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);

private slots:
	void newModelLoaded(); 

	void setTexture(int which);
	void updateCoordFont();

	void reconfLight();
	void reconfProj();
	void reconfCull();
	void updateMouseTracking();

	void updateSelPointColor();
	void changedSelectedCol();
	void changedFps();
	void fpsTimeout();

public slots:
//	void changedRunType();
	void rebindTexture(int which);
	void redoFrameBuffers();

signals:
	void decompProgChanged(const QString& txt);
	void mouseMove(QMouseEvent* event);
	void message(const QString& s);
	void changedFBOs();

private:
	void drawObject(const MyObject& obj, bool colorize);
	void drawMesh(const RMesh* rmesh, bool colorize);
	void drawPointDots();
	void drawSolids(bool colorize);
	void drawAddTracker(AddTracker& at);
	void paintPoly();
	void paintFlat();
	void drawPointCoords();
	void updateTrack(IPoint* sel);
	boost::shared_ptr<GlTexture> T2GLWidget::makeNoise(const QString& cmd);

	bool shouldUseProg() const;

	void programConfig(const PassPtr& pass);

	void emitDecompile();

private:
	Document *m_doc;

	IPoint *m_selPnt;
	TIPoints m_dragPnts;
	QPoint m_rubberOrigin;
	QRubberBand *m_rubberBand;
	int m_framesThisSecond, m_framesLast;
	QTimer m_fpsTimer;

	int m_countPoly, m_countVtx;

	bool m_bShowHiddenPoints;

	bool m_useProg;

	DisplayConf &conf;
	PassPtr m_curPass;

	MyFramebufferObject *m_offbufA[N_TEX];
	MyFramebufferObject *m_offbufB[N_TEX];

	QString currentDecompile;

	typedef QMap<QString, boost::shared_ptr<GlTexture> > TNoiseCache;
	TNoiseCache m_noisesCache;

	friend class PointDrawer;
	friend class VertexDrawer;
	friend struct PointVertexDrawer;

public:

	struct TexUnit
	{
		boost::weak_ptr<Pass> outputOf;
	};
	QVector<TexUnit> m_texUnits;
};


#endif