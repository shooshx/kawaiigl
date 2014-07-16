#include "ProjBrowser.h"
#include "KawaiiGL.h"
#include "Document.h"

#define ITEM_PASS (QTreeWidgetItem::UserType + 1)
#define ITEM_DOC (QTreeWidgetItem::UserType + 2)
#define ITEM_OP (QTreeWidgetItem::UserType + 3)


#define PTR_ROLE (Qt::UserRole + 1)

Q_DECLARE_METATYPE(DocSrc*);

ProjBrowser::ProjBrowser(KawaiiGL* kw, Document *doc)
    : QWidget(kw), m_doc(doc)
{
    ui.setupUi(this);

    updateTree();

    connect(ui.tree, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(ui.tree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemChanged(QTreeWidgetItem *, int)));

    ui.loadBut->setMenu(kw->m_progMenu);
    ui.loadModelBut->setMenu(kw->m_modelsMenu);

    QMenu *passMenu = new QMenu(this);
    QAction *act;
    QMenu *newShaderMenu = new QMenu("New Shader...");
    passMenu->addMenu(newShaderMenu);

    newShaderMenu->addAction(act = new QAction(Document::getTypeIcon(SRC_MODEL), "Model", this));
    act->setData(SRC_MODEL);
    connect(act, SIGNAL(triggered()), this, SLOT(addDoc()));

    newShaderMenu->addAction(act = new QAction(Document::getTypeIcon(SRC_VTX), "Vertex", this));
    act->setData(SRC_VTX);
    connect(act, SIGNAL(triggered()), this, SLOT(addDoc()));

    newShaderMenu->addAction(act = new QAction(Document::getTypeIcon(SRC_FRAG), "Fragment", this));
    act->setData(SRC_FRAG);
    connect(act, SIGNAL(triggered()), this, SLOT(addDoc()));

    newShaderMenu->addAction(act = new QAction(Document::getTypeIcon(SRC_GEOM), "Geometry", this));
    act->setData(SRC_GEOM);
    connect(act, SIGNAL(triggered()), this, SLOT(addDoc()));

    QMenu *existShaderMenu = new QMenu("Load exisiting...", passMenu);
    passMenu->addMenu(existShaderMenu);

/* phasing out the concept of stand alone shader files
    existShaderMenu->addAction(act = new QAction(Document::getTypeIcon(SRC_VTX), "Vertex", this));
    act->setData(SRC_VTX);
    connect(act, SIGNAL(triggered()), this, SLOT(existingDoc()));

    existShaderMenu->addAction(act = new QAction(Document::getTypeIcon(SRC_FRAG), "Fragment", this));
    act->setData(SRC_FRAG);
    connect(act, SIGNAL(triggered()), this, SLOT(existingDoc()));

    existShaderMenu->addAction(act = new QAction(Document::getTypeIcon(SRC_GEOM), "Geometry", this));
    act->setData(SRC_GEOM);
    connect(act, SIGNAL(triggered()), this, SLOT(existingDoc()));

    passMenu->addSeparator();
*/
    passMenu->addAction(act = new QAction(QIcon(), "Add Pass", this));
    connect(act, SIGNAL(triggered()), this, SLOT(addPass()));

    QMenu *nothingMenu = new QMenu(this); // menu of the background
    nothingMenu->addAction(act);

    passMenu->addAction(act = new QAction(QIcon(), "Add swap operation", this));
    connect(act, SIGNAL(triggered()), this, SLOT(addSwap()));
    nothingMenu->addAction(act);

    QMenu *swapMenu = new QMenu(this);
    
    QMenu *docMenu = new QMenu(this);
    act = new QAction(QIcon(), "Remove", this);
    connect(act, SIGNAL(triggered()), this, SLOT(on_removeBut_clicked()));
    act->setShortcut(QKeySequence(Qt::Key_Delete));
    docMenu->addAction(act);
    passMenu->addAction(act);
    ui.tree->addAction(act);
    swapMenu->addAction(act);

    act = new QAction(QIcon(), "Rename", this);
    connect(act, SIGNAL(triggered()), this, SLOT(renameFromMenu()));
    act->setShortcut(QKeySequence(Qt::Key_F2));
    docMenu->addAction(act);
    passMenu->addAction(act);
    ui.tree->addAction(act);

    ui.tree->setTypeMenu(ITEM_PASS, passMenu);
    ui.tree->setTypeMenu(ITEM_DOC, docMenu);
    ui.tree->setTypeMenu(-1, nothingMenu);
    ui.tree->setTypeMenu(ITEM_OP, swapMenu);
}

void ProjBrowser::addDoc()
{
    MyTreeItem* passItem = static_cast<MyTreeItem*>(ui.tree->eventItem());
    int type = ((QAction*)sender())->data().toInt();
    m_doc->addNewShader(passItem->m_rpass, (ElementType)type);
    updateTree();
}

void ProjBrowser::removeFromMenu()
{
    QTreeWidgetItem* item = ui.tree->eventItem();
    if (item == NULL)
        item = ui.tree->currentItem();
    if (item == NULL)
        return;
    MyTreeItem* mitem = static_cast<MyTreeItem*>(item);

    if (mitem->type() != ITEM_PASS)
        m_doc->removeShader(mitem->m_rpass, mitem->m_src);
    else
        m_doc->removePass(mitem->m_pass);
    updateTree();
}

void ProjBrowser::existingDoc()
{
    int type = ((QAction*)sender())->data().toInt();
    QStringList filenames = QFileDialog::getOpenFileNames(this, QString("Load ") + Document::getTypeName((ElementType)type) + " Shader", 
                                "", "model files (*.txt *.glsl)");
    
    MyTreeItem* item = static_cast<MyTreeItem*>(ui.tree->eventItem());
    foreach(const QString& filename, filenames)
    {
        if (filename.isEmpty())
            return;
        QString name = QFileInfo(filename).fileName(); // remove directory
        m_doc->loadShaderFile(item->m_rpass, name, (ElementType)type);
    }
    updateTree();
}

void ProjBrowser::renameFromMenu()
{
    QTreeWidgetItem* item = ui.tree->eventItem();
    if (item == NULL)
        item = ui.tree->currentItem();
    if (item == NULL)
        return;

    MyTreeItem* mitem = static_cast<MyTreeItem*>(item);

    Qt::ItemFlags oldf = mitem->flags();
    mitem->m_itIsIChanging = true;
    mitem->setFlags(oldf | Qt::ItemIsEditable);

    mitem->updateDisplay(mitem->m_elem->name()); // get rid of the * for editing
    mitem->m_itIsIChanging = true;
    ui.tree->editItem(mitem, 0);
    mitem->m_itIsIChanging = false;
    mitem->setFlags(oldf);
}

void ProjBrowser::on_saveBut_clicked() {
    QString filename = QFileDialog::getSaveFileName(this, "Save Program", QString(), "xml files (*.xml)");
    if (filename.isEmpty())
        return;
    emit commitGuiData(); // anything edited in the gui, to commit to the document
    m_doc->m_confxmls.saveProg(*m_doc, filename);
}

/*
void ProjBrowser::on_loadBut_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Program", QString(), "xml files (*.xml)");
    if (filename.isEmpty())
        return;

    m_doc->m_confxmls.loadProg(*m_doc, filename);
}
*/

void ProjBrowser::addPass()
{
//	MyTreeItem* item = static_cast<MyTreeItem*>(ui.tree->eventItem());
    m_doc->addNewPass();
    updateTree();
}

void ProjBrowser::addSwap()
{
    m_doc->addNewSwap();
    updateTree();
}


void MyTreeItem::updateDisplay(const QString& name)
{
    if (name == text(0))
        return;
    m_itIsIChanging = true;
    setText(0, name);
    m_itIsIChanging = false;
}


// part of updateTree
void ProjBrowser::addItem(Pass* pass, MyTreeItem *parent, DocSrc* t, Pass* selpass, DocSrc* seldoc)
{
    if (t == NULL)
        return;
    MyTreeItem *item = new MyTreeItem(NULL, QStringList(t->displayName()), ITEM_DOC);
    item->m_pass = pass;
    item->m_rpass = dynamic_cast<RenderPass*>(pass);
    item->m_elem = item->m_src = t;
    item->setIcon(0, Document::getTypeIcon(t->type));
    if (item->m_pass == selpass && item->m_src == seldoc)
        ui.tree->setCurrentItem(item);

    connect(t, SIGNAL(nameChanged(const QString&)), item, SLOT(updateDisplay(const QString&)));
    parent->addChild(item);
}


void ProjBrowser::updateTree(Pass* selpass, DocSrc* seldoc)
{
    ui.tree->clear();
    for(int i = 0; i < m_doc->m_passes.size(); ++i)
    {
        PassPtr& pass = m_doc->m_passes[i];
        MyTreeItem *item = NULL;

        if (pass->type == SRC_RENDER)
        {
            RenderPass *rpass = dynamic_cast<RenderPass*>(pass.get());
            item = new MyTreeItem(NULL, QStringList(pass->displayName()), ITEM_PASS);
            item->m_elem = item->m_pass = item->m_rpass = rpass;

            item->setIcon(0, Document::getTypeIcon(pass->type));
                    
            addItem(pass.get(), item, rpass->model.get());
            foreach(const shared_ptr<DocSrc>& t, rpass->shaders)
            {
                addItem(pass.get(), item, t.get());
            }
        }
        else if (pass->type == SRC_OP_SWAP)
        {
            SwapOpPass *spass = dynamic_cast<SwapOpPass*>(pass.get());
            item = new MyTreeItem(NULL, QStringList(pass->displayName()), ITEM_OP);
            item->m_elem = item->m_pass = spass;

            item->setIcon(0, Document::getTypeIcon(spass->type));
        }

        ui.tree->addTopLevelItem(item);
        if (item->m_pass == selpass && item->m_src == seldoc) // src is NULL
            ui.tree->setCurrentItem(item);

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
        RenderPass* pass = ((MyTreeItem*)item)->m_rpass;
        emit openPassConf(pass);
    }
    else if (item->type() == ITEM_DOC)
    {
        DocSrc* sdoc = ((MyTreeItem*)item)->m_src;
        emit openDocText(sdoc);
    }
    else if (item->type() == ITEM_OP)
    {
        SwapOpPass* pass = static_cast<SwapOpPass*>(static_cast<MyTreeItem*>(item)->m_pass);
        ParamTemp prm("Selection");
        static PassConf pc;
        TypeProp<PassConf::ERenderTo> a(&prm, &pc, "_m", "swap the frame buffer of", pass->a);
        TypeProp<PassConf::ERenderTo> b(&prm, &pc, "_m", "with the frame buffer of", pass->b);
        prm.reset();

        if (!MyInputDlg::getValues(this, a, b, "Swap two frame buffers"))
            return;
        pass->a = a;
        pass->b = b;
    }
}


void ProjBrowser::itemChanged(QTreeWidgetItem *item, int col)
{
    if (item == 0)
        return;
    
    MyTreeItem* mitem = static_cast<MyTreeItem*>(item);
    if ( mitem->m_itIsIChanging)
        return;

    mitem->m_elem->setName(item->text(0));
}

void ProjBrowser::on_upBut_clicked()
{
    QTreeWidgetItem* item = ui.tree->currentItem();
    if (item == NULL)
        return;
    MyTreeItem* mitem = static_cast<MyTreeItem*>(item);
    if (mitem->type() != ITEM_PASS)
        return;
    m_doc->movePass(mitem->m_pass, -1);
    updateTree(mitem->m_pass); // the tree still exists when this is evaluated
}

void ProjBrowser::on_downBut_clicked()
{
    QTreeWidgetItem* item = ui.tree->currentItem();
    if (item == NULL)
        return;
    MyTreeItem* mitem = static_cast<MyTreeItem*>(item);
    if (mitem->type() != ITEM_PASS)
        return;
    m_doc->movePass(mitem->m_pass, 1);
    updateTree(mitem->m_pass);
}

void ProjBrowser::on_removeBut_clicked()
{
    QList<QTreeWidgetItem*> items = ui.tree->selectedItems();

    foreach(QTreeWidgetItem* item, items)
    {
        MyTreeItem* mitem = static_cast<MyTreeItem*>(item);
        if (mitem->type() == ITEM_DOC)
            m_doc->removeShader(mitem->m_rpass, mitem->m_src);		
        else
            m_doc->removePass(mitem->m_pass);
    }

    updateTree();
}