#include "ProjBrowser.h"
#include "KawaiiGL.h"
#include "Document.h"

#include <boost/shared_ptr.hpp>
using namespace boost;

#define ITEM_PASS (QTreeWidgetItem::UserType + 1)
#define ITEM_DOC (QTreeWidgetItem::UserType + 2)


#define PTR_ROLE (Qt::UserRole + 1)

Q_DECLARE_METATYPE(DocSrc*);

ProjBrowser::ProjBrowser(KawaiiGL* kw, Document *doc)
	: QWidget(kw), m_doc(doc)
{
	ui.setupUi(this);

	updateTree();

	connect(ui.tree, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));
	connect(ui.tree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemChanged(QTreeWidgetItem *, int)));

	ui.loadBut->setMenu(m_doc->m_confxmls.m_progMenu);
	ui.loadModelBut->setMenu(m_doc->m_confxmls.m_modelsMenu);

	QMenu *passMenu = new QMenu(this);
	QAction *act;
	act = new QAction(Document::getTypeIcon(SRC_VTX), "New Vertex Shader", this);
	act->setData(SRC_VTX);
	connect(act, SIGNAL(triggered()), this, SLOT(addDoc()));
	passMenu->addAction(act);

	act = new QAction(Document::getTypeIcon(SRC_FRAG), "New Fragment Shader", this);
	act->setData(SRC_FRAG);
	connect(act, SIGNAL(triggered()), this, SLOT(addDoc()));
	passMenu->addAction(act);

	act = new QAction(Document::getTypeIcon(SRC_GEOM), "New Geometry Shader", this);
	act->setData(SRC_GEOM);
	connect(act, SIGNAL(triggered()), this, SLOT(addDoc()));
	passMenu->addAction(act);

	QMenu *docMenu = new QMenu(this);
	act = new QAction(QIcon(), "Remove", this);
	connect(act, SIGNAL(triggered()), this, SLOT(removeDoc()));
	docMenu->addAction(act);
	act = new QAction(QIcon(), "Rename", this);
	connect(act, SIGNAL(triggered()), this, SLOT(renameDoc()));
	docMenu->addAction(act);

	ui.tree->setTypeMenu(ITEM_PASS, passMenu);
	ui.tree->setTypeMenu(ITEM_DOC, docMenu);
}

void ProjBrowser::addDoc()
{
	MyTreeItem* passItem = static_cast<MyTreeItem*>(ui.tree->eventItem());
	int type = ((QAction*)sender())->data().toInt();
	m_doc->addNewShader(passItem->m_pass, (ElementType)type);
	updateTree();
}

void ProjBrowser::removeDoc()
{
	MyTreeItem* item = static_cast<MyTreeItem*>(ui.tree->eventItem());
	m_doc->removeShader(item->m_pass, item->m_src);
	updateTree();
}

void ProjBrowser::renameDoc()
{
	MyTreeItem* item = static_cast<MyTreeItem*>(ui.tree->eventItem());
	Qt::ItemFlags oldf = item->flags();
	item->m_itIsIChanging = true;
	item->setFlags(oldf | Qt::ItemIsEditable);

	item->updateDisplay(item->m_src->name()); // get rid of the * for editing
	item->m_itIsIChanging = true;
	ui.tree->editItem(item, 0);
	item->m_itIsIChanging = false;
	item->setFlags(oldf);

}


void ProjBrowser::addItem(Pass* pass, MyTreeItem *parent, DocSrc* t)
{
	if (t == NULL)
		return;
	MyTreeItem *item = new MyTreeItem(parent, QStringList(t->displayName()), ITEM_DOC);
	item->m_pass = pass;
	item->m_src = t;
	item->setIcon(0, Document::getTypeIcon(t->type));
	
	connect(t, SIGNAL(nameChanged(const QString&)), item, SLOT(updateDisplay(const QString&)));
}

void MyTreeItem::updateDisplay(const QString& name)
{
	if (name == text(0))
		return;
	m_itIsIChanging = true;
	setText(0, name);
	m_itIsIChanging = false;
}


void ProjBrowser::updateTree()
{
	ui.tree->clear();
	for(int i = 0; i < m_doc->m_passes.size(); ++i)
	{
		PassPtr& pass = m_doc->m_passes[i];
		MyTreeItem *passItem = new MyTreeItem((QTreeWidget*)NULL, QStringList(pass->displayName()), ITEM_PASS);
		passItem->m_pass = pass.get();
		
		QList<QTreeWidgetItem *> items;
		items.append(passItem);
		
		addItem(pass.get(), passItem, pass->model.get());
		foreach(const shared_ptr<DocSrc>& t, pass->shaders)
		{
			addItem(pass.get(), passItem, t.get());
		}

		ui.tree->insertTopLevelItems(0, items);
	}

	ui.tree->expandAll();
}

void ProjBrowser::readProg(ProgKeep*)
{
	updateTree();
}
void ProjBrowser::readModel()
{
	updateTree();
}

void ProjBrowser::itemClicked(QTreeWidgetItem *item, int col)
{
	if (item == 0)
		return;
	if (item->type() == ITEM_PASS)
	{
		Pass* pass = ((MyTreeItem*)item)->m_pass;
		emit openPassConf(pass);
	}
	else
	{
		DocSrc* sdoc = ((MyTreeItem*)item)->m_src;
		emit openDocText(sdoc);
	}
}


void ProjBrowser::itemChanged(QTreeWidgetItem *item, int col)
{
	if (item == 0)
		return;
	if (item->type() != ITEM_DOC)
		return;
	
	MyTreeItem* mitem = static_cast<MyTreeItem*>(item);
	if ( mitem->m_itIsIChanging)
		return;

	mitem->m_src->setName(item->text(0));
	
}