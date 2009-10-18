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
#include "ShaderConfigDlg.h"

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

static QColor ERROR_COL = QColor(255, 128, 128); // duplicate?


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

	//ui.tabs->setCurrentIndex(m_sett.gui.configWindowTab);

}



bool KwEdit::isEnabled() 
{ 
	return m_doc->m_shaderEnabled; 
}

KPage::~KPage()
{
	delete tab; // remove the tab from the tabs
}

void EditPage::commit()
{
	src->text = ed->toPlainText();
}

void DlgPage::commit()
{
	dlg->pparamsContainer()->commit();
}
void DlgPage::postCompile() 
{
	dlg->pparamsContainer()->postCompile();
}


void KwEdit::doShadersUpdate()
{
	m_doc->m_shaderEnabled = ui.shaderEnable->isChecked();
	if (m_doc->m_shaderEnabled)
	{
		// go over all opened docs and commit the change in the GUI to the document
		foreach(const KPagePtr& page, m_pages)
		{
			page->commit();
		}
		// commits all texts the the Pass instance
		// and params
	}

//	m_in.runType = conf().runType;

	emit updateShaders(); // calls compile

	foreach(const KPagePtr& page, m_pages)
	{
		page->postCompile(); // update the colors of params
	}

	//doVarsUpdate();
	m_doc->emitProgChanged();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////

bool KPage::textual() const 
{ 
	return elem->textual(); 
}


void KwEdit::updateCursorPosLine()
{
	KPagePtr curEd;
	if (!(curEd = m_curEd.lock()))
		return;
	if (!curEd->textual())
		return;

	int i = ((EditPage*)curEd.get())->ed->textCursor().block().firstLineNumber();
	ui.infoLabel->setText(QString("Line %1").arg(i + 1));
}

void KwEdit::zoomEdits(int delta)
{
	int size = m_conf.editFontSize + delta;;
	m_conf.editFontSize = size;

	//ui.ed->setFontSize(size);

	foreach(const KPagePtr& page, m_pages) 
	{
		static_cast<EditPage*>(page.get())->ed->setFontSize(size);
	}
}

void KwEdit::editTextChanged(int pos, int rem, int add)
{
	if (rem == 0 && add == 0)
		return; // text did not change.

	shared_ptr<KPage> curEd;
	if (!(curEd = m_curEd.lock()))
		return;

	// model requires immediate action
	if (curEd->elem->type == SRC_MODEL)
	{
		curEd->commit();
		emit changedModel(static_cast<EditPage*>(curEd.get())->src);
	}
}

void KwEdit::saveCurDoc()
{
	KPagePtr curEd;
	if (!(curEd = m_curEd.lock()))
		return;
	if (!curEd->textual())
		return;

	DocSrc *src = static_cast<EditPage*>(curEd.get())->src;

	if (!src->isFilename)
	{
		QString filename = QFileDialog::getSaveFileName(this, "Save Shader", QString(src->name()).replace(" ", "_"), "glsl files (*.glsl.txt)");
		if (filename.isEmpty())
			return;
		filename = QFileInfo(filename).fileName(); // ignore directories for now
		src->setName(filename);
		src->isFilename = true;
	}

	curEd->commit();
	Document::writeToFile(src->text, src->name());
	src->setChangedSinceLoad(false);
}

void KwEdit::modificateChanged(bool modif)
{
	KPagePtr curEd;
	if (!(curEd = m_curEd.lock()))
		return;
	if (!curEd->textual())
		return;

	static_cast<EditPage*>(curEd.get())->src->setChangedSinceLoad(modif);
}

void KwEdit::pageNameChanged(const QString& name)
{
	DocSrc *src = static_cast<DocSrc*>(sender());
	KPagePtr page = findPage(src);
	ui.tabs->setTabText(ui.tabs->indexOf(page->tab), name);
}


void KwEdit::addError(DocSrc* src, int start, int count)
{
	if (src == NULL)
		return;
	EditPagePtr page = findEditPage(src);
	if (page.get() == NULL)
		return;

	QTextBlock b = page.get()->ed->document()->findBlock(start);
	b.setUserData(new OffsetData(start - b.position(), count));
	errorBlockNumbers.append(b.blockNumber());
}


void KwEdit::clearErrors(DocSrc* src)
{
	if (src != NULL)
	{
		EditPagePtr page = findEditPage(src);
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
	EditPagePtr page = findEditPage(src);
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
		const PassPtr &pass = m_doc->m_passes[0];
		addPage(pass->model.get());

		//	if (ui.tabs->count() > oldIndex)
		//		ui.tabs->setCurrentIndex(oldIndex);
	}

}

void KwEdit::setText(const QString& text)
{
	//ui.ed->setPlainText(text);
}


void KwEdit::removePage(DocElement* src)
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
		const PassPtr &pass = m_doc->m_passes[0];
		foreach(const shared_ptr<DocSrc>& s, pass->shaders)
		{
			if (s->type != SRC_GEOM || !s->text.isEmpty()) // don't open geometry by default
				addPage(s.get());
		}
	//	if (ui.tabs->count() > oldIndex)
	//		ui.tabs->setCurrentIndex(oldIndex);
	}

	// set uniform and attributes from prog
	//clearParam();

	if (prog != NULL)
	{
		m_lastLoadedProg = prog->name; // for reload

	}

	ui.shaderEnable->setChecked(true);

	doShadersUpdate();
}

// find an edit page by its index
KPagePtr KwEdit::findPage(int tabi)
{
	QWidget *tab = ui.tabs->widget(tabi);
	foreach(const KPagePtr& p, m_pages)
	{
		if (tab == p->tab)
		{
			return p;
		}
	}
	return KPagePtr();
}

EditPagePtr KwEdit::findEditPage(DocSrc* src)
{
	TPages::iterator it = m_pages.find(src);
	if (it == m_pages.end() )
		return EditPagePtr();
	return static_pointer_cast<EditPage>(it.value());
}

KPagePtr KwEdit::findPage(DocElement* src)
{
	TPages::iterator it = m_pages.find(src);
	if (it == m_pages.end())
		return KPagePtr();
	return it.value();
}


void KwEdit::on_tabs_currentChanged(int ti) 
{
	// search the page
	if (ti == -1)
		return;  // no tabs left. 

	KPagePtr page = findPage(ti);  

	ui.shaderControl->setVisible(page.get() != NULL && page->elem->type != SRC_MODEL);
	ui.modelControl->setVisible(page.get() != NULL && page->elem->type == SRC_MODEL);
	updateCursorPosLine();

	m_curEd = page;

	
}


void KwEdit::on_tabs_tabCloseRequested(int index)
{
	{
		KPagePtr page = findPage(index);
		page->commit();
		m_pages.remove(page->elem);
	}	
}

void KwEdit::tabsBarClose()
{
	KPagePtr curEd;
	if (!(curEd = m_curEd.lock()))
		return;
	{
		curEd->commit();
		m_pages.remove(curEd->elem);
	}	
}





// void KwEdit::on_reloadBot_clicked()
// {
// 	m_doc->m_confxmls.activateAction(m_lastLoadedProg);
// }

KPagePtr KwEdit::addTextPage(DocSrc* src, int& index)
{
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


	return page;
}



KPagePtr KwEdit::addDlgPage(Pass* pass)
{
	DlgPagePtr page(new DlgPage());
	page->pass = pass;

	page->dlg = new ShaderConfigDlg(this, pass, view(), m_doc, m_conf);
	page->tab = page->dlg;

	return page;
}

void KwEdit::addPage(DocElement* elem, int index)
{
	// check if it's already open
	TPages::iterator it = m_pages.find(elem);
	if (it != m_pages.end())
	{
		ui.tabs->setCurrentWidget((*it)->tab);
		return;
	}

	KPagePtr page;
	if (elem->textual())
		page = addTextPage(static_cast<DocSrc*>(elem), index);
	else
		page = addDlgPage(static_cast<Pass*>(elem));

	page->elem = elem;

	connect(page->elem, SIGNAL(removed(DocElement*)), this, SLOT(removePage(DocElement*)));
	connect(page->elem, SIGNAL(nameChanged(const QString&)), this, SLOT(pageNameChanged(const QString&)));

	m_pages.insert(page->elem, page);
	if (index == -1)
		ui.tabs->addTab(page->tab, Document::getTypeIcon(page->elem->type), page->elem->displayName());
	else
		ui.tabs->insertTab(index, page->tab, Document::getTypeIcon(page->elem->type), page->elem->displayName());
	ui.tabs->setCurrentWidget(page->tab);
}

