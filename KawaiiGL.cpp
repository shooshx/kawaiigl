#include "KawaiiGL.h"

#include "KwEdit.h"
#include "Document.h"
#include "ControlPanel.h"


struct ErrorHighlight : public ErrorActor
{
	ErrorHighlight(KwEdit* edw) : m_edw(edw) {}

	virtual void clear()
	{
		m_edw->clearErrors();
	}
	virtual void operator()(int start, int end)
	{
		m_edw->addError(start, end);
	}
	virtual void finish()
	{
		m_edw->finishErrors();
	}
	KwEdit *m_edw;
};

#define EXAMPLES_DIR ("examples")


KawaiiGL::KawaiiGL(QWidget *parent)
	: QMainWindow(parent), m_kView(NULL), m_edDlg(NULL), m_doc(NULL)
{
	ui.setupUi(this);

	m_sett.loadFromReg();
	connect(&m_sett, SIGNAL(changed()), &m_sett, SLOT(storeToReg()));

	setWindowTitle("KawaiiGL");
	show(); // needed becase we're creating display lists in the c'tor.

	QDir::setCurrent(EXAMPLES_DIR); // we're reading the config and textures from there

	m_doc = new Document(this);

	m_kView = new T2GLWidget(this, m_doc);
	setCentralWidget(m_kView);

	m_edDlg = new KwEdit(this, m_sett.disp, m_doc, m_kView); // need the view for tracking vec2s
	m_edDlg->show();
	m_edDlg->move(pos() + QPoint(width() - 20, 30));

	m_contDlg = new ControlPanel(&m_sett.disp, this, m_doc);
	m_contDlg->show();
	m_contDlg->move(pos() + QPoint(-30, 20));


	m_doc->m_errAct = new ErrorHighlight(m_edDlg);

	connect(m_kView, SIGNAL(message(const QString&)), this, SLOT(message(const QString&)));

	connect(m_edDlg, SIGNAL(changed(const QString&)), m_doc, SLOT(calc(const QString&)));
	connect(m_edDlg, SIGNAL(updateShaders(const ProgInput&)), m_doc, SLOT(compileShaders(const ProgInput&)));
//	connect(m_edDlg, SIGNAL(updateVars(const ProgInput&)), m_doc, SLOT(parseAllParams(const ProgInput&)));

//	connect(m_kView, SIGNAL(decompProgChanged(const QString&)), m_edDlg, SLOT(curChanged(const QString&)));

	connect(m_doc, SIGNAL(loaded()), m_kView, SLOT(newModelLoaded()));
	connect(m_doc, SIGNAL(loaded()), m_edDlg, SLOT(doVarsUpdate())); // parsed new text. vars defs may be wrong
	connect(m_doc, SIGNAL(modelChanged()), m_kView, SLOT(updateGL()));
	connect(m_doc, SIGNAL(progChanged()), m_kView, SLOT(updateGL()));
	connect(m_doc, SIGNAL(progParamChanged()), m_kView, SLOT(updateGL()));
	connect(m_doc, SIGNAL(textChanged(const QString&)), m_edDlg, SLOT(setText(const QString&)));

	connect(m_contDlg, SIGNAL(changedRend()), m_doc, SLOT(calcNoParse())); // passes update
	connect(m_contDlg, SIGNAL(changedFont()), m_kView, SLOT(updateCoordFont()));
	connect(m_contDlg, SIGNAL(doUpdate()), m_kView, SLOT(updateGL())); // passes update

	connect(m_contDlg, SIGNAL(resetView()), m_kView, SLOT(resetState()));
	connect(m_contDlg, SIGNAL(resetLight()), m_kView, SLOT(resetLight()));
	connect(m_contDlg, SIGNAL(changedTexFile(int)), m_kView, SLOT(setTexture(int)));
	connect(m_contDlg, SIGNAL(reassertTex(int)), m_kView, SLOT(rebindTexture(int)));
	connect(m_contDlg, SIGNAL(saveMesh()), m_doc, SLOT(calcSave()));

	connect(&m_sett.disp.bVtxNormals, SIGNAL(changed()), m_doc, SLOT(calcNoParse())); // TBD - this is bad.

	connect(m_contDlg->ui.clipSlider, SIGNAL(valueChanged(int)), m_kView, SLOT(setClipValue(int)));

	m_kView->setContextMenuPolicy(Qt::ActionsContextMenu);

	QPushButton *viewBot = new QPushButton("View");
	viewBot->setMaximumSize(60, 19);
	statusBar()->addPermanentWidget(viewBot);
	QMenu *view = new QMenu("View");
	viewBot->setMenu(view);

	QPushButton *fpsBot = new QPushButton("O");
	fpsBot->setMaximumSize(20, 19);
	statusBar()->addPermanentWidget(fpsBot);
	(new CheckBoxIn(&m_sett.disp.fullFps, fpsBot))->reload();

	QAction *confVis = new QAction("Display", view);
	view->addAction(confVis);
	m_contDlg->connectAction(confVis);
	QAction *editVis = new QAction("Edit", view);
	view->addAction(editVis);
	m_edDlg->connectAction(editVis);


	processCmdArgs();
}


void KawaiiGL::closeEvent(QCloseEvent *event) 
{ 
	m_edDlg->close();  
	m_contDlg->close();
}


void KawaiiGL::message(const QString& s)
{
	statusBar()->showMessage(s, 5000);
}


void KawaiiGL::processCmdArgs()
{
	QStringList args = QCoreApplication::arguments();
	if (args.isEmpty())
		return;
	for(int i = 1; i < args.size(); ++i)
	{
		m_edDlg->activateAction(args[i]); // a model or a program name

	}
}