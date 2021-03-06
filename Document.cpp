#include "Document.h"
#include "ShapeIFS.h"
#include "DisplayConf.h"
#include "KawaiiGL.h"

#include "MyLib/MeshIO.h"
#include "OpenGL/glGlob.h"
#include "ProgInput.h"
#include "ShapeIFS.h"

#include <QFileDialog>

// ADDTYPE
template<> bool isEType(EParamType t, const float&) { return (t == EPFloatRange || t == EPFloat || t == EPFloatTime); }
template<> bool isEType(EParamType t, const int& v) { return (t == EPInt || t == EPTexture); }
template<> bool isEType(EParamType t, const Vec3&) { return (t == EPVec3 || t == EPVec3Color); }
template<> bool isEType(EParamType t, const Vec2&) { return (t == EPVec2); }
template<> bool isEType(EParamType t, const Vec4&) { return (t == EPVec4 || t == EPVec4Color); }

EParamType getBaseType(EParamType t)
{
    switch(t)
    {
    case EPFloat:
    case EPFloatRange:
    case EPFloatTime:
        return EPFloat;
    case EPInt:
    case EPTexture:
        return EPInt;
    case EPVec2:
        return EPVec2;
    case EPVec3:
    case EPVec3Color:
        return EPVec3;
    case EPVec4:
    case EPVec4Color:
        return EPVec4;
    }
    return EPFloat;
}

ModelDocument::ModelDocument(Document* doc)
    : m_errAct(NULL), m_frameObj(NULL), m_obj(NULL), m_conf(doc->m_conf)
{
    m_kparser.addFunction("wholeScreenQuad");
    m_kparser.addFunction("torus");
    m_kparser.addFunction("arrow");
    m_kparser.addFunction("sphereQuads");
    m_kparser.addFunction("sphereTri");
    m_kparser.addFunction("curveLine");
    m_kparser.addFunction("curveRotate");
    m_kparser.addFunction("plot");
    m_kparser.addFunction("axis");

}


#define EDIT_CONF_FILE ("conf.edit.xml")
#define PROG_CONF_FILE ("conf.programs.xml")


Document::Document(KawaiiGL* mainc)
    :m_nPoly(0), m_nPoints(0)
    ,m_conf(mainc->sett().disp), m_main(mainc)
    //,m_inputUnit(-1), m_outputUnit(-1),
    ,m_confxmls(mainc, EDIT_CONF_FILE, PROG_CONF_FILE) // inits the menus
    ,m_shaderEnabled(false)
    ,m_inCalc(false)
{
    connect(&m_conf.addFace, SIGNAL(changed()), this, SLOT(setAddTrack()));
    connect(&m_conf.materialCol, SIGNAL(changed()), this, SLOT(calcNoParse())); 

    connect(&m_confxmls, SIGNAL(readProg(ProgKeep*)), this, SLOT(readProg(ProgKeep*)));
    connect(&m_confxmls, SIGNAL(readModel(const QString&, const ModelData&)), this, SLOT(readModel(const QString&, const ModelData&)));

    m_currentModel.reset(new ModelDocument(this));

    // default empty program
    RenderPassPtr p(new RenderPass("Pass 1"));
    p->model = shared_ptr<DocSrc>(new DocSrc("", "Model", SRC_MODEL));
    p->model->modelDoc = m_currentModel;
    p->shaders.append(shared_ptr<DocSrc>(new DocSrc("", "Vertex Shader", SRC_VTX)));
    p->shaders.append(shared_ptr<DocSrc>(new DocSrc("", "Fragment Shader", SRC_FRAG)));
    addPass(p);
}


KwSettings& Document::sett() 
{ 
    return m_main->sett();
}


void Document::readToString(const QString& filename, QString& into)
{
    if (filename.isEmpty())
        return;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        printf("can't open %s\n", filename.toLatin1().data());
        into = "can't open " + filename + "\n";
    }
    file.setTextModeEnabled(true);
    QTextStream in(&file);
    into = in.readAll();
}

void Document::writeToFile(const QString& text, const QString filename)
{
    if (filename.isEmpty())
        return;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        printf("can't open %s\n", filename.toLatin1().data());
    }
    file.setTextModeEnabled(true);
    QTextStream out(&file);
    out << text;
}

// find the pass where the model should go
RenderPassPtr Document::passForModel()
{
    RenderPassPtr rpass;
    for (auto it = m_passes.begin(); it != m_passes.end(); ++it)
    {
        rpass = dynamic_pointer_cast<RenderPass>(*it);
        if (rpass && rpass->conf.what == PassConf::Model)
            break;
    }
    return rpass;
}

void Document::loadProgramFile() {
    QString filename = QFileDialog::getOpenFileName(m_main, "Load Program", m_conf.lastDir, "program files (*.xml)");
    if (filename.isEmpty())
        return;
    ProgKeep prog;
    m_confxmls.parseProgFile(filename, &prog);
    readProg(&prog);
}

// from config
void Document::readProg(ProgKeep* prog)
{
    // remove everything except the model of the first pass
    DocSrcPtr model;
    
    for (auto it = m_passes.begin(); it != m_passes.end(); ++it) // need to find the model
    {
        RenderPassPtr rpass = dynamic_pointer_cast<RenderPass>(*it);
        if (rpass && rpass->model)
        {
            model = rpass->model;
            break;
        }
    }

    emit goingToClearProg();
    clearPasses(); // removes them from the edit window as well

    foreach(const ProgKeep::PassKeep* pk, prog->m_passes)
    {
        const ProgKeep::RenderPassKeep *rpk = dynamic_cast<const ProgKeep::RenderPassKeep*>(pk);
        if (rpk == NULL)
        {
            const ProgKeep::SwapPassKeep *spk = dynamic_cast<const ProgKeep::SwapPassKeep*>(pk);
            if (spk != NULL)
            {
                SwapOpPassPtr spass(new SwapOpPass(spk->name));
                spass->a = (PassConf::ERenderTo)spk->a;
                spass->b = (PassConf::ERenderTo)spk->b;
                addPass(spass);
            }
            continue;
        }

        RenderPassPtr p(new RenderPass(rpk->name));
        foreach(const ProgKeep::SrcKeep& sk, rpk->shaders)
        {
            p->shaders.append(shared_ptr<DocSrc>(new DocSrc(sk.text, sk.name, sk.type)));
        }
        
        p->params = rpk->params; // copy the ParamInput's of the loaded xml.
        for(int i = 0; i < p->params.size(); ++i)
        {
            ParamInput& pi = p->params[i];
            pi.mypass = p.get();
        }

        p->conf.copyFrom(rpk->conf);
        
        addPass(p);
    }

    RenderPassPtr rpass = passForModel();
    if (rpass)
        rpass->model = model;

    // set config params
    for(auto it = prog->args.begin(); it != prog->args.end(); ++it)
    {
        Prop* prop = m_conf.propByName(it.key());
        prop->fromString(*it);
    }

    emit didReadProg(prog);
}


void Document::addNewShader(RenderPass* pass, ElementType type)
{
    DocSrc *d = new DocSrc("", "Doc", type);
    pass->shaders.append(shared_ptr<DocSrc>(d));
}

void Document::loadShaderFile(RenderPass* pass, const QString& filename, ElementType type)
{
    DocSrc *d = new DocSrc(filename, type);
    readToString(d->name(), d->text);
    pass->shaders.append(shared_ptr<DocSrc>(d));
}


void Document::removeShader(RenderPass* pass, DocSrc* src)
{
    // The list contains shared_ptrs, we have a pointer, need to look for it manually.
    auto &shaders = pass->shaders;
    auto it = shaders.begin();
    while(it != shaders.end())
    {
        if (it->get() == src)
        {
            it = shaders.erase(it);
            return;
        }
        else
            ++it;
    }
}

void Document::addNewPass()
{
    int i = m_passes.size() + 1;
    RenderPassPtr p(new RenderPass(QString("Pass %1").arg(i) ));
    p->shaders.append(shared_ptr<DocSrc>(new DocSrc("// Vertex Shader", QString("Vertex Shader %1").arg(i), SRC_VTX)));
//	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Geometry Shader", false, SRC_GEOM)));
    p->shaders.append(shared_ptr<DocSrc>(new DocSrc("// Fragment Shader", QString("Fragment Shader %1").arg(i), SRC_FRAG)));
    addPass(p);
}

void Document::addNewSwap()
{
    SwapOpPassPtr p(new SwapOpPass("Swap"));
    addPass(p);
}

void Document::movePass(Pass* pass, int delta)
{
    PassPtr passptr; // look for the PassPtr of the pass
    auto it = m_passes.begin();
    while (it != m_passes.end())
    {
        if (it->get() == pass)
        {
            passptr = *it;
            it = m_passes.erase(it);
            break;
        }
        else
            ++it;
    }

    if (delta != 0)
    {
        it += delta;
        m_passes.insert(it, passptr);
    } // delta = 0 is remove
}

void Document::removePass(Pass* pass)
{
    movePass(pass, 0);
}


struct GatherPoints : public PointActor
{
    vector<Vec3> pnt;
    virtual void operator()(const std::string& name, const Vec3& p, bool used, IPoint* handle)
    {
        int i = QString(name.c_str()).remove(0,1).toInt();
        pnt.resize(qMax(i+1, (int)pnt.size()));
        pnt[i] = p;
    }
};


// some custom process the I write in C++
// for now - mobius model processing
QString Document::dedicatedProcess()
{
    GatherPoints orig;
    model()->m_kparser.creator()->foreachPoints(orig);

    QString prog;
    prog += "d=1\nr=1\n";
    int mi = 0, pi = 0;
    for(size_t i = 0; i < orig.pnt.size(); i += 4)
    {
        Vec3 &p0 = orig.pnt[i], &p1 = orig.pnt[i+1], &p2 = orig.pnt[i+2], &p3 = orig.pnt[i+3];
        Vec3 m = (p0+p1+p2+p3)/4.0;
        prog += QString().sprintf("m%d={%1.3f, %1.3f, %1.3f}*d\n", mi, m.x, m.y, m.z); 
        p0 -= m;
        p1 -= m;
        p2 -= m;
        p3 -= m;
        prog += QString().sprintf("p%d=m%d+{%1.3f, %1.3f, %1.3f}*r\n", pi++, mi, p0.x, p0.y, p0.z);
        prog += QString().sprintf("p%d=m%d+{%1.3f, %1.3f, %1.3f}*r\n", pi++, mi, p1.x, p1.y, p1.z);
        prog += QString().sprintf("p%d=m%d+{%1.3f, %1.3f, %1.3f}*r\n", pi++, mi, p2.x, p2.y, p2.z);
        prog += QString().sprintf("p%d=m%d+{%1.3f, %1.3f, %1.3f}*r\n\n", pi++, mi, p3.x, p3.y, p3.z);
        mi++;
    }
    return prog;
}

void Document::readModel(const QString& name, const ModelData& md)
{
    RenderPassPtr pass = passForModel();
    if (!pass)
        return;

    QString text; 

    if (name == "Dedicated processing")
    {
        text = dedicatedProcess();
        pass->model.reset(new DocSrc(text, "Generated", SRC_MODEL));
    }
    else if (name == "Generate from File")
    {
        text = generateFromFile();
        pass->model.reset(new DocSrc(text, "Generated", SRC_MODEL));
    }
    else if (name == "Load File")
    {
        QString filename = QFileDialog::getOpenFileName(m_main, "Load Model", m_conf.lastDir, "model files (*.obj *.off *.ply2 *.gsd *.json)");
        if (filename.isEmpty())
            return;
        m_conf.lastDir = QFileInfo(filename).absolutePath();
        filename = QDir::current().relativeFilePath(filename);

        text = "load(\"" + filename + "\")";
        pass->model.reset(new DocSrc(text, "Loaded Mesh", SRC_MODEL));
    }
    else if (name == "clear")
    {
        text = "";
        pass->model.reset(new DocSrc(text, "Model", SRC_MODEL));
    }
    else
    {
        //ModelData md = send->data().value<ModelData>();
        if (!md.isMesh) 
        {
            Document::readToString(md.filename, text);
            pass->model.reset(new DocSrc(text, md.filename, SRC_MODEL));
        }
        else
        {
            text = "load(" + md.filename + ")";
            pass->model.reset(new DocSrc(text, "Loaded Mesh", SRC_MODEL));
        }
    }
    
    emit didReadModel(pass->model.get());

    calc(pass->model.get());
}

QString Document::generateFromFile()
{
    QString filename = QFileDialog::getOpenFileName(m_main, "Load Obj", m_conf.lastDir, "points files (*.obj)");
    if (filename.isEmpty())
        return "";
    m_conf.lastDir = QFileInfo(filename).absolutePath();

    bool useMeshIO = false;
    if (!MyInputDlg::getValue("use MeshIO (triagulate)", useMeshIO, "bla", m_main))
        return "";

    QString prog;
    if (useMeshIO)
    {
        Mesh mesh(filename);
        if (!MeshIO::read_Obj(filename, &mesh))
            return "// Error in file";
        mesh.finalize(false);

        mesh.rescaleAndCenter(2.0f);
        for(int i = 0; i < mesh.numVtx(); ++i)
        {
            Mesh::Vertex_const_handle vh = mesh.find_vertex(i);
            Vec3 p = vh->point();

            prog += QString().sprintf("p%d={%1.3f, %1.3f, %1.3f}\n", i, p.x, p.y, p.z);
        }
        for(int i = 0; i < mesh.numFaces(); ++i)
        {
            Mesh::Face_const_handle fh = mesh.find_facet(i);

            prog += QString().sprintf("add(p%d,p%d,p%d)\n", fh->vertexIndex(0), fh->vertexIndex(1), fh->vertexIndex(2));
        }
    }
    else
    {
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
    }

    return prog;
}





QIcon Document::getTypeIcon(ElementType t)
{
    switch (t)
    {
    case SRC_VTX: return QIcon(":/images/doc_vtx.png");
    case SRC_GEOM: return QIcon(":/images/doc_geom.png");
    case SRC_FRAG: return QIcon(":/images/doc_frag.png");
    case SRC_MODEL: return QIcon(":/images/doc_model.png");
    case SRC_RENDER: return QIcon(":/images/gear.png");
    case SRC_OP_SWAP: return QIcon(":/images/arrow-switch-270.png");
    default: return QIcon();
    }
}

QString Document::getTypeName(ElementType t)
{
    switch (t)
    {
    case SRC_VTX: return QString("Vertex");
    case SRC_GEOM: return QString("Geometry");
    case SRC_FRAG: return QString("Fragment");
    case SRC_MODEL: return QString("Model");
    default: return QString();
    }
}


void Document::setAddTrack()
{
    model()->m_addTrack.reset();
}

void Document::calcNoParse() 
{ 
    calc(NULL, false); 
}


#define FILTER_OBJ "object files (*.obj)"
#define FILTER_JSON_TRI "JSON triangles (*.json)"
#define FILTER_JSON_QUADS "JSON quads (*.json)"
#define FILTER_JSON_LINES "JSON lines (*.json)"

void Document::calcSave()
{
    QString selectedFilter;
    QString filename = QFileDialog::getSaveFileName(m_main, "Save Object", sett().gui.saveDir, 
        FILTER_OBJ ";;" FILTER_JSON_TRI ";;" FILTER_JSON_QUADS ";;" FILTER_JSON_LINES, &selectedFilter);
    if (filename.isEmpty())
        return;

    sett().gui.saveDir = QFileInfo(filename).path();

    ModelDocument* mdoc = model();

    if (!mdoc->m_obj)
        calc(NULL, false); // can this happen?

    
    QFile file(filename);
    if (!file.open(QFile::WriteOnly))
    {
        printf("unable to open file %s\n", filename.toLatin1().data());
        return;
    }

    file.setTextModeEnabled(true);
    QTextStream out(&file);

    if (selectedFilter == FILTER_OBJ)
    {
        mdoc->m_obj->saveAs(out, "obj");
    }
    else if (selectedFilter == FILTER_JSON_TRI)
    {
        mdoc->m_obj->saveAs(out, "json", MyObject::SaveTriangles);
        foreach(const shared_ptr<Mesh>& mesh, mdoc->m_meshs)
        {
            JsonWriter writer(mesh.get());
            writer.write(out);
        }
    }
    else if (selectedFilter == FILTER_JSON_QUADS)
    {
        mdoc->m_obj->saveAs(out, "json", MyObject::SaveQuads);
    }
    else if (selectedFilter == FILTER_JSON_LINES)
    {
        mdoc->m_obj->saveAs(out, "json", MyObject::SaveEdges);	
    }
    else
    {
        printf("unknown filter\n");
    }


}


MyAllocator g_alloc(50000, 50000, 50000);


struct MyObjAdder : public PolyAdder
{
    MyObjAdder(MyObject* obj, DisplayConf& conf, bool purgeCache) : m_obj(obj), m_defColor(conf.materialCol), m_purgePointCache(purgeCache)
    {}
    virtual void operator()(vector<IPoint*>& v)
    {
        if (v.size() < 3 || v.size() > 4)
            return;
        if (m_purgePointCache) {
            for(int i = 0; i < v.size(); ++i)
                v[i]->purgeCache();
        }
        MyPolygon *poly = m_obj->AddPoly(v[0], v[1], v[2], (v.size() > 3)?(v[3]):NULL, constAncs);

        for(size_t i = 0; i < v.size(); ++i)
        {
            Vec3 gc = v[i]->getColor();
            if (!gc.isValidColor())
                gc = m_defColor;
            poly->vtx[i]->col = gc;
        }
    }

    Vec3 m_defColor; // default color;
    MyObject* m_obj;
    bool m_purgePointCache;
    static TexAnchor constAncs[4];
};

TexAnchor MyObjAdder::constAncs[4] = { TexAnchor(0,0), TexAnchor(1,0), TexAnchor(1,1), TexAnchor(0,1) };


struct MeshAdder : public StringAdder
{
    MeshAdder(ModelDocument* doc) : m_doc(doc), m_index(0) {}
    virtual void operator()(const string& s)
    {
        QString filename = QString(s.c_str()).toLower();
        auto it = m_doc->m_meshIndex.find(filename);
        shared_ptr<Mesh> mesh;
        /*if (it != m_doc->m_meshIndex.end())
        {  // TBD: compare date as well
            mesh = it.value();
        }
        else*/
        {
            mesh.reset(new Mesh(filename.toLatin1().data()));
            IPoint *p = m_doc->m_kparser.creator()->lookupSymbol(QString("defcol%1").arg(m_index).toLatin1().data());
            if (p != NULL)
                mesh->setDefaultMtl(Mesh::Material("", p->getCoord()));
            
            if (MeshIO::read_Ext(filename, mesh.get()))
            {
                mesh->finalize(false);
                printf("  center=%s  boxSize=%s\n", mesh->minMaxCenter().toStringF().toLatin1().data(), mesh->diagLength().toStringF().toLatin1().data());

                //mesh->translateCenter();
                mesh->rescaleAndCenter(6.0f);
                printf("  center=%s  boxSize=%s\n", mesh->minMaxCenter().toStringF().toLatin1().data(), mesh->diagLength().toStringF().toLatin1().data());
            }
            m_doc->m_meshIndex[filename] = mesh;
        }

        m_doc->m_meshs.append(mesh);
        ++m_index;
    }
    ModelDocument *m_doc;
    int m_index;
};

struct FuncAdder : public MultiStringAdder
{
    FuncAdder(ModelDocument* doc) : m_doc(doc) {}
    virtual void operator()(const vector<string>& sa)
    {
        vector<string> args;
        if (sa.size() >= 2) {
            QString sargs = sa[1].c_str();
            QStringList sep = sargs.split(',', QString::SkipEmptyParts);
            foreach(const QString& s, sep)
                args.push_back(s.trimmed().toLatin1().data());
        }
        if (sa[0] == "wholeScreenQuad")
            m_doc->m_rends.append(shared_ptr<Renderable>(new WholeScreenQuad()));
        else if (sa[0] == "arrow") {
            shared_ptr<ArrowRenderable> p(new ArrowRenderable());
            if (p->init(args, m_doc))
                m_doc->m_rends.append(shared_ptr<Renderable>(p));
        }
        else if (sa[0] == "torus")
            m_doc->generateTorus(args);
        else if (sa[0] == "sphereQuads")
            m_doc->generateQuadSphere(args);
        else if (sa[0] == "sphereTri")
            m_doc->generateTriSphere(args);
        else if (sa[0] == "curveLine")
            m_doc->generateCurve(args);
        else if (sa[0] == "curveRotate")
            m_doc->generateRotObj(args);
        else if (sa[0] == "plot")
            m_doc->generateIsoSurface(args);
        else if (sa[0] == "axis") {
            float sz = 1.0; 
            if (args.size() == 1)
                sz = QString(args[0].c_str()).toFloat();
            shared_ptr<ArrowRenderable> xa(new ArrowRenderable());
            xa->init(Vec3(0,0,0), Vec3(sz,0,0), Vec3(1,0,0));
            m_doc->m_rends.append(shared_ptr<Renderable>(xa));

            shared_ptr<ArrowRenderable> ya(new ArrowRenderable());
            ya->init(Vec3(0,0,0), Vec3(0,sz,0), Vec3(0,1,0));
            m_doc->m_rends.append(shared_ptr<Renderable>(ya));

            shared_ptr<ArrowRenderable> za(new ArrowRenderable());
            za->init(Vec3(0,0,0), Vec3(0,0,sz), Vec3(0,0,1));
            m_doc->m_rends.append(shared_ptr<Renderable>(za));
        }
    }
    ModelDocument *m_doc;
};



void Document::calc(DocSrc* src, bool doParse, bool purgePointCache)
{
    // prevent recursive calls which occur due to syntax highlighing
    if (m_inCalc)
        return;
    m_inCalc = true; 

    g_alloc.clear();

    ModelDocument* mdoc = model();

    mdoc->m_frameObj.reset(new MyObject(&g_alloc));

    mdoc->m_rends.clear();

    if (doParse)
    {
        mdoc->m_meshs.clear();
        mdoc->m_addTrack.reset(); // don't want to continue to point to smething that's going to disappear

        QByteArray ba;
        if (src != NULL) {
            ba = src->text.toLatin1();
        } // otherwise it's empty

        const char* iter = ba.data();
        const char* end = ba.data() + ba.size();
        mdoc->m_errAct->clear(src);
        mdoc->m_kparser.kparse(iter, end, false, mdoc->m_errAct);
        mdoc->m_errAct->finish();

        mdoc->m_kparser.creator()->addMeshes(&MeshAdder(mdoc));

        emit loaded();
    }
    else
    {
        if (!mdoc->m_kparser.isValid())
            return;
    }

    // this is here because the torus is not part of the parsed record so it needs to be recreated
    mdoc->m_kparser.creator()->callFuncs(&FuncAdder(mdoc));

    //m_kparser.creator()->printTree();
    

    MyObjAdder adder(mdoc->m_frameObj.get(), m_conf, purgePointCache);
    mdoc->m_kparser.creator()->createPolygons(&adder);
    //updateParams(m_onCalcEvals); // if there's a prog active, it might depend on the variables.

    //	currentDecompile.clear();


    //	emitDecompile(); 
    //pntDlg->updatePoint();
    //	if (bSymLoad)
    //		updateSelPointColor();// take the color from the text

    mdoc->m_frameObj->vectorify();
    mdoc->m_frameObj->clacNormals(m_conf.bVtxNormals);

    if (m_conf.numberOfPasses > 0) 
    {
        mdoc->m_obj.reset(new MyObject(*mdoc->m_frameObj)); // copy it
        mdoc->m_obj->detachPoints();

        for (int i = 0; i < m_conf.numberOfPasses; ++i)
        {
            if (!mdoc->m_obj->subdivide(*m_conf.passRound[i]))
                break; // stop it
        }
        mdoc->m_obj->clacNormals(m_conf.bVtxNormals);
    }
    else {
        mdoc->m_obj = mdoc->m_frameObj;
    }
    m_nPoly = mdoc->m_obj->nPolys;
    m_nPoints = mdoc->m_obj->nPoints;



    g_alloc.checkMaxAlloc(); // pool maintainance

    emit modelChanged();
    m_inCalc = false;
}


void Document::updateTrack(IPoint* sel)
{
    if (!m_conf.addFace)
        return;

    auto* mdoc = model();

    if (mdoc->m_addTrack.add(sel))
    {
        int size = mdoc->m_addTrack.m_added.size();
        if (size == 3 || size == 4)
        {
            QString line;
            line += "add(";
            line += mdoc->m_addTrack.m_added[0]->getName().c_str();
            for(int i = 1; i < mdoc->m_addTrack.m_added.size(); ++i)
            {
                IPoint *p = mdoc->m_addTrack.m_added[i];
                line += ", ";
                line += p->getName().c_str();
            }
            line += ")\n";

            mdoc->m_addTrack.reset();
            emit addModelLine(line);
        }

        mdoc->m_addTrack.reset();
        
    }

}

void Document::addPass(PassPtr pass)
{
    RenderPassPtr rpass = dynamic_pointer_cast<RenderPass>(pass);
    if (rpass)
        connect(&rpass->conf, SIGNAL(changed()), this, SIGNAL(progParamChanged()));
    m_passes.append(pass);
}

void Document::clearPasses()
{
    m_passes.clear();
}



void Document::compileShaders()
{
    if (!model()->m_kparser.isValid())
    { // we did not parse any model yet so the parser is invalid. wake it up.
        calc(NULL);
    }


    //m_onCalcEvals.clear();
    m_onFrameEvals.clear();


    foreach(const PassPtr &pass, m_passes)
    {
        RenderPassPtr rpass = dynamic_pointer_cast<RenderPass>(pass);
        if (!rpass)
            continue;
        rpass->prog.clear();

        if (!m_shaderEnabled)
            continue;

        foreach(const shared_ptr<DocSrc>& src, rpass->shaders)
        {
            if (src->text.trimmed().isEmpty())
                continue;
            if (src->type == SRC_VTX)
                rpass->prog.vtxprogs() += src->text;
            else if (src->type == SRC_FRAG)
                rpass->prog.fragprogs() += src->text;
            else if (src->type == SRC_GEOM)
                rpass->prog.geomprogs() += src->text;
        }

        // if it's empty, do nothing.
        if (rpass->prog.vtxprogs().size() + rpass->prog.fragprogs().size() + rpass->prog.geomprogs().size() == 0)
            continue;

        //m_conf.runType = pass->conf.runType.val();

        //m_onCalcEvals.resize(pass->params.size());
        //m_onFrameEvals.resize(pass->params.size());
        m_attribEval.clear();

        for(int i = 0; i < rpass->params.size(); ++i)
        {
            const ParamInput &pi = rpass->params[i];
            ShaderParam *sp = NULL;
            if (pi.dest == EDUniform)
                sp = new UniformParam(pi.name);
            else if (pi.dest == EDAttribute)
                sp = new AttribParam(pi.name);
            else
                continue;
            rpass->prog.addParam(sp, i);
            pi.index = i;
        }

        ProgCompileConf conf(rpass->conf.geomInput, rpass->conf.geomOutput, rpass->conf.geomVtxCount);
        if (rpass->prog.init(conf))
        {
            // need to take care of the variables background colors.
            parseAllParams(rpass);
        }
    }

    emit progChanged();
}


void Document::parseAllParams(const RenderPassPtr &pass)
{
    {
        ProgramUser use(&pass->prog);
        for(int i = 0; i < pass->params.size(); ++i)
        {
            const ParamInput &pi = pass->params[i];
            parseParam(pi);
        }
    }

    mglCheckErrorsC("parseAllParam");

}


bool Document::parseSingleParam(const ParamInput& pi, bool update)
{
    bool ok = false;
    if (pi.mypass != NULL) 
    {
        if (!pi.mypass->prog.isOk())
            return false;
        {
            ProgramUser use(&pi.mypass->prog);
            ok = parseParam(pi);
        }
        mglCheckErrorsC("parseSingleParam");
    }
    else {
        // a model parameter
        ok = parseParam(pi);
    }

    if (update)
        emit progParamChanged(); // causes updateGL
    return ok;
}

QString uniqueParamId(const ParamInput& pi)
{
    return pi.mypass->name() + QString("_%1").arg(pi.index);
}

// parse uniform. this defines what a uniform variable can be.
bool Document::parseParam(const ParamInput& pi)
{
    if (pi.dest == EDAttribute)
    {
        pi.lastParseOk = parseAttrib(pi);
        return pi.lastParseOk;
    }
    
    QString value = pi.value;
    // handle special variables.
    QSize glRect = m_main->view()->clientRect();
    value.replace("$(screenHeight)", QString("%1").arg(glRect.height()));
    value.replace("$(screenWidth)", QString("%1").arg(glRect.width()));


    QByteArray ba = (pi.name + "=" + value).toLatin1();
    const char* iter = ba.data();
    const char* end = ba.data() + ba.size();
    const char* nameend = ba.data() + pi.name.size();

    GenericShader *prog = NULL;
    if (pi.mypass != NULL)
        prog = &pi.mypass->prog;


    ModelDocument* mdoc = model();
    // TBD: iterate all models
    bool ok = true;
    switch(pi.type) // ADDTYPE
    {
    case EPFloat: 
    case EPFloatRange:
    case EPFloatTime:
        {
            float f = 0.0f;
            if (ok = mdoc->m_kparser.kparseFloat(iter, end, nameend, f))
            {
                if (prog)
                    prog->setUniform(f, pi.index);
            }
        }
        break;
    case EPInt:
        {
            float f = 0.0f;
            if (ok = mdoc->m_kparser.kparseFloat(iter, end, nameend, f))
            {
                int i = (int)f;
                if (prog)
                    prog->setUniform(i, pi.index);
            }
        }
        break;
    case EPVec2:
        {
            Vec2 v;
            if (ok = mdoc->m_kparser.kparseVec2(nameend + 1, end, v))
            {
                if (prog)
                    prog->setUniform(v, pi.index);
            }
        }
        break;
    case EPVec3:
    case EPVec3Color:
        {
            IPoint *v = NULL;
            if (ok = mdoc->m_kparser.kparseVec(iter, end, nameend, v))
            {
                shared_ptr<ParamAdapter> pa(new VecParamAdapter(v, pi.prop, pi.index));
                //m_onCalcEvals[uniqueParamId(pi)] = pa;
                if (prog)
                    pa->update(*prog);
            }
        }
        break;
    case EPVec4:
    case EPVec4Color:
        {
            Vec4 v;
            if (ok = mdoc->m_kparser.kparseVec4(nameend + 1, end, v))
            {
                if (prog)
                    prog->setUniform(v, pi.index);
                if (pi.prop != NULL)
                {
                    pi.prop->tryAssignTypeVal(v.toColor());
                }
            }
        }
        break;
    case EPTexture:
        {
            int ti = pi.value.toInt(&ok);
            if (ok)
            {
                if (prog)
                    prog->setUniform(ti, pi.index);
                m_onFrameEvals[uniqueParamId(pi)].reset();
            }
/*
            else
            {
                QString v = pi.value.trimmed().toLower();
                if (v.startsWith("input"))
                {
                    shared_ptr<ParamAdapter> pa(new TexParamAdapter(this, pi.index));
                    m_onFrameEvals[uniqueParamId(pi)] = pa;

                    pa->update(prog);
                    ok = true;
                }
            }*/

        }
        break;
    }

    pi.lastParseOk = ok;
    return ok;
}


void Document::VecParamAdapter::update(GenericShader &prog) const
{
    if (pe == NULL)
        return;
    Vec3 ev = pe->getCoord();
    prog.setUniform(ev, index);

    if (toprop != NULL)
    {
        toprop->tryAssignTypeVal(ev.toColor());
    }
}

void Document::TexParamAdapter::update(GenericShader &prog) const
{
/*
    int tex = m_doc->m_inputUnit;
    if (tex == -1)
        return;
    prog.setUniform(tex, index);*/
}


void Document::updateParams(TAdaptersList &adapters)
{
    mglCheckErrorsC("startUpdateParams");
    {
        //throw "blat";
        //ProgramUser use(&m_prog);
        //foreach(const shared_ptr<ParamAdapter>& pa, adapters)
        {
        //	if (pa.get() != NULL)
        //		pa->update(m_prog); // there are only Vec3 that need updates at the moment.
        }
    }

    mglCheckErrorsC("updateParams");
}



struct TangentAttribEval : public Document::AttribEval
{
    virtual void setAttribVal(const MyPolygon* poly, int vi)
    {
        //m_prog->setAttrib(poly->tangent, index);
        m_prog->setAttrib(poly->vtx[vi]->tangent, index);
    }
    virtual bool initWithMesh(const Mesh *rmesh)
    {
        m_enabled = rmesh->hasVtxProp(Prop_Tangent);
        return m_enabled;
    }
    virtual void setAttribVal(Mesh::Vertex_const_handle vh)
    {
        if (!m_enabled)
            return;
        m_prog->setAttrib(vh->prop<Vec3>(Prop_Tangent), index);
    }
    bool m_enabled;
};
struct BiTangentAttribEval : public Document::AttribEval
{
    virtual void setAttribVal(const MyPolygon* poly, int vi)
    {
        //m_prog->setAttrib(poly->bitangent, index);
        m_prog->setAttrib(poly->vtx[vi]->bitangent, index);
    }
    virtual bool initWithMesh(const Mesh *rmesh)
    {
        m_enabled = rmesh->hasVtxProp(Prop_BiTangent);
        return m_enabled;
    }
    virtual void setAttribVal(Mesh::Vertex_const_handle vh)
    {
        if (!m_enabled)
            return;
        m_prog->setAttrib(vh->prop<Vec3>(Prop_BiTangent), index);
    }

    bool m_enabled;
};

// parse attribute text. this defines what an attribute can be.
bool Document::parseAttrib(const ParamInput &pi)
{
    QString func = pi.value.trimmed().toLower();

    AttribEval *atte = NULL;
    if (func.startsWith("tangent"))
        atte = new TangentAttribEval();
    else if (func.startsWith("bitangent"))
        atte = new BiTangentAttribEval();
    else 
        return false;

    atte->setEnv(&pi.mypass->prog, pi.index);
    m_attribEval.append(shared_ptr<AttribEval>(atte));
    return true;
}

void Document::initAttribMesh(const Mesh* rmesh)
{
    for(int i = 0; i < m_attribEval.size(); ++i)
    {
        m_attribEval[i]->initWithMesh(rmesh);
    }
}


void Document::setAttribs(MyPolygon* poly, int vi) // set values to attributes
{
    for(int i = 0; i < m_attribEval.size(); ++i)
    {
        m_attribEval[i]->setAttribVal(poly, vi);
    }
}

void Document::setAttribs(Mesh::Vertex_const_handle vh)
{
    for(int i = 0; i < m_attribEval.size(); ++i)
    {
        m_attribEval[i]->setAttribVal(vh);
    }
}
