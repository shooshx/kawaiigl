#pragma once
#ifndef KWEDIT_H
#define KWEDIT_H

#include <QDialog>
#include <QTextEdit>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "ui_KwEdit.h"
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


struct EditPage
{
	EditPage() : src(NULL), ed(NULL), tab(NULL) {}
	virtual ~EditPage();
	void commitText(); // commit the changes in the edit widget to the DocSrc

	DocSrc* src;
	ProgTextEdit* ed;
	QWidget* tab;
	MySyntaxHighlighter *m_high; // the edit widget deletes this.
};

typedef boost::shared_ptr<EditPage> EditPagePtr;

class KwEdit : public MyDialog
{
	Q_OBJECT
public:
	KwEdit(QWidget *parent, DisplayConf& conf, Document* doc, T2GLWidget *view);
	~KwEdit()
	{
		foreach(const EditPagePtr& page, m_pages)
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
	void doShadersUpdate();
	void doVarsUpdate();
	void setText(const QString& text);

	void addPage(DocSrc*, int index = -1);
	void removePage(DocSrc* src);

private slots:
	// void on_reloadBot_clicked();
	void on_addParam_clicked();
	void on_tabs_currentChanged(int);
	void on_tabs_tabCloseRequested(int index);

	void tabsBarClose();
	void pageNameChanged(const QString&);
	void saveCurDoc();

	void updateCursorPosLine();
	void zoomEdits(int delta);
	void removeParam();

	void doVarUpdate(); // sent from the LineEdit
	void editTextChanged(int pos, int rem, int add);
	void modificateChanged(bool modif);

signals:
	void changedModel(DocSrc* txt);
	void updateShaders(const ProgInput& in);
	void updateVars(const ProgInput& in);

public:
	Ui::KwEdit ui;

private:
	QAction* confAddModel(const QString& display, const QString& filename, bool isMesh);
	QAction* confAddProg(const QString& display, ProgKeep* prog); // taking over this.

	EditPagePtr findPage(int tabi);
	EditPagePtr findPage(DocSrc* src);

	void addParam(const ParamInput& pi);
	void removeParam(ParamUi *pui);
	void clearParam();
	void doVarUpdate(const ParamUi* pui, bool update = true);
	void initMoreWidget(ParamUi *pui);

	QVector<ParamUi*> m_paramUi;
	QMap<DocSrc*, EditPagePtr> m_pages;

	boost::weak_ptr<EditPage> m_curEd;
	QString m_lastLoadedProg;

	Document *m_doc;
	T2GLWidget *m_view;


	DisplayConf &m_conf;


	ProgInput m_in;


	QString generateFromFile();

	
	QList<int> errorBlockNumbers;

	friend class ConfXmls;
	friend class ParamUi;
};



#endif // KWEDIT_H
