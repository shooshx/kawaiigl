#ifndef PROJBROWSER_H
#define PROJBROWSER_H

#include <QWidget>
#include "ui_ProjBrowser.h"

class KawaiiGL;
class Document;
class DocSrc;
class ProgKeep;
class Pass;
class RenderPass;
class DocElement;

class MyTreeItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    MyTreeItem(QTreeWidgetItem *parent, const QStringList & strings, int type)
        :QObject(NULL), QTreeWidgetItem(parent, strings, type), m_rpass(NULL), m_pass(NULL), m_src(NULL), m_elem(NULL),
         m_itIsIChanging(false)
    {}

public slots:
    void updateDisplay(const QString& name);

public:
    DocElement *m_elem;
    Pass* m_pass;
    DocSrc* m_src; // may be NULL if it's not a doc
    RenderPass* m_rpass; // may be NULL 

    bool m_itIsIChanging;
};

class ProjBrowser : public QWidget
{
    Q_OBJECT
public:
    ProjBrowser(KawaiiGL* kw, Document *doc);
    ~ProjBrowser() {}

private slots:
    void on_removeBut_clicked();
    void on_downBut_clicked();
    void on_upBut_clicked();
    void on_saveBut_clicked();
    //void on_loadBut_clicked();
    void itemClicked(QTreeWidgetItem *item, int col);
    void itemChanged(QTreeWidgetItem *item, int col);

public slots:
    void readProg(ProgKeep *);
    void readModel();
    void addDoc();
    void removeFromMenu();
    void renameFromMenu();
    void existingDoc();
    void addPass();
    void addSwap();

signals:
    void openDocText(DocElement* sdoc);
    void openPassConf(DocElement* pass);
    void commitGuiData();

private:
    // optionally set the selected item as well
    void updateTree(Pass* selpass = NULL, DocSrc* seldoc = NULL);
    void addItem(Pass* pass, MyTreeItem *parent, DocSrc* t, Pass* selpass = NULL, DocSrc* seldoc = NULL);

private:
    Ui::ProjBrowserClass ui;
    Document *m_doc;

};

#endif // PROJBROWSER_H
