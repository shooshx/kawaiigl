#pragma once
#ifndef KWEDIT_H
#define KWEDIT_H

#include <QDialog>
#include <QTextEdit>

#include "ui_KwEdit.h"
#include "general.h"
#include "MyLib/MyDialog.h"
#include "ProgInput.h"

class QLineEdit;
class QComboBox;

class OffsetData;
class MySyntaxHighlighter;
class DisplayConf;
class Document;
class Prop;
class WidgetIn;
class KwEdit;
class T2GLWidget;
class ParamUi;
class DocSrc;
class ProgTextEdit;
class DocElement;
class ShaderConfigDlg;
class Pass;
class ProgKeep;

struct KPage
{
	KPage() : tab(NULL), elem(NULL) {}
	virtual ~KPage();
	virtual void commit() {}
	virtual void postCompile() {}
	bool textual() const;

	QWidget* tab;
	DocElement* elem; // same as src or pass
};

struct EditPage : public KPage
{
	EditPage() : src(NULL), ed(NULL), m_high(NULL) {}
	virtual void commit(); // commit the changes in the edit widget to the DocSrc

	DocSrc* src;
	ProgTextEdit* ed;
	MySyntaxHighlighter *m_high; // the edit widget deletes this.
};

// we kinda don't need this class. TBD
struct DlgPage : public KPage
{
	DlgPage() : pass(NULL), dlg(NULL) {}
	virtual void commit();
	virtual void postCompile();

	Pass* pass;
	ShaderConfigDlg* dlg;
};

typedef shared_ptr<KPage> KPagePtr;
typedef shared_ptr<EditPage> EditPagePtr;
typedef shared_ptr<DlgPage> DlgPagePtr;



class KwEdit : public MyDialog
{
	Q_OBJECT
public:
	KwEdit(QWidget *parent, DisplayConf& conf, Document* doc, T2GLWidget *view);
	~KwEdit()
	{
		foreach(const KPagePtr& page, m_pages)
			page->tab = NULL; // throw hands in air and cry out "fuck this shit"
		m_pages.clear();
	}

	void addError(DocSrc* src, int start, int count);
	void clearErrors(DocSrc* src);
	void finishErrors(DocSrc* src);

	DisplayConf& conf() { return m_conf; }
	T2GLWidget* view() { return m_view; }
	bool isEnabled();

public slots:
	void readModel(DocSrc* src);
	void readProg(ProgKeep* prog);
	void clearingProg();

	void doShadersUpdate();
	void addModelLine(const QString& line);

	QWidget* addPage(DocElement*, int index = -1);

	void removePage(DocElement* src);

private slots:
	// void on_reloadBot_clicked();
	void on_tabs_currentChanged(int);
	void on_tabs_tabCloseRequested(int index);

	void tabsBarClose();
	void pageNameChanged(const QString&);
	//void saveCurDoc();
	void decompile();

	void updateCursorPosLine();
	void zoomEdits(int delta);

	void editTextChanged(int pos, int rem, int add);
	void modificateChanged(bool modif);

signals:
	void changedModel(DocSrc* txt);
	void updateShaders();

public:
	Ui::KwEdit ui;

private:
	QAction* confAddModel(const QString& display, const QString& filename, bool isMesh);
	QAction* confAddProg(const QString& display, ProgKeep* prog); // taking over this.

	KPagePtr findPage(int tabi);
	KPagePtr findPage(DocElement* src);
	EditPagePtr findEditPage(DocSrc* src);


	KPagePtr addDlgPage(RenderPass* pass);
	KPagePtr addTextPage(DocSrc* src, int& index);
	
private:

	typedef QMap<DocElement*, KPagePtr> TPages;
	TPages m_pages;

	weak_ptr<KPage> m_curEd;
	QString m_lastLoadedProg;

	Document *m_doc;
	T2GLWidget *m_view;
	int m_lastActiveTabIndex;

	DisplayConf &m_conf;
	
	QList<int> errorBlockNumbers; // move somewhere else TBD

	friend class ConfXmls;
	friend class ParamUi;
};



#endif // KWEDIT_H
