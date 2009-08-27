#include "KwEdit.h"

#include <QSyntaxHighlighter>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QAction>
#include <QFileDialog>
#include <QTextStream>
#include <QKeyEvent>

#include <QMenu>

#include "PresetModels.h"
#include "DisplayConf.h"
#include "MyLib/MyInputDlg.h"
#include "Document.h"
#include "ConfXmls.h"
#include "T2GLWidget.h"
#include "ParamUI.h"


class OffsetData : public QTextBlockUserData
{
public:
	OffsetData() : offset(-1), count(-1) {}
	OffsetData(int _offset, int _count) : offset(_offset), count(_count) {}
	int offset, count;
};

static QColor ERROR_COL = QColor(255, 128, 128);
static QColor WARNING_COL = QColor(255, 174, 33);

class MySyntaxHighlighter : public QSyntaxHighlighter
{
public:
	MySyntaxHighlighter(QTextDocument *parent, KwEdit* cont)
		:QSyntaxHighlighter(parent), m_cont(cont)
	{}

protected:
	virtual void highlightBlock(const QString &text)
	{
		//m_cont->edChanged(); // this actually happens before the update to the text so we update it here.
		// TBD: this is really NOT efficient. causes the whole text to be compared again and again
		QTextBlockUserData *ud = currentBlockUserData();
		if (ud == NULL)
			return;
		OffsetData *od = (OffsetData*)ud;

		QTextCharFormat f;
		f.setBackground(ERROR_COL);
		setFormat(od->offset, od->count, f);

	}
private:
	KwEdit *m_cont;
};


static QFont fixedFont("Courier", 10);


#define EDIT_CONF_FILE ("conf.edit.xml")
#define PROG_CONF_FILE ("conf.programs.xml")


KwEdit::KwEdit(QWidget* parent, DisplayConf& conf, Document* doc, T2GLWidget *view)
:MyDialog(parent), m_conf(conf), m_doc(doc), m_view(view), m_isEnabled(false)
{
	setWindowTitle("Kawaii Script");
	ui.setupUi(this);
	on_tabs_currentChanged(ui.tabs->currentIndex());

	(new CheckBoxIn(&conf.addFace, ui.addFaceBot))->reload();

	connect(ui.vtxEd, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursorPosLine()));
	connect(ui.fragEd, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursorPosLine()));
	connect(ui.ed, SIGNAL(zoomChange(int)), this, SLOT(zoomEdits(int)));
	connect(ui.vtxEd, SIGNAL(zoomChange(int)), this, SLOT(zoomEdits(int)));
	connect(ui.fragEd, SIGNAL(zoomChange(int)), this, SLOT(zoomEdits(int)));


	ConfXmls confxmls(EDIT_CONF_FILE, PROG_CONF_FILE, this);

	confxmls.loadModelsFile(ui.ed);

	ui.ed->setContextMenuPolicy(Qt::ActionsContextMenu);
 	QAction *act;
	ui.ed->addAction(act = new QAction("", ui.ed));
	act->setSeparator(true);
	ui.ed->addAction(act = new QAction("clear", ui.ed));
	connect(act, SIGNAL(triggered(bool)), this, SLOT(setToPre()));
	ui.ed->addAction(act = new QAction("Generate from File", ui.ed));
	connect(act, SIGNAL(triggered(bool)), this, SLOT(setToPre()));
	ui.ed->addAction(act = new QAction("Load File", ui.ed));
	connect(act, SIGNAL(triggered(bool)), this, SLOT(setToPre()));

	m_progmenu = new QMenu(this);
	confxmls.loadProgramsFile(m_progmenu);

	ui.vtxEd->setMenu(m_progmenu);
	ui.fragEd->setMenu(m_progmenu);

	//ui.runTypeBox->addItem("Run Normal", (int)RunNormal);
	//ui.runTypeBox->addItem("Run From Quad", (int)RunQuadProcess);
	//ui.runTypeBox->addItem("Run Tex2Tex", (int)RunTex2Tex);
	(new ComboBoxIn<DisplayConf::RunType>(&conf.runType, ui.runTypeBox))->reload();

	connect(ui.update_shader, SIGNAL(clicked()), this, SLOT(doShadersUpdate()));
	connect(ui.shaderEnable, SIGNAL(clicked()), this, SLOT(doShadersUpdate()));
	//connect(ui.runTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(doShadersUpdate())); // rather wasteful. recompiles the thing

	m_high = new MySyntaxHighlighter(ui.ed->document(), this);

	resize(300, 500);
	move(pos().x()+500, pos().y());
	connect(ui.ed, SIGNAL(textChanged()), this, SLOT(edChanged()));

}

struct ModelData
{
	ModelData() {}
	ModelData(QString name, bool _isMesh) :filename(name), isMesh(_isMesh) {}
	QString filename;
	bool isMesh;
};
Q_DECLARE_METATYPE(ModelData);


QAction* KwEdit::confAddModel(const QString& display, const QString& filename, bool isMesh)
{
	QAction *act = new QAction(display, this);

	act->setData(QVariant::fromValue(ModelData(filename, isMesh)));
	connect(act, SIGNAL(triggered(bool)), this, SLOT(setToPre()));

	m_userActions[display.toLower()] = act;
	return act;
}

QAction* KwEdit::confAddProg(const QString& display, ProgKeep* prog)
{
	m_progrep.append(prog);

	QAction *act = new QAction(display, this);
	act->setData((int)prog);
	connect(act, SIGNAL(triggered(bool)), this, SLOT(readProg()));

	m_userActions[display.toLower()] = act;
	return act;
}

void KwEdit::activateAction(const QString& name)
{
	TActionMap::iterator it = m_userActions.find(name.toLower());
	if (it == m_userActions.end())
		return;
	(*it)->trigger();
}



void KwEdit::doShadersUpdate()
{
	m_in.params.clear();
	m_isEnabled = ui.shaderEnable->isChecked();
	if (m_isEnabled)
	{
		m_in.vtxProg = ui.vtxEd->toPlainText();
		m_in.fragProg = ui.fragEd->toPlainText();

		foreach(ParamUi* pui, m_paramUi)
		{
			ParamInput pi(pui->name->text(), 
				(EParamType)pui->type->itemData(pui->type->currentIndex()).toInt(), 
				pui->value->text(), pui->m_isUniform);
			pui->index = m_in.params.size();
			m_in.params.append(pi);
			
		}
	}
	else
	{
		m_in.vtxProg.clear();
		m_in.fragProg.clear();
	}

	m_in.runType = conf().runType;

	emit updateShaders(m_in); // calls compile

	doVarsUpdate();
	m_doc->emitProgChanged();
}

void KwEdit::doVarsUpdate()
{
	if (!m_isEnabled)
		return; // no need to do anything if not enabled.

	for(int i = 0; i < m_paramUi.size(); ++i)
	{
		const ParamUi* pui = m_paramUi[i];
		doVarUpdate(pui, false);
	}
		
}



void KwEdit::doVarUpdate(const ParamUi* pui, bool update)
{
	ParamInput curpi(pui->name->text(), pui->m_dtype, pui->value->text(), pui->m_isUniform);

	QColor c = Qt::white;

	if (!curpi.name.isEmpty() && !curpi.value.isEmpty())
	{
		if (pui->index < m_in.params.size())
		{
			ParamInput progpi = m_in.params[pui->index];

			if (progpi.name == curpi.name && getBaseType(progpi.type) == getBaseType(curpi.type) && progpi.isUniform == curpi.isUniform)
			{
				progpi.value = curpi.value;

				if (!m_doc->parseSingleParam(progpi, (pui->moreCont != NULL)?pui->moreCont->prop:NULL, update))
					c = ERROR_COL;
			}
			else
				c = WARNING_COL; // no such param in the compiled prog
		}
		else
			c = WARNING_COL; // no such param	
	}

	QPalette pl = pui->value->palette();
	pl.setColor(QPalette::Base, c);
	pui->value->setPalette(pl);


}

void KwEdit::doVarUpdate()
{
	QObject *sp = sender();
	ParamUi *pui = ((UserData<ParamUi*>*)sp->userData(0))->i;

	doVarUpdate(pui);
}


void KwEdit::updateCursorPosLine()
{
	int i = m_curEd->textCursor().block().firstLineNumber();
	ui.infoLabel->setText(QString("Line %1").arg(i + 1));
}

void KwEdit::zoomEdits(int delta)
{
	int size = m_conf.editFontSize + delta;;
	m_conf.editFontSize = size;
	ui.ed->setFontSize(size);
	ui.vtxEd->setFontSize(size);
	ui.fragEd->setFontSize(size);
}




void KwEdit::edChanged()
{
	if (ui.ed->toPlainText() == curText)
		return;
	curText = ui.ed->toPlainText();
	emit changed(curText);
}

void KwEdit::addError(int start, int count)
{
	QTextBlock b = ui.ed->document()->findBlock(start);
	b.setUserData(new OffsetData(start - b.position(), count));
	errorBlockNumbers.append(b.blockNumber());
}

void KwEdit::clearErrors()
{
	foreach(int i, errorBlockNumbers)
		ui.ed->document()->findBlockByNumber(i).setUserData(NULL);
	errorBlockNumbers.clear();
}

void KwEdit::finishErrors()
{
	//if (!errorBlockNumbers.isEmpty())
	m_high->rehighlight();
}

QString KwEdit::generateFromFile()
{
	QString filename = QFileDialog::getOpenFileName(this, "Load Obj", m_conf.lastDir, "points files (*.obj)");
	if (filename.isEmpty())
		return "";
	m_conf.lastDir = QFileInfo(filename).absolutePath();
	

	QString prog;


	QFile file(filename);
	file.open(QFile::ReadOnly);
	file.setTextModeEnabled(true);
	QTextStream in(&file);

	int vi = 1;
	while (!in.atEnd())
	{
		QString line = in.readLine();
		QStringList sl = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		if (sl.isEmpty())
			continue;

		if (sl[0] == "v")
		{
			float x = sl[1].toFloat();
			float y = sl[2].toFloat();
			float z = sl[3].toFloat();

			prog += QString().sprintf("p%d={%1.2f, %1.2f, %1.2f}\n", vi++, x, y, z);
		}
		else if (sl[0] == "f")
		{
			QVector<int> ii;
			for (int i = 1; i < sl.size(); ++i)
			{
				QStringList sls = sl[i].split('/');
				ii.append(sls[0].toInt());
			}
			if (ii.size() == 4)
				prog += QString().sprintf("add(p%d,p%d,p%d,p%d)\n", ii[0], ii[1], ii[2], ii[3]);
			else if (ii.size() == 3)
				prog += QString().sprintf("add(p%d,p%d,p%d)\n", ii[0], ii[1], ii[2]);
		}


	}

	return prog;
}


////////////////////////////////// Edit widget

void ProgTextEdit::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab)
	{
		QTextCursor cursel = textCursor();
		if (cursel.hasSelection())
		{
			int start = cursel.anchor(), end = cursel.position();
			if (start > end)
			{
				qSwap(start, end);
			}
			cursel.setPosition(start);
			cursel.movePosition(QTextCursor::StartOfLine);
			cursel.movePosition(QTextCursor::PreviousCharacter);
			int diff = start - cursel.position();

			cursel.setPosition(end, QTextCursor::KeepAnchor);

			QString sel;
			
			if (event->key() == Qt::Key_Backtab)
			{
				sel = cursel.selectedText();
				sel.replace(QString::fromUtf16(L"\x2029\t"), "\n");
				cursel.insertText(sel);
			}
			else // tab
			{
				sel = cursel.selectedText();
				sel.replace(QString::fromUtf16(L"\x2029"), "\n\t");
				cursel.insertText(sel);
			}
			cursel.setPosition(start);
			cursel.setPosition(start + sel.size() - diff, QTextCursor::KeepAnchor);
			setTextCursor(cursel);

			return;
		}
	}

	QTextEdit::keyPressEvent(event);
}

void ProgTextEdit::wheelEvent(QWheelEvent *e)
{
	// copied from QTextEdit where it works only if read only for some reason.
	if (e->modifiers() & Qt::ControlModifier) {
		const int delta = e->delta();
		if (delta < 0)
			emit zoomChange(-1);
		else if (delta > 0)
			emit zoomChange(1);
		return;
	}
	QTextEdit::wheelEvent(e);
}

void ProgTextEdit::setFontSize(int size)
{
	QFont f = font();
	f.setPointSize(size);
	setFont(f);
}



///////////////////////////////// pre-configured shapes ///////////////
void readToString(const QString& filename, QString& into)
{
	if (filename.isEmpty())
		return;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		printf("can't open %s\n", filename.toAscii().data());
		into = "can't open " + filename + "\n";
	}
	file.setTextModeEnabled(true);
	QTextStream in(&file);
	into = in.readAll();
}

void readToEdit(const QString& filename, QTextEdit* edit)
{
	QString txt;
	readToString(filename, txt);
	edit->setText(txt);
}



void KwEdit::setToPre()
{
	QAction *send = qobject_cast<QAction*>(sender());
	QString name = send->text();
	QString prog;
	if (name == "Generate from File")
	{
		prog = generateFromFile();
	}
	else if (name == "Load File")
	{
		QString filename = QFileDialog::getOpenFileName(this, "Load Model", m_conf.lastDir, "model files (*.obj *.off *.ply2 *.gsd)");
		if (filename.isEmpty())
			return;
		m_conf.lastDir = QFileInfo(filename).absolutePath();
		filename = QDir::current().relativeFilePath(filename);

		prog = "load(" + filename + ")";
	}
	else if (name == "clear")
	{
		prog = "";
	}
	else
	{
		ModelData md = send->data().value<ModelData>();
		if (!md.isMesh)
			readToString(md.filename, prog);
		else
			prog = "load(" + md.filename + ")";
	}
	ui.ed->setPlainText(prog);
}

void KwEdit::setText(const QString& text)
{
	ui.ed->setPlainText(text);
}

void KwEdit::readProg()
{
	QAction *send = qobject_cast<QAction*>(sender());
	ProgKeep *prog = (ProgKeep*)send->data().toInt();
	m_lastLoadedProg = prog->name;

	readToEdit(prog->vtxProg, ui.vtxEd);
	readToEdit(prog->fragProg, ui.fragEd);

	clearParam();
	foreach(const ParamInput& ppi, prog->params)
	{
		addParam(ppi);
	}

	conf().runType = prog->runType;
	ui.shaderEnable->setChecked(true);

	for(ProgKeep::TArgsMap::iterator it = prog->args.begin(); it != prog->args.end(); ++it)
	{
		Prop* prop = m_conf.propByName(it.key());
		prop->fromString(*it);
	}

	doShadersUpdate();
}


void KwEdit::on_tabs_currentChanged(int ti)
{
	ui.shaderControl->setVisible(ti != 0);
	switch (ui.tabs->currentIndex()) 
	{
	case 0: m_curEd = ui.ed; break;
	case 1: m_curEd = ui.vtxEd; break;
	case 2: m_curEd = ui.fragEd; break;
	}
	updateCursorPosLine();
}

void KwEdit::on_addParam_clicked()
{
	addParam(ParamInput("", EPFloat, "", true));
}

void KwEdit::addParam(const ParamInput& pi) //const QString& name, EParamType type, const QString& value)
{
	ParamUi *pui = new ParamUi(this);

	pui->containter = new QWidget(this);
	pui->layout = new QHBoxLayout(); // name edit, type combo, value edit, SOMETHING, x bot. 
	pui->layout->setMargin(0);
	pui->layout->setSpacing(2);

	pui->containter->setLayout(pui->layout);

	pui->uniAttr = new QPushButton();
	pui->uniAttr->setMaximumSize(20, 20);
	pui->uniAttr->setToolTip("Uniform/Attribute");

	pui->name = new QLineEdit();
	pui->name->setToolTip("name");
	pui->type = new QComboBox();
	pui->type->setMaximumWidth(80);
	pui->type->addItem("float", (int)EPFloat);  // ADDTYPE
	pui->type->addItem("float (range)", (int)EPFloatRange);
	pui->type->addItem("float (time)", (int)EPFloatTime);
	pui->type->addItem("int", (int)EPInt);
	pui->type->addItem("vec2", (int)EPVec2);
	pui->type->addItem("vec3", (int)EPVec3);
	pui->type->addItem("vec4", (int)EPVec4);
	pui->type->addItem("vec3 (color)", (int)EPVec3Color);
	pui->type->addItem("vec4 (color)", (int)EPVec4Color);
	pui->type->addItem("texture", (int)EPTexture);
	pui->type->setToolTip("type");
	pui->value = new QLineEdit();
	pui->value->setUserData(0, new UserData<ParamUi*>(pui)); // needed
	pui->value->setToolTip("value");

	pui->name->setText(pi.name);
	pui->type->setCurrentIndex(pui->type->findData(pi.type));
	pui->value->setText(pi.value);
	pui->m_isUniform = pi.isUniform;
	pui->setUniAttrText();
// 	pui->guiA = pi.guiA;
// 	pui->guiB = pi.guiB;

	pui->moreContainer = new QWidget();
	QHBoxLayout *moreLayout = new QHBoxLayout();
	pui->moreContainer->setLayout(moreLayout);
	moreLayout->setMargin(0);
	moreLayout->setSpacing(0);
	pui->moreContainer->hide();

	connect(pui->uniAttr, SIGNAL(clicked()), pui, SLOT(uniAttrChange()));
	connect(pui->value, SIGNAL(textEdited(const QString&)), this, SLOT(doVarUpdate())); // update single variable
	connect(pui->type, SIGNAL(currentIndexChanged(int)), pui, SLOT(initMoreWidget()));

	pui->confGui = new QPushButton("?");
	pui->confGui->setToolTip("Properties");
	pui->confGui->setMaximumSize(20, 20);
	connect(pui->confGui, SIGNAL(clicked()), pui, SLOT(configGui()));

	QPushButton *remBot = new QPushButton("X");
	remBot->setToolTip("remove");
	remBot->setUserData(0, new UserData<ParamUi*>(pui)); // needed
	connect(remBot, SIGNAL(clicked()), this, SLOT(removeParam()));
	//pui.name->set
	remBot->setMaximumSize(20, 20);
	pui->layout->addWidget(pui->uniAttr);
	pui->layout->addWidget(pui->name);
	pui->layout->addWidget(pui->type);
	pui->layout->addWidget(pui->value);
	pui->layout->addWidget(pui->moreContainer);
	pui->layout->addWidget(pui->confGui);
	pui->layout->addWidget(remBot);
	((QBoxLayout*)ui.shaderControl->layout())->addWidget(pui->containter);

	pui->index = m_paramUi.size();
	m_paramUi.append(pui);

	pui->initMoreWidget(&pi);
}



void KwEdit::removeParam()
{
	QObject *sp = sender();
	ParamUi *pui = ((UserData<ParamUi*>*)sp->userData(0))->i;
	removeParam(pui);
}

void KwEdit::removeParam(ParamUi *pui)
{
	QWidget *w = pui->containter;
	delete pui;
	delete w;
	
	m_paramUi.remove(m_paramUi.indexOf(pui));
}

void KwEdit::clearParam()
{
	while (!m_paramUi.isEmpty())
		removeParam(*m_paramUi.begin());
}

void KwEdit::on_reloadBot_clicked()
{
	activateAction(m_lastLoadedProg);
}