#ifndef __T2GLWIDGET__
#define __T2GLWIDGET__

#include "GLWidget.h"
#include "ShapeIFS.h"
#include "DisplayConf.h"
#include "KwParser.h"
#include "Pass.h"

class T2GLWidget;
class Document;
class Mesh;
class KawaiiGL;


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
	void reconfVSync();
	void reconfBlend();

	void updateMouseTracking();

	void updateSelPointColor();
	void changedSelectedCol();
//	void changedFps();
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
	void drawObjectPoints(const MyObject& obj);
	void drawMesh(const Mesh* rmesh, bool colorize);
	void drawMeshPoints(const Mesh* rmesh);
	void drawPointDots();
	void drawSolids(bool colorize);
	void drawAddTracker(AddTracker& at);
	void paintPoly();
	void paintFlat();
	void drawPointCoords();
	void updateTrack(IPoint* sel);
	shared_ptr<GlTexture> T2GLWidget::makeNoise(const QString& cmd);
	void doSwapPass(int sa, int sb);

	bool shouldUseProg() const;

	void programConfig(const RenderPassPtr& pass);

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
	uint m_curTexTarget; // the current target if texturing of 0 if not.

	DisplayConf &conf;
	RenderPassPtr m_curPass;

	MyFramebufferObject *m_offbufA[N_TEX];
	MyFramebufferObject *m_offbufB[N_TEX];

	QString currentDecompile;

	typedef QMap<QString, shared_ptr<GlTexture> > TNoiseCache;
	TNoiseCache m_noisesCache;

	friend class PointDrawer;
	friend class VertexDrawer;
	friend struct PointVertexDrawer;

public:

	struct TexUnit
	{
		weak_ptr<Pass> outputOf;
	};
	QVector<TexUnit> m_texUnits;
    KawaiiGL *m_mainwin;
};


#endif