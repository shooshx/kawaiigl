#include "KawaiiGL.h"

#include "KwEdit.h"
#include "Document.h"
#include "ControlPanel.h"
#include "ProjBrowser.h"

// this class connects the document and KwEdit
struct ErrorHighlight : public ErrorActor
{
    ErrorHighlight(KwEdit* edw) : m_edw(edw), m_src(NULL) {}

    virtual void clear(DocSrc* src) // in the context of src
    {
        m_src = src;
        m_edw->clearErrors(m_src);
    }
    virtual void operator()(int start, int end)
    {
        m_edw->addError(m_src, start, end);
    }
    virtual void finish()
    {
        m_edw->finishErrors(m_src);
        m_src = NULL;
    }

    KwEdit *m_edw;
    DocSrc *m_src;
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

    m_progMenu = new QMenu(this);
    m_modelsMenu = new QMenu(this);

    m_doc = new Document(this); // adds to the menus

    m_progMenu->addSeparator();
    QAction *loadFromFileAct = m_progMenu->addAction("From file...");
    connect(loadFromFileAct, SIGNAL(triggered(bool)), m_doc, SLOT(loadProgramFile()));

    m_kView = new T2GLWidget(this, m_doc);
    setCentralWidget(m_kView);

    m_edDlg = new KwEdit(this, m_sett.disp, m_doc, m_kView); // need the view for tracking vec2s
    m_edDlg->show();
    m_edDlg->move(pos() + QPoint(width() - 20, 30));

    m_control = new MyDialog(this);
    QBoxLayout *control_l = new QVBoxLayout();
    m_control->setLayout(control_l);
    control_l->setMargin(0);
    control_l->setSpacing(0);
    QTabWidget *tabs = new QTabWidget();
    control_l->addWidget(tabs);

    m_contDlg = new ControlPanel(&m_sett.disp, this, m_doc, m_kView);
    tabs->addTab(m_contDlg, "Config");
    m_browse = new ProjBrowser(this, m_doc);
    tabs->addTab(m_browse, "Browser");
    tabs->setCurrentWidget(m_browse);

    //tabs->setCurrentIndex(m_sett.gui.configWindowTab);
    tabs->setCurrentIndex(0);

    m_control->show();
    m_control->move(pos() + QPoint(-30, 20));
    m_control->resize(100, 100); // make it as small as possible

    m_doc->m_errAct = new ErrorHighlight(m_edDlg);

    connect(m_kView, SIGNAL(message(const QString&)), this, SLOT(message(const QString&)));

    connect(m_edDlg, SIGNAL(changedModel(DocSrc*)), m_doc, SLOT(calc(DocSrc*)));
    connect(m_edDlg, SIGNAL(updateShaders()), m_doc, SLOT(compileShaders()));

//	connect(m_kView, SIGNAL(decompProgChanged(const QString&)), m_edDlg, SLOT(curChanged(const QString&)));

    connect(m_doc, SIGNAL(loaded()), m_kView, SLOT(newModelLoaded()));
    //connect(m_doc, SIGNAL(loaded()), m_edDlg, SLOT(doVarsUpdate())); // parsed new text. vars defs may be wrong
    connect(m_doc, SIGNAL(modelChanged()), m_kView, SLOT(updateGL()));
    connect(m_doc, SIGNAL(progChanged()), m_kView, SLOT(redoFrameBuffers()));
    connect(m_kView, SIGNAL(changedFBOs()), m_contDlg, SLOT(updateTexEdits()));

    connect(m_doc, SIGNAL(progParamChanged()), m_kView, SLOT(updateGL()));
    connect(m_doc, SIGNAL(addModelLine(const QString&)), m_edDlg, SLOT(addModelLine(const QString&)));

    connect(m_contDlg, SIGNAL(changedRend()), m_doc, SLOT(calcNoParse())); // passes update
    connect(m_contDlg, SIGNAL(changedFont()), m_kView, SLOT(updateCoordFont()));
    connect(m_contDlg, SIGNAL(doUpdate()), m_kView, SLOT(updateGL())); // passes update

    connect(m_contDlg, SIGNAL(resetView()), m_kView, SLOT(resetState()));
    connect(m_contDlg, SIGNAL(resetLight()), m_kView, SLOT(resetLight()));
    connect(m_contDlg, SIGNAL(changedTexFile(int)), m_kView, SLOT(setTexture(int)));
//	connect(m_contDlg, SIGNAL(reassertTex(int)), m_kView, SLOT(rebindTexture(int)));
    connect(m_contDlg, SIGNAL(saveMesh()), m_doc, SLOT(calcSave()));

    connect(m_browse, SIGNAL(openDocText(DocElement*)), m_edDlg, SLOT(addPage(DocElement*)) );
    connect(m_browse, SIGNAL(openPassConf(DocElement*)), m_edDlg, SLOT(addPage(DocElement*)) );

    connect(m_doc, SIGNAL(goingToClearProg()), m_edDlg, SLOT(clearingProg()));
    connect(m_doc, SIGNAL(didReadProg(ProgKeep*)), m_edDlg, SLOT(readProg(ProgKeep*)) );
    connect(m_doc, SIGNAL(didReadProg(ProgKeep*)), m_browse, SLOT(readProg(ProgKeep*)) );
    connect(m_doc, SIGNAL(didReadModel(DocSrc*)), m_browse, SLOT(readModel()) );
    connect(m_doc, SIGNAL(didReadModel(DocSrc*)), m_edDlg, SLOT(readModel(DocSrc*)));

    connect(&m_sett.disp.bVtxNormals, SIGNAL(changed()), m_doc, SLOT(calcNoParse())); // TBD - this is bad.

    connect(m_contDlg->ui.clipSlider, SIGNAL(valueChanged(int)), m_kView, SLOT(setClipValue(int)));

    m_kView->setContextMenuPolicy(Qt::ActionsContextMenu);

    QPushButton *viewBot = new QPushButton("View");
    viewBot->setMaximumSize(60, 19);
    statusBar()->addPermanentWidget(viewBot);
    QMenu *view = new QMenu("View");
    viewBot->setMenu(view);

    QPushButton *fpsBot = new QPushButton(QIcon(":/images/arrow-circle.png"), QString());
    fpsBot->setMaximumSize(20, 19);
    statusBar()->addPermanentWidget(fpsBot);
    (new CheckBoxIn(&m_sett.disp.fullFps, fpsBot))->reload();

    QCheckBox *vSyncBox = new QCheckBox("vSync");
    vSyncBox->setMaximumHeight(19);
    statusBar()->addPermanentWidget(vSyncBox);
    (new CheckBoxIn(&m_sett.disp.vSync, vSyncBox))->reload();

    QAction *confVis = new QAction("Display", view);
    view->addAction(confVis);
    m_control->connectAction(confVis);
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
    statusBar()->showMessage(s, 0);
}


void KawaiiGL::processCmdArgs()
{
    QStringList args = QCoreApplication::arguments();
    if (args.isEmpty())
        return;
    for(int i = 1; i < args.size(); ++i)
    {
        m_doc->m_confxmls.activateAction(args[i]); // a model or a program name

    }
}