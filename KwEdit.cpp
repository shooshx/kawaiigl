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
#include "ProgTextEdit.h"

#include "MyLib/QtAdd/CloseButton.h"

#include <boost/shared_ptr.hpp>

using namespace boost;


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



KwEdit::KwEdit(QWidget* parent, DisplayConf& conf, Document* doc, T2GLWidget *view)
:MyDialog(parent), m_conf(conf), m_doc(doc), m_view(view)
{
	setWindowTitle("Kawaii Script");
	ui.setupUi(this);
	ui.tabs->clear();
	//ui.tabs->setContex

	on_tabs_currentChanged(ui.tabs->currentIndex());

	//ui.tabs->setTabsClosable(true);
	QAbstractButton *close = new CloseButton();
	ui.tabs->setCornerWidget(close);
	connect(close, SIGNAL(clicked()), this, SLOT(tabsBarClose()));

	(new CheckBoxIn(&conf.addFace, ui.addFaceBot))->reload();

	(new ComboBoxIn<DisplayConf::RunType>(&conf.runType, ui.runTypeBox))->reload();

	connect(ui.update_shader, SIGNAL(clicked()), this, SLOT(doShadersUpdate()));
	connect(ui.shaderEnable, SIGNAL(clicked()), this, SLOT(doShadersUpdate()));
	connect(ui.saveBot1, SIGNAL(clicked()), this, SLOT(saveCurDoc()));
	connect(ui.saveBot2, SIGNAL(clicked()), this, SLOT(saveCurDoc()));

	//resize(300, 500);
	//move(pos().x()+500, pos().y());
	//connect(ui.ed, SIGNAL(textChanged()), this, SLOT(edChanged()));

	// open the default windows
	readModel(NULL);
	readProg(NULL);

}



bool KwEdit::isEnabled() 
{ 
	return m_doc->m_shaderEnabled; 
}

EditPage::~EditPage()
{
	delete tab; // remove the tab from the tabs
}

void EditPage::commitText()
{
	src->text = ed->toPlainText();
}


void KwEdit::doShadersUpdate()
{
	m_in.params.clear();
	m_doc->m_shaderEnabled = ui.shaderEnable->isChecked();
	if (m_doc->m_shaderEnabled)
	{
		// go over all opened docs and commit the change in the GUI to the document
		foreach(const EditPagePtr& page, m_pages)
		{
			page->commitText();
		}

		foreach(ParamUi* pui, m_paramUi)
		{
			ParamInput pi(pui->name->text(), 
				(EParamType)pui->type->itemData(pui->type->currentIndex()).toInt(), 
				pui->value->text(), pui->m_isUniform);
			pui->index = m_in.params.size();
			m_in.params.append(pi);
			
		}
	}

	m_in.runType = conf().runType;

	emit updateShaders(m_in); // calls compile

	doVarsUpdate();
	m_doc->emitProgChanged();
}

void KwEdit::doVarsUpdate()
{
	if (!m_doc->m_shaderEnabled)
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
	EditPagePtr curEd;
	if (!(curEd = m_curEd.lock()))
		return;

	int i = curEd->ed->textCursor().block().firstLineNumber();
	ui.infoLabel->setText(QString("Line %1").arg(i + 1));
}

void KwEdit::zoomEdits(int delta)
{
	int size = m_conf.editFontSize + delta;;
	m_conf.editFontSize = size;

	//ui.ed->setFontSize(size);

	foreach(const EditPagePtr& page, m_pages) 
	{
		page->ed->setFontSize(size);
	}
}

void KwEdit::editTextChanged(int pos, int rem, int add)
{
	if (rem == 0 && add == 0)
		return; // text did not change.

	shared_ptr<EditPage> curEd;
	if (!(curEd = m_curEd.lock()))
		return;

	// model requires immediate action
	if (curEd->src->type == SRC_MODEL)
	{
		curEd->commitText();
		emit changedModel(curEd->src);
	}
}

void KwEdit::saveCurDoc()
{
	EditPagePtr curEd;
	if (!(curEd = m_curEd.lock()))
		return;

	DocSrc *src = curEd->src;

	if (!src->isFilename)
	{
		QString filename = QFileDialog::getSaveFileName(this, "Save Shader", "", "glsl files (*.glsl.txt)");
		if (filename.isEmpty())
			return;
		filename = QFileInfo(filename).fileName(); // ignore directories for now
		src->setName(filename);
		src->isFilename = true;
	}

	curEd->commitText();
	Document::writeToFile(src->text, src->name());
	src->setChangedSinceLoad(false);
}

void KwEdit::modificateChanged(bool modif)
{
	EditPagePtr curEd;
	if (!(curEd = m_curEd.lock()))
		return;

	curEd->src->setChangedSinceLoad(modif);
}

void KwEdit::pageNameChanged(const QString& name)
{
	DocSrc *src = static_cast<DocSrc*>(sender());
	EditPagePtr page = findPage(src);
	ui.tabs->setTabText(ui.tabs->indexOf(page->tab), name);
}


void KwEdit::addError(DocSrc* src, int start, int count)
{
	if (src == NULL)
		return;
	EditPagePtr page = findPage(src);
	if (page.get() == NULL)
		return;
	QTextBlock b = page->ed->document()->findBlock(start);
	b.setUserData(new OffsetData(start - b.position(), count));
	errorBlockNumbers.append(b.blockNumber());
}


void KwEdit::clearErrors(DocSrc* src)
{
	if (src != NULL)
	{
		EditPagePtr page = findPage(src);
		if (page.get() != NULL) // might not be open
		{
			QTextDocument *d = page->ed->document();
			foreach(int i, errorBlockNumbers)
				d->findBlockByNumber(i).setUserData(NULL);
		}
	}
	errorBlockNumbers.clear();
}

void KwEdit::finishErrors(DocSrc* src)
{
	if (src == NULL)
		return;
	EditPagePtr page = findPage(src);
	if (page.get() == NULL || page->m_high == NULL)
		return;
	page->m_high->rehighlight();
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


void KwEdit::readModel(DocSrc* src)
{
	// automatically
	if (!m_doc->m_passes.isEmpty())
	{
		const Pass &pass = m_doc->m_passes[0];
		addPage(pass.model.get());

		//	if (ui.tabs->count() > oldIndex)
		//		ui.tabs->setCurrentIndex(oldIndex);
	}

}

void KwEdit::setText(const QString& text)
{
	//ui.ed->setPlainText(text);
}


void KwEdit::removePage(DocSrc* src)
{
	m_pages.remove(src);
}

// read a program from the reposityry
void KwEdit::readProg(ProgKeep* prog)
{

	//int oldIndex = ui.tabs->currentIndex();
	
	// automatically open the progs of pass 1
	if (!m_doc->m_passes.isEmpty()) 
	{
		const Pass &pass = m_doc->m_passes[0];
		foreach(const shared_ptr<DocSrc>& s, pass.shaders)
		{
			if (s->type != SRC_GEOM) // don't open geometry by default
				addPage(s.get());
		}
	//	if (ui.tabs->count() > oldIndex)
	//		ui.tabs->setCurrentIndex(oldIndex);
	}

	// set uniform and attributes from prog
	clearParam();

	if (prog != NULL)
	{
		m_lastLoadedProg = prog->name; // for reload
		foreach(const ParamInput& ppi, prog->params)
		{
			addParam(ppi);
		}
	}

	ui.shaderEnable->setChecked(true);

	doShadersUpdate();
}

// find an edit page by its index
EditPagePtr KwEdit::findPage(int tabi)
{
	QWidget *tab = ui.tabs->widget(tabi);
	foreach(const EditPagePtr& p, m_pages)
	{
		if (tab == p->tab)
		{
			return p;
		}
	}
	return EditPagePtr();
}

EditPagePtr KwEdit::findPage(DocSrc* src)
{
	QMap<DocSrc*, EditPagePtr>::iterator it = m_pages.find(src);
	if (it == m_pages.end())
		return EditPagePtr();
	return it.value();
}


void KwEdit::on_tabs_currentChanged(int ti) 
{
	// search the page
	if (ti == -1)
		return;  // no tabs left. 

	EditPagePtr page = findPage(ti);  

	ui.shaderControl->setVisible(page.get() != NULL && page->src->type != SRC_MODEL);
	ui.modelControl->setVisible(page.get() != NULL && page->src->type == SRC_MODEL);

	m_curEd = page;

	updateCursorPosLine();
}

bool operator==(const EditPage& a, const EditPage& b)
{
	return a.src == b.src && a.ed == b.ed && a.tab == b.tab;
}

void KwEdit::on_tabs_tabCloseRequested(int index)
{
	{
		EditPagePtr page = findPage(index);
		page->commitText();

		//ui.tabs->removeTab(index);
		m_pages.remove(page->src);
	}	
}

void KwEdit::tabsBarClose()
{
	EditPagePtr curEd;
	if (!(curEd = m_curEd.lock()))
		return;
	{
		curEd->commitText();

		//ui.tabs->removeTab(index);
		m_pages.remove(curEd->src);
	}	
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

// void KwEdit::on_reloadBot_clicked()
// {
// 	m_doc->m_confxmls.activateAction(m_lastLoadedProg);
// }


void KwEdit::addPage(DocSrc* src, int index)
{
	// check if it's already open
	foreach(const EditPagePtr& page, m_pages)
	{
		if (src == page->src) // points to the same original
		{
			ui.tabs->setCurrentWidget(page->tab);
			return;
		}
	}

	// open a new page
	EditPagePtr page(new EditPage());
	page->src = src;

	page->tab = new QWidget();
	QVBoxLayout *tabLayout = new QVBoxLayout(page->tab);
	tabLayout->setContentsMargins(0, 3, 0, 0);
	page->ed = new ProgTextEdit(page->tab);
	QFont font;
	font.setFamily(QString::fromUtf8("Courier"));
	font.setPointSize(10);
	page->ed->setFont(font);
	page->ed->setLineWrapMode(QTextEdit::NoWrap);
	page->ed->setTabStopWidth(40);
	page->ed->setAcceptRichText(false);

	page->ed->setPlainText(page->src->text);
	page->ed->document()->setModified(false);

	tabLayout->addWidget(page->ed);

	if (src->type != SRC_MODEL)
	{
		page->ed->setMenu(m_doc->m_confxmls.m_progMenu);
	}
	else
	{
		page->ed->setMenu(m_doc->m_confxmls.m_modelsMenu);
		page->m_high = new MySyntaxHighlighter(page->ed->document(), this);
		index = 0;
	}


	connect(page->ed, SIGNAL(zoomChange(int)), this, SLOT(zoomEdits(int)));
	connect(page->ed, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursorPosLine()));
	connect(page->ed->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(editTextChanged(int,int,int)));
	connect(page->ed->document(), SIGNAL(modificationChanged(bool)), this, SLOT(modificateChanged(bool)));

	connect(page->src, SIGNAL(removed(DocSrc*)), this, SLOT(removePage(DocSrc*)));
	connect(page->src, SIGNAL(nameChanged(const QString&)), this, SLOT(pageNameChanged(const QString&)));

	m_pages.insert(page->src, page);
	if (index == -1)
		ui.tabs->addTab(page->tab, Document::getTypeIcon(page->src->type), page->src->displayName());
	else
		ui.tabs->insertTab(index, page->tab, Document::getTypeIcon(page->src->type), page->src->displayName());
	ui.tabs->setCurrentWidget(page->tab);
}