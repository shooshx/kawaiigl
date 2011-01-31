#include "T2GLWidget.h"

#include "MyLib/MyInputDlg.h"
#include "MyLib/TrMatrix.h"

#include "KawaiiGL.h"
#include "KwParser.h"
#include "Document.h"
#include "OpenGL/glGlob.h"
#include "OpenGL/MyFramebufferObject.h"
#include "Renderable.h"
#include "NoiseGenerator.h"
#include "GaussianGenerator.h"

#include <QRubberBand>


#include <GL/wglew.h>

// TBD 
// halfedge sanity check, 
// error messages on compile

// numeral symbols
// syntax highlight....



void T2GLWidget::updateSelPointColor()
{
	if (m_selPnt == NULL)
		return;
	conf.selectedCol = m_selPnt->getColor().toColor();

	// TBD in status bar:
	//m_text->setText(QString("%1: (%2, %3, %4)").arg(name.c_str()).arg(ptr->point[0], 3, 'f', 2)
	//	.arg(ptr->point[1], 3, 'f', 2).arg(ptr->point[2], 3, 'f', 2));
}

void T2GLWidget::changedSelectedCol()
{
	if (m_selPnt == NULL)
		return;
	Vec3 c(conf.selectedCol);
	m_selPnt->setColor(c); 
	// TBD: color more than one point at a time.
	m_doc->calcNoParse(); 
}

///////////////////////////////////////////////////////////////////////////////////////

T2GLWidget::T2GLWidget(KawaiiGL *parent, Document *doc)
	:GLWidget(parent)
	,m_selPnt(NULL), m_bShowHiddenPoints(false)
	,conf(parent->sett().disp)
	,m_doc(doc), m_useProg(true)
	,m_rubberBand(NULL)
	,m_framesThisSecond(0), m_framesLast(0)
	,m_curTexTarget(0)
{
	m_bBackFaceCulling = false; // needed or else upsidedown faces are bad.
	m_bSkewReset = false;

	aqmin = Vec3(-2, -2, -2);
	aqmax = Vec3(2, 2, 2);
	m_lightPos = Vec3(0.0f, 0.0f, 20.0f);

	for(int i = 0; i < N_TEX; ++i)
	{
		m_offbufA[i] = NULL;
		m_offbufB[i] = NULL;
	}
	m_texUnits.resize(N_TEX);


	//BuildFont(QFont());
	show();


	connect(&conf.lighting, SIGNAL(changed()), this, SLOT(reconfLight()));
	
	connect(&conf.perspective, SIGNAL(changed()), this, SLOT(reconfProj()));

	connect(&conf.bLines, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bLinesAll, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bPoly, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bPoints, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bUnusedPoints, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bAllPolyPoints, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bCoordNum, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bCoordName, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.backCol, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.lineColor, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bTriangulate, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.pointSize, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&conf.bDepthTest, SIGNAL(changed()), this, SLOT(updateGL()));

	connect(&conf.texAct, SIGNAL(changed()), this, SLOT(updateGL()));

	//connect(&conf.selectedCol, SIGNAL(changed()), this, SLOT(updateGL()));

	connect(&conf.lightAmb, SIGNAL(changed()), this, SLOT(reconfLight()));
	connect(&conf.lightDiff, SIGNAL(changed()), this, SLOT(reconfLight()));
	connect(&conf.lightSpec, SIGNAL(changed()), this, SLOT(reconfLight()));
	connect(&conf.materialShine, SIGNAL(changed()), this, SLOT(reconfLight()));
	connect(&conf.bLitBackFace, SIGNAL(changed()), this, SLOT(reconfLight()));
	connect(&conf.isPointLight, SIGNAL(changed()), this, SLOT(reconfLight()));
	connect(&conf.bBlend, SIGNAL(changed()), this, SLOT(reconfBlend()));

	connect(&conf.selectedCol, SIGNAL(changed()), this, SLOT(changedSelectedCol()) );
	connect(&conf.materialCol, SIGNAL(changed()), this, SLOT(reconfLight()));

	connect(&conf.bCull, SIGNAL(changed()), this, SLOT(reconfCull()));
	connect(&conf.trackForParam, SIGNAL(changed()), this, SLOT(updateMouseTracking()));
	connect(&conf.fullFps, SIGNAL(changed()), this, SLOT(updateGL()));
	connect(&m_fpsTimer, SIGNAL(timeout()), this, SLOT(fpsTimeout()));

	connect(&conf.vSync, SIGNAL(changed()), this, SLOT(reconfVSync()));

	//connect(&conf.runType, SIGNAL(changed()), this, SLOT(changedRunType()));
	//connect(&conf.quadMultiSamp, SIGNAL(changed()), this, SLOT(changedRunType()));


	setFocusPolicy(Qt::StrongFocus); // for key presses

	// need to be done after show. after GL is created

	reconfLight();
	reconfProj();
	reconfCull();
	reconfVSync();
	reconfBlend();

	updateMouseTracking();

	m_framesLast = 0;
	m_framesThisSecond = 0;
	m_fpsTimer.start(1000);

	//setAutoBufferSwap(false);

	for(int i = 0; i < N_TEX; ++i)
	{
		setTexture(i);
	}

	if (!GLEW_EXT_framebuffer_blit)
	{
		printf("missing extension 'GL_EXT_framebuffer_blit' image processing is going to look bad\n");
	}
	
}

void T2GLWidget::reconfLight()
{
	m_lightAmbient = Vec3(conf.lightAmb);
	m_lightDiffuse = Vec3(conf.lightDiff);
	m_lightSpecular = Vec3(conf.lightSpec);
	m_materialShininess = conf.materialShine;
	m_materialDiffAmb = Vec3(conf.materialCol);
	m_pointLight = conf.isPointLight;

	m_fUseLight = conf.lighting; 

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, conf.bLitBackFace?(1):(0));

	reLight();
}
void T2GLWidget::reconfProj()
{
	setViewFrustrum(conf.perspective?GLWidget::Perspective:GLWidget::Ortho);
}
void T2GLWidget::reconfCull()
{
	setCullFace(conf.bCull, true);
}
void T2GLWidget::reconfVSync()
{
	if (!conf.vSync)
	{
		if (WGLEW_EXT_swap_control)			
			wglSwapIntervalEXT(0); // disable vertical sync
		else
			printf("Can't disable vSync since WGL_EXT_swap_control is not supported");
	}
	else
	{
		if (WGLEW_EXT_swap_control)			
			wglSwapIntervalEXT(1);
	}
}
void T2GLWidget::reconfBlend()
{
	if (conf.bBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	updateGL();
}


void T2GLWidget::updateMouseTracking()
{
	//setMouseTracking(conf.trackForParam > 0);
	setMouseTracking(true);
}


void T2GLWidget::newModelLoaded()
{
	m_selPnt = NULL;
	m_dragPnts.clear();

	foreach(shared_ptr<Renderable> r, m_doc->m_rends)
		r->setEnv(this);
}


void T2GLWidget::initializeGL()
{
	GLWidget::initializeGL(); // must call this.

	setFont(0, QFont("Courier New", 16, 75));
	setFont(1, QFont("MS Shell Dlg 2", 8, 75));

	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glEnable(GL_POLYGON_OFFSET_POINT);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

}


void T2GLWidget::fpsTimeout()
{
	m_framesLast = m_framesThisSecond;
	m_framesThisSecond = 0;

	QString s = QString("Vertices: %1 | Polygons: %2 | %3 FPS").arg(m_countVtx).arg(m_countPoly).arg(m_framesLast);
	emit message(s);
}


shared_ptr<GlTexture> T2GLWidget::makeNoise(const QString& cmd)
{
	QStringList args = cmd.split(QRegExp("[\\s,()]"), QString::SkipEmptyParts);
	if (args.isEmpty())
		return shared_ptr<GlTexture>();
	int size = 128;
	float ampStart = 0.5, ampFactor = 0.5;
	if (args.size() > 1)
		size = args[1].toInt();
	if (args.size() > 2)
		ampStart = args[2].toFloat();
	if (args.size() > 3)
		ampFactor = args[3].toFloat();
	
	QString key = QString("%1_%2_%3").arg(size).arg(ampStart).arg(ampFactor);
	TNoiseCache::iterator it = m_noisesCache.find(key);
	if (it == m_noisesCache.end())
	{
		NoiseGenerator n(this);
		shared_ptr<GlTexture> ntex(n.make3Dnoise(size, ampStart, ampFactor));
		m_noisesCache[key] = ntex;
		return ntex;
	}
	return *it;
}

void T2GLWidget::rebindTexture(int which)
{
	if (!isTextureValid(which))
		return;
	glActiveTexture(GL_TEXTURE0 + which);
	m_textures[which]->bind();
}


void T2GLWidget::setTexture(int which)
{
	QString filename = *conf.texFile[which];

	glActiveTexture(GL_TEXTURE0 + which);
	filename = filename.trimmed();

	if (filename.startsWith("noise") && filename.endsWith(")"))
	{
		shared_ptr<GlTexture> ntex = makeNoise(filename);
		if (ntex.get() != NULL)
			doTakeTexture(which, ntex);
		else
			doBindTexture(which, NULL);

	}
	else if (filename.startsWith("gaussian") && filename.endsWith(")"))
	{
		QStringList args = filename.split(QRegExp("[\\s,()]"), QString::SkipEmptyParts);
		int size = 128;
		if (args.size() > 1)
			size = args[1].toInt();

		GaussianGenerator gen;
		shared_ptr<GlTexture> ntex(gen.make2D(size));
		doTakeTexture(which, ntex);
	}
	else // it probably is a filename
	{
		QFileInfo fi(filename);
		if (filename.isEmpty() || !fi.exists() || !fi.isFile())
			doBindTexture(which, NULL);
		else
		{
			QImage img(filename);
			if (!img.isNull())
				doBindTexture(which, &img);
		}
	}
	glActiveTexture(GL_TEXTURE0);
	updateGL();
}

void T2GLWidget::updateCoordFont()
{
	killFont(1);
	QFont f("MS Shell Dlg 2", conf.coordFontSize);
	if (conf.coordFontSize < 13)
		f.setBold(true);

	setFont(1, f);

	// needed to be done twice duo to some strange ass bug in wgl (possibly?)
	// which makes the first one not work in the first time called.
	killFont(1);
	setFont(1, f);

	updateGL();
}


void T2GLWidget::emitDecompile()
{
//	emit decompProgChanged(currentDecompile + "\n" + ExpParser::defDecomp(m_symbols["LIGHT"])); 
}

void T2GLWidget::drawAddTracker(AddTracker& at)
{
	if (at.m_added.isEmpty())
		return;

	glColor3f(1.0, 1.0, 0.0);
	if (at.m_added.size() == 1)
	{
		glPointSize(3);
		glBegin(GL_POINTS);
		glVertex3fv(at.m_added[0]->getCoord().v);
		glEnd();
		return;
	}

	glLineWidth(3.0f);
	glBegin(GL_LINE_STRIP);
	foreach(IPoint* p, at.m_added)
	{
		glVertex3fv(p->getCoord().v);
	}

	glEnd();
	glLineWidth(1.0f);
}


void T2GLWidget::paintFlat()
{
	if (isUsingLight())	
		glDisable(GL_LIGHTING); 

	if ((conf.bLines || conf.bPoints) && m_bShowHiddenPoints)
		glClear(GL_DEPTH_BUFFER_BIT);

	if ((conf.bLines) || (conf.bLinesAll))
	{
		glPolygonOffset(0.0, 0.0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3fv(Vec3(conf.lineColor).v);

		if (m_bBackFaceCulling)
			glDisable(GL_CULL_FACE);


		if (conf.bLines)
		{
			drawObject(*m_doc->m_frameObj, false);
		}

		if (m_bBackFaceCulling)
			glEnable(GL_CULL_FACE);

		if (conf.bLinesAll)
		{
			drawSolids(false);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (conf.bPoints)
	{
		drawPointDots();
	}

	if (conf.bCoordNum || conf.bCoordName)
	{
		drawPointCoords();
	}

	drawAddTracker(m_doc->m_addTrack);



	if (isUsingLight())
		glEnable(GL_LIGHTING);
}

void T2GLWidget::paintPoly()
{
	glPolygonOffset(1.0, 1.0);

	if (!conf.bPoly)
		return;

	ProgramUser proguser;
	if (shouldUseProg() && m_curPass->conf.incPoly)
		proguser.use(&m_curPass->prog);

	glColor3fv(Vec4(m_materialDiffAmb, 1.0f).v);

	if (m_curTexTarget)
		glEnable(m_curTexTarget);

	drawSolids(true);

	if (m_curTexTarget)
		glDisable(m_curTexTarget);

}

void T2GLWidget::drawSolids(bool colorize)
{
	if (m_doc->m_obj != NULL)
		drawObject(*m_doc->m_obj, colorize);
	foreach(shared_ptr<Mesh> mesh, m_doc->m_meshs)
	{
		drawMesh(mesh.get(), colorize);
	}
	foreach(shared_ptr<Renderable> r, m_doc->m_rends)
		r->render();
}

// should be in ShaderProgram?
void T2GLWidget::programConfig(const RenderPassPtr& pass)
{
	PassConf &pconf = pass->conf;
	if (pconf.vertexPointSize)
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	else
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	if (pconf.pointSpirits)
	{
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, pconf.spiriteReplace.val());
	}
	else
	{
		glDisable(GL_POINT_SPRITE);
	}

}

bool T2GLWidget::shouldUseProg() const
{
	return m_useProg && m_curPass && m_curPass->prog.isOk();
}


void T2GLWidget::paint3DScene(bool clearBack)
{
	if (clearBack)  // depth is no cleared?
	{
		Vec3 backCol = Vec3(conf.backCol);
		glClearColor(backCol.r, backCol.g, backCol.b, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		if (conf.bDepthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	if (!m_doc->isValid())
		return;

	// set up texture
	int texunit = (int)conf.texAct.val();
	m_curTexTarget = 0;
	if (conf.texAct != DisplayConf::Tex_None)
	{
		if (isTextureValid(texunit))
		{
			glActiveTexture(GL_TEXTURE0 + texunit);
			m_curTexTarget = m_textures[texunit]->target();
		}
		else if (m_offbufA[texunit] != NULL)
		{
			glActiveTexture(GL_TEXTURE0 + texunit);
			m_curTexTarget = GL_TEXTURE_2D; // frame buffers render to 2D
		}
	}

	if (m_curPass)
		programConfig(m_curPass);


	m_countPoly = 0; m_countVtx = 0;

	if (!m_bShowHiddenPoints || !conf.bDepthTest)
	{ // this is faster
		paintFlat();

		paintPoly();
	}
	else
	{ // needed to 
		paintPoly();

		glClear(GL_DEPTH_BUFFER_BIT);
		paintFlat();
	}

	// tear down texture

	//glActiveTexture(GL_TEXTURE0);

	mglCheckErrorsC("paint");
}


void T2GLWidget::resizeGL(int width, int height)
{
	GLWidget::resizeGL(width, height);
	redoFrameBuffers();
}


void T2GLWidget::redoFrameBuffers()
{
	bool touched[N_TEX] = {0}; // has this run touched texUnit[i] ?
	bool multisampled[N_TEX] = {0}; // is this buffer multisampled?

	if (m_doc->isProgEnabled())
	{
		for(int i = 0; i < N_TEX; ++i)
		{
			delete m_offbufA[i]; 
			m_offbufA[i] = NULL;
			delete m_offbufB[i]; 
			m_offbufB[i] = NULL;
		}

		for(int passi = 0; passi < m_doc->m_passes.size(); ++passi)
		{
			PassPtr pass = m_doc->m_passes[passi];
			RenderPassPtr rpass = dynamic_pointer_cast<RenderPass>(pass);

			int index = -1;
			bool multisamp = false;

			if (rpass)
			{
				PassConf &pconf = rpass->conf;
				index = pconf.to;
				multisamp = pconf.toMultisample;
			}
			else // swap!
			{
				SwapOpPassPtr spass = dynamic_pointer_cast<SwapOpPass>(pass); 
				if (touched[spass->a] && touched[spass->b] || !touched[spass->a] && !touched[spass->b])
					continue; // if both are touched or none are touched, there's nothing to do
				else if (touched[spass->a])
				{  // otherwise we need to create the other buffer that is not the one that is already created
				   // and create it with the same multisampling.
					index = spass->b;
					multisamp = multisampled[spass->a];
				}
				else if (touched[spass->b])
				{
					index = spass->a;
					multisamp = multisampled[spass->b];
				}
			}
			

			if (index != PassConf::Display)  // to frame buffer (texture)
			{
				if (touched[index])
					continue;

				bool doMulti = GLEW_EXT_framebuffer_blit && multisamp;
				
				glActiveTexture(GL_TEXTURE0 + index); // all binding are going to happen in the right unit
				const GlTexture* drawTex = NULL;
				if (doMulti) 
				{	// rendering to the multisampled fbo and then immediatly copy to the normal one which is the texture
					m_offbufA[index] = new MyFramebufferObject(size(), MyFramebufferObject::Depth, GL_TEXTURE_2D, MyFramebufferObject::FMT_RGBA, 4);
					m_offbufB[index] = new MyFramebufferObject(size(), MyFramebufferObject::Depth, GL_TEXTURE_2D, MyFramebufferObject::FMT_RGBA, 0, MyFramebufferObject::FI_LINEAR);
					drawTex = m_offbufB[index]->texture();
					printf("created multisample off-screen buffer (%d,%d) for tex %d\n", width(), height(), index);
				}
				else
				{
					m_offbufA[index] = new MyFramebufferObject(size(), MyFramebufferObject::Depth, GL_TEXTURE_2D, MyFramebufferObject::FMT_RGBA);
					drawTex = m_offbufA[index]->texture();
					printf("created off-screen buffer (%d,%d) for tex %d\n", width(), height(), index);
				}
				
				drawTex->bind();

				m_texUnits[index].outputOf = pass;
				touched[index] = true;
				multisampled[index] = multisamp;
			} // display
		} // passes for
	} // prog enabled

	for(int i = 0; i < N_TEX; ++i)
	{
		if (!touched[i])
		{
			m_texUnits[i].outputOf.reset();
			rebindTexture(i);
		}
	}

	emit changedFBOs();
	updateGL();
}


void T2GLWidget::doSwapPass(int sa, int sb)
{
	qSwap(m_offbufA[sa], m_offbufA[sb]);
	qSwap(m_offbufB[sa], m_offbufB[sb]);
	if (m_offbufA[sb] != NULL)
	{
		glActiveTexture(GL_TEXTURE0 + sa);
		if (m_offbufB[sb]) // multi sampled
			m_offbufB[sb]->texture()->bind();
		else
			m_offbufA[sb]->texture()->bind();
	}
	if (m_offbufA[sa] != NULL)
	{
		glActiveTexture(GL_TEXTURE0 + sb);
		if (m_offbufB[sa]) // multi sampled
			m_offbufB[sa]->texture()->bind();
		else
			m_offbufA[sa]->texture()->bind();
	}
}


void T2GLWidget::myPaintGL()
{
	if (m_doc->isProgEnabled())
	{
		for(int passi = 0; passi < m_doc->m_passes.size(); ++passi)
		{
			m_curPass = dynamic_pointer_cast<RenderPass>(m_doc->m_passes[passi]);
			if (!m_curPass)
			{
				SwapOpPassPtr spass = dynamic_pointer_cast<SwapOpPass>(m_doc->m_passes[passi]);
				if (spass) // do swap pass.
				{
					doSwapPass(spass->a, spass->b);
				}
				continue;
			}
			PassConf &pconf = m_curPass->conf;

			// set up render to
			MyFramebufferObject *fbo = NULL;
			int toIndex = pconf.to;
			if (toIndex != PassConf::Display)  // to frame buffer (texture)
			{
				fbo = m_offbufA[toIndex];
			}

			// render
			MyFramebufferObject::doBind(fbo);
			if (pconf.what == PassConf::Model)
			{
				paint3DScene(pconf.clear);
			}
			else if (pconf.what == PassConf::Model_From_Light)
			{
				Vec3 lpos = lightPos();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				gluLookAt( lpos.x, lpos.y, lpos.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
				paint3DScene(pconf.clear);
				glPopMatrix();
			}
			else // Quad_TexX
			{
				ProgramUser proguser;  //... through the program
				if (m_curPass->prog.isOk())
					proguser.use(&m_curPass->prog);

				int texunit = (int)pconf.what;
				glActiveTexture(GL_TEXTURE0 + texunit);
				glEnable(GL_TEXTURE_2D);
				WholeScreenQuad wsq(this, texunit);
				wsq.render();
				glDisable(GL_TEXTURE_2D);
			}
			MyFramebufferObject::doRelease(fbo);

			// take care of to-multisampling
			if (toIndex != PassConf::Display)
			{
				bool doMulti = GLEW_EXT_framebuffer_blit && pconf.toMultisample;
				if (doMulti && m_offbufA[toIndex] != NULL && m_offbufB[toIndex] != NULL)
				{ // can't read directly from a multi-sample render buffer, need to copy it to a texture.
					glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, m_offbufA[toIndex]->handle()); // from
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, m_offbufB[toIndex]->handle()); // to
					// using GL_ARB_framebuffer_object
					glBlitFramebuffer(0, 0, width(), height(), 0, 0, width(), height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
					glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, 0);
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, 0);
				}
			}
		}
	}
	else
	{
		paint3DScene();
	}

	++m_framesThisSecond;
	if (conf.fullFps)
	{
		update();
	}
}



class VertexDrawer
{
public:
	VertexDrawer(T2GLWidget *glw, bool _hasTex, bool _colorize, bool _vertexNormals) 
		: m_glw(glw), m_doc(glw->doc()), hasTex(_hasTex), colorize(_colorize) , vertexNormals(_vertexNormals)
	{
		m_texUnit = (int)(m_glw->conf.texAct.val());
	}
	
	T2GLWidget *m_glw;
	Document *m_doc;
	bool hasTex, colorize, vertexNormals;
	int m_texUnit;

	void drawVertex(MyPolygon &curpl, int pni)
	{
		MyPoint *curpn = curpl.vtx[pni];

		if (vertexNormals)
			glNormal3fv(curpn->n.v);
		else
			glNormal3fv(curpl.center.v);

		if (hasTex)
		{
			//glTexCoord2f(curpl.texAncs[pni].x, curpl.texAncs[pni].y);
			glMultiTexCoord2f(GL_TEXTURE0 + m_texUnit, curpl.texAncs[pni].x, curpl.texAncs[pni].y);
		}

		if (m_glw->shouldUseProg())
		{
			m_doc->setAttribs(&curpl, pni);
		}
	
		if (colorize)
			glColor3fv(curpn->col.v);

		glVertex3fv(curpn->p.ptr());
	}
};

void T2GLWidget::drawObject(const MyObject& obj, bool colorize)
{
	bool hasTex = true; //false;
	bool triangulate = conf.bTriangulate;
	VertexDrawer drawer(this, hasTex, colorize, obj.verterxNormals);

	if (!obj.poly.isEmpty())
	{
		glBegin(triangulate?GL_TRIANGLES:GL_QUADS); 
		for(int pli = 0; pli < obj.poly.size(); ++pli)
		{
			MyPolygon &curpl = *obj.poly[pli];
			if (curpl.pnum != 4)
				continue;

			if (triangulate)
			{
				drawer.drawVertex(curpl, 0);
				drawer.drawVertex(curpl, 1);
				drawer.drawVertex(curpl, 2);

				drawer.drawVertex(curpl, 0);
				drawer.drawVertex(curpl, 3);
				drawer.drawVertex(curpl, 2);
				m_countPoly += 2;
				m_countVtx += 6;
			}
			else
			{
				for(int pni = 0; pni < 4; ++pni) 
				{
					drawer.drawVertex(curpl, pni);
				}	
				++m_countPoly;
				m_countVtx += 3;
			}
		} 
		glEnd();
	}

	if (!obj.triangles.isEmpty())
	{
		glBegin(GL_TRIANGLES); 
		for(int pli = 0; pli < obj.triangles.size(); ++pli) 
		{
			MyPolygon &curpl = *obj.triangles[pli];
			Q_ASSERT(curpl.pnum == 3);
			for(int pni = 0; pni < 3; ++pni)
			{
				drawer.drawVertex(curpl, pni);
			}
			++m_countPoly;
			m_countVtx += 4;
		} 
		glEnd();
	}
}


void T2GLWidget::drawObjectPoints(const MyObject& obj)
{
	if (obj.points.isEmpty())
		return;

	glBegin(GL_POINTS);
	
	for(int pi = 0; pi < obj.points.size(); ++pi)
	{
		MyPoint &curpn = *obj.points[pi];
	
		glColor3fv(curpn.col.v);
		glVertex3fv(curpn.p.v);
		// no normals.
	}

	glEnd();
}

// colorize - do any color at all (false for lines drawing)
void T2GLWidget::drawMesh(const Mesh* rmesh, bool colorize)
{
	if (rmesh->numFaces() == 0)
		return;

	bool vtxNormals = conf.bVtxNormals;
	bool revNormals = false;

	bool vtxTexCoord = colorize && rmesh->hasVtxProp(Prop_TexCoord);
	bool doMtl = colorize && !rmesh->mtl().isEmpty();
	int curMtl = -1, nextMtl = -1;

	//bool eachTexCoord = colorize && rmesh->hasEachProp(Prop_TexCoord);
	m_doc->initAttribMesh(rmesh);

	int texUnit = (int)(conf.texAct.val());

	if (colorize)
		glColor3fv(Vec4(m_materialDiffAmb, 1.0f).v);

	glBegin(GL_TRIANGLES);

	for(Mesh::Face_const_iterator it = rmesh->faces_begin(); it != rmesh->faces_end(); ++it)
	{
		Mesh::Face_const_handle fh = &(*it);

		if (doMtl && curMtl != (nextMtl = fh->prop<int>(Prop_Group)))
		{
			glColor3fv(rmesh->mtl()[nextMtl].diffuseCol.v);
			curMtl = nextMtl;
		}
		if (!vtxNormals)
			glNormal3fv((revNormals?-fh->normal():fh->normal()).v);
		for(int i = 0; i < 3; ++i)
		{
			Mesh::Vertex_const_handle vh = fh->vertex(i);

			if (vtxNormals)
				glNormal3fv((revNormals?-vh->normal():vh->normal()).v);
			//if (eachTexCoord)
			//	glMultiTexCoord2fv(GL_TEXTURE0 + texUnit, fh->propEach<Vec2>(Prop_TexCoord, i).v);
			else if (vtxTexCoord)
				glMultiTexCoord2fv(GL_TEXTURE0 + texUnit, vh->prop<Vec2>(Prop_TexCoord).v);
			
			if (shouldUseProg())
			{
				m_doc->setAttribs(vh);
			}

			glVertex3fv(vh->point().v);
		}

		++m_countPoly;
		m_countVtx += 3;
	}
	glEnd();
}


void T2GLWidget::drawMeshPoints(const Mesh* rmesh)
{
	if (rmesh->numVtx() == 0)
		return;

	glColor3fv(Vec4(m_materialDiffAmb, 1.0f).v);
	glBegin(GL_POINTS);
	for(Mesh::Vertex_const_iterator it = rmesh->vertices_begin(); it != rmesh->vertices_end(); ++it)
	{
		Mesh::Vertex_const_handle vh = &(*it);
		glVertex3fv(vh->point().v);
	}

	glEnd();
}


	
class PointDrawer : public PointActor
{
public:
	PointDrawer(DisplayConf &conf, T2GLWidget* glw) 
		: m_conf(conf), m_bShowUnused(conf.bUnusedPoints), m_glw(glw) {}

	bool shouldShow(bool used)
	{
		return (m_bShowUnused || used);
	}

	Vec3 pointCol(IPoint* handle)
	{
		if (m_glw->m_dragPnts.contains(handle))
			return Vec3(1.0f, 0.3f, 0.3f);
		else
			return Vec3(0.0f, 0.0f, 1.0f);
	}
protected:
	DisplayConf &m_conf;
	bool m_bShowUnused;
	T2GLWidget *m_glw;
};

struct PointVertexDrawer : public PointDrawer
{
	PointVertexDrawer(DisplayConf &conf, T2GLWidget* glw) : PointDrawer(conf, glw) {}
	virtual void operator()(const string& name, const Vec3& p, bool used, IPoint* handle)
	{
		if (!shouldShow(used))
			return;
		glColor3fv(pointCol(handle).v);

		glVertex3fv(p.v);
		++m_glw->m_countVtx;
	}

};

void T2GLWidget::drawPointDots()
{
	//if (m_bBackFaceCulling)
	//	glDisable(GL_CULL_FACE);

	glPointSize(conf.pointSize);

	ProgramUser proguser;
	if (shouldUseProg() && m_curPass->conf.incPoints)
		proguser.use(&m_curPass->prog);

	bool dotex = m_curPass && m_curPass->conf.pointSpirits;

	if (m_curTexTarget && dotex)
		glEnable(m_curTexTarget);

	glBegin(GL_POINTS);

	m_doc->m_kparser.creator()->foreachPoints(PointVertexDrawer(conf, this));

	glEnd();

	if (conf.bAllPolyPoints)
	{
		if (m_doc->m_obj != NULL)
			drawObjectPoints(*m_doc->m_obj);
		foreach(shared_ptr<Mesh> mesh, m_doc->m_meshs)
		{
			drawMeshPoints(mesh.get());
		}
	}

	if (m_curTexTarget && dotex)
		glDisable(m_curTexTarget);



	//if (m_bBackFaceCulling)
	//	glEnable(GL_CULL_FACE);

}

struct PointTextDrawer : public PointDrawer
{
	PointTextDrawer(DisplayConf &conf, T2GLWidget* glw) : PointDrawer(conf, glw) {}
	virtual void operator()(const string& name, const Vec3& p, bool used, IPoint* handle)
	{
		if (!shouldShow(used))
			return;
		glColor3fv(pointCol(handle).v);

		glRasterPos3d(p.x, p.y, p.z);

		QString ct = "   ";
		if (m_conf.bCoordName)
			ct += QString("%1").arg(name.c_str());
		if (m_conf.bCoordName && m_conf.bCoordNum)
			ct += "=";
		if (m_conf.bCoordNum)
			ct += QString("(%2,%3,%4)").arg(p[0], 0, 'g', 2).arg(p[1], 0, 'g', 2).arg(p[2], 0, 'g', 2);

		//glDisable(GL_DEPTH_TEST);// not working.

		m_glw->mglPrint(ct, 1); 
		//glEnable(GL_DEPTH_TEST);
	}

};

void T2GLWidget::drawPointCoords()
{
	m_doc->m_kparser.creator()->foreachPoints(PointTextDrawer(conf, this));
}

struct PointNameDrawer : public PointDrawer
{
	PointNameDrawer(DisplayConf& conf, T2GLWidget* glw) :PointDrawer(conf, glw) 
	{
		PNT_SZ = 0.1 / (glw->getZoomVal() / 100.0); // targets should always be the same size
	}

	virtual void operator()(const string& name, const Vec3& p, bool used, IPoint* handle)
	{
		if (!shouldShow(used))
			return;
		glLoadName((uint)handle);
		
		glBegin(GL_QUADS); // TBD - this could probably be done better, without polygons.
			glVertex3f(p.x - PNT_SZ, p.y - PNT_SZ, p.z);
			glVertex3f(p.x + PNT_SZ, p.y - PNT_SZ, p.z);
			glVertex3f(p.x + PNT_SZ, p.y + PNT_SZ, p.z);
			glVertex3f(p.x - PNT_SZ, p.y + PNT_SZ, p.z);

			glVertex3f(p.x, p.y - PNT_SZ, p.z - PNT_SZ);
			glVertex3f(p.x, p.y - PNT_SZ, p.z + PNT_SZ);
			glVertex3f(p.x, p.y + PNT_SZ, p.z + PNT_SZ);
			glVertex3f(p.x, p.y + PNT_SZ, p.z - PNT_SZ);

			glVertex3f(p.x - PNT_SZ, p.y, p.z - PNT_SZ);
			glVertex3f(p.x - PNT_SZ, p.y, p.z + PNT_SZ);
			glVertex3f(p.x + PNT_SZ, p.y, p.z + PNT_SZ);
			glVertex3f(p.x + PNT_SZ, p.y, p.z - PNT_SZ);
		glEnd();
	}

	float PNT_SZ;
};


void T2GLWidget::drawTargets(bool inChoise)
{
	if (!m_doc->isValid())
		return;

	if (m_bBackFaceCulling)
		glDisable(GL_CULL_FACE);

	glColor3f(0.0f, 0.0f, 1.0f);

	m_doc->m_kparser.creator()->foreachPoints(PointNameDrawer(conf, this));

	if (m_bBackFaceCulling)
		glEnable(GL_CULL_FACE);
}




#include <QtDebug>

void T2GLWidget::mousePressEvent(QMouseEvent *event) 
{
	m_lastPos = event->pos();
	if (conf.bLightMove)
		return;

	if (event->modifiers() & Qt::ShiftModifier)
	{
		m_rubberOrigin = event->pos();
		if (!m_rubberBand)
			m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
		m_rubberBand->setGeometry(QRect(m_rubberOrigin, QSize()));
		m_rubberBand->show();
		return;
	}

	int chs = DoChoise(event->x(), event->y());
	if (chs > 0)
	{
		m_selPnt = (IPoint*)chs;
		//pntDlg->setPoint(m_selPnt->name);
		updateSelPointColor();
		if (event->button() == Qt::RightButton)
		{
			if ((event->modifiers() & Qt::ControlModifier) == 0) // ctrl not pressed
				m_dragPnts.clear();
			m_dragPnts.insert(m_selPnt);
		}
		else if (event->button() == Qt::LeftButton)
		{
			m_doc->updateTrack(m_selPnt);
		}
	}
	else if (((event->button() == Qt::RightButton) && (event->modifiers() & Qt::ControlModifier) == 0))
	{
		m_dragPnts.clear();
	}

	GLWidget::mousePressEvent(event);
	updateGL();
}

void T2GLWidget::mouseReleaseEvent(QMouseEvent *event) 
{
	//m_dragPnts.clear();
	if (m_rubberBand != NULL)
	{
		m_rubberBand->hide();
		delete m_rubberBand;
		m_rubberBand = NULL;
	}
	updateGL();
}

void T2GLWidget::wheelEvent(QWheelEvent *event)
{
	if (conf.bLightMove)
	{
		Vec3 lp = lightPos();
		float len = lp.length();
		float fact = 5.0f;
		if ((event->modifiers() & Qt::ControlModifier) != 0)
			fact = 0.2f;
		len += (event->delta() > 0)?fact:-fact;
		len = qMin(qMax(len, 0.1f), 100.0f);
		lp.setLength(len);
		setLightPos(lp);
		emit message(QString("Light distance %1").arg(len));

		return;
	}
	GLWidget::wheelEvent(event);
}

void T2GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if ((event->modifiers() & Qt::ControlModifier) != 0)
	{
		emit mouseMove(event);
	}
	if (m_rubberBand != NULL)
	{
		m_rubberBand->setGeometry(QRect(m_rubberOrigin, event->pos()).normalized());
	}
	if (event->buttons() == 0)
		return;
	if ((event->buttons() & Qt::LeftButton) != 0)
	{
		GLWidget::mouseMoveEvent(event);
		return;
	}
	if ((m_dragPnts.empty()) && (!conf.bLightMove))
	{
		GLWidget::mouseMoveEvent(event);
		return;
	}
	int dx = event->x() - m_lastPos.x();
	int dy = event->y() - m_lastPos.y();

	m_lastPos = event->pos();

	PointMover mvp(this);
	mvp.setDelta(dx, dy);

	if (!conf.bLightMove)
	{
		for(TIPoints::const_iterator it = m_dragPnts.begin(); it != m_dragPnts.end(); ++it)
		{
			Vec3 v = (*it)->getCoord();
			v = mvp.movePointXY(v);

			(*it)->setCoord(v);
		}
		m_doc->m_kparser.creator()->cacheVecs(); // TBD - wasteful. need only cache the one that changed!
		m_doc->calcNoParse();
		updateGL();
	}
	else
	{
		TrMatrix model;
		glPushMatrix();
		glLoadIdentity();
		glRotatef(dx, 0, 1, 0);
		glRotatef(dy, 1, 0, 0);
		glGetFloatv(GL_MODELVIEW_MATRIX, model.m);
		glPopMatrix();

		setLightPos(model.multVec(lightPos()));

		emitDecompile();

	}


	updateSelPointColor(); // Meh?
	
}

void T2GLWidget::keyPressEvent(QKeyEvent *event) 
{
	m_bShowHiddenPoints = ((event->modifiers() & Qt::AltModifier) != 0);
	updateGL();
	GLWidget::keyPressEvent(event);
}
void T2GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	m_bShowHiddenPoints = ((event->modifiers() & Qt::AltModifier) != 0);
	updateGL();
	GLWidget::keyReleaseEvent(event);
}

