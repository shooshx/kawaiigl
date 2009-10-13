#ifndef PROJBROWSER_H
#define PROJBROWSER_H

#include <QWidget>
#include "ui_ProjBrowser.h"

class KawaiiGL;
class Document;
class DocSrc;
struct ProgKeep;
struct Pass;

class MyTreeItem : public QObject, public QTreeWidgetItem
{
	Q_OBJECT
public:
	MyTreeItem(QTreeWidgetItem *parent, const QStringList & strings, int type)
		:QObject(NULL), QTreeWidgetItem(parent, strings, type), m_pass(NULL), m_src(NULL), m_itIsIChanging(false)
	{}
	MyTreeItem(QTreeWidget *parent, const QStringList & strings, int type)
		:QObject(NULL), QTreeWidgetItem(parent, strings, type), m_pass(NULL), m_src(NULL), m_itIsIChanging(false)
	{}

public slots:
	void updateDisplay(const QString& name);

public:
	Pass* m_pass;
	DocSrc* m_src; // may be NULL if it's not a doc

	bool m_itIsIChanging;
};

class ProjBrowser : public QWidget
{
	Q_OBJECT
public:
	ProjBrowser(KawaiiGL* kw, Document *doc);
	~ProjBrowser() {}

private slots:
	void itemClicked(QTreeWidgetItem *item, int col);
	void itemChanged(QTreeWidgetItem *item, int col);

public slots:
	void readProg(ProgKeep *);
	void readModel();
	void addDoc();
	void removeDoc();
	void renameDoc();

signals:
	void openDocText(DocSrc* sdoc);

private:
	void updateTree();
	void addItem(Pass* pass, MyTreeItem *parent, DocSrc* t);

private:
	Ui::ProjBrowserClass ui;
	Document *m_doc;

};

#endif // PROJBROWSER_H
