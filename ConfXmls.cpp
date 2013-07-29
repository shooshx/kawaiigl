#include "ConfXmls.h"

#include <QDomDocument>
#include <QMenu>
#include <QFile>

#include "KwEdit.h"
#include "DisplayConf.h"
#include "KawaiiGL.h"
#include "Pass.h"
#include "Document.h"


ConfXmls::ConfXmls(KawaiiGL* mainc, const QString& modelsFile, const QString& progFiles) 
    :m_conf(mainc->sett().disp), m_parent(mainc)
{
    loadModelsFile(modelsFile, mainc->m_modelsMenu);
    loadProgramsFile(progFiles, mainc->m_progMenu);
}


void ConfXmls::loadModelsFile(const QString& filename, QMenu* modelsMenu)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        printf("could not open examples file %s\n", filename.toLatin1().data());
        return;
    }
    QDomDocument doc("conf");
    if (!doc.setContent(&file))
    {
        file.close();
        printf("could not parse XML file %s\n", filename.toLatin1().data());
        return;
    }
    QDomElement de = doc.documentElement();
    QDomNode n = de.firstChild();
    if (n.isNull())
        return;
    QDomElement se = n.toElement();
    if (se.isNull())
        return;
    if (se.tagName() != "shapes_edit")
        return;

    // clear actions... TBD

    for (QDomNode fn = se.firstChild(); !fn.isNull(); fn = fn.nextSibling())
    {
        QDomElement fe = fn.toElement();
        if (fe.isNull())
            continue;
        if (fe.tagName() != "file")
            continue;
        QDomAttr disa = fe.attributeNode("display");
        if (disa.isNull())
            continue;
        bool isMesh;
        QDomAttr typea = fe.attributeNode("type");
        if (!disa.isNull())
        {
            isMesh = typea.value() == "mesh";
        }

        QString display = disa.value();
        QString filename = fe.text();

        modelsMenu->addAction(confAddModel(display, filename, isMesh));
    }

    // other actions
    QAction *act;
    modelsMenu->addAction(act = new QAction("", m_parent));
    act->setSeparator(true);
    modelsMenu->addAction(act = new QAction("clear", m_parent));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));
    modelsMenu->addAction(act = new QAction("Generate from File", m_parent));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));
    modelsMenu->addAction(act = new QAction("Load File", m_parent));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));

    modelsMenu->addAction(act = new QAction("Dedicated processing", m_parent));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));

}



ProgKeep::SrcKeep makeSrc(const QDomElement& e, ElementType type) {
    QString t;
    QString name;
    if (e.hasAttribute("name")) {// body is the shader text
        name = e.attribute("name");
        t = e.text();
        if (t.size() > 0 && t[0] == '\n')
            t.remove(0, 1);
        if (t.size() > 3 && t.right(2) == "\n\n")
            t.remove(t.size() - 2, 1);
    }
    else { // body is the filename
        name = e.text();
        Document::readToString(name, t);
    }
    return ProgKeep::SrcKeep(name, t, type);
}

void ConfXmls::loadPassElement(ProgKeep::RenderPassKeep* pass, QDomElement &pe)
{
    for (QDomNode n = pe.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement e = n.toElement();
        if (e.isNull())
            continue;
        if (e.tagName() == "vtxtext")
            pass->shaders.append(makeSrc(e, SRC_VTX)); // contains the filenames
        else if (e.tagName() == "fragtext")
            pass->shaders.append(makeSrc(e, SRC_FRAG));
        else if (e.tagName() == "geomtext")
            pass->shaders.append(makeSrc(e, SRC_GEOM));
        else if (e.tagName() == "param")
        {
            QDomAttr namea = e.attributeNode("name");
            if (namea.isNull())
                continue;
            QDomAttr typea = e.attributeNode("type");
            if (typea.isNull())
                continue;
            QString type = typea.value();
            EParamType etype;
            if (type == "float") // ADDTYPE
                etype = EPFloat;
            else if (type == "float01" || type == "floatRange")
                etype = EPFloatRange;
            else if (type == "floatTime")
                etype = EPFloatTime;
            else if (type == "int")
                etype = EPInt;
            else if (type == "vec2")
                etype = EPVec2;
            else if (type == "vec3")
                etype = EPVec3;
            else if (type == "vec4")
                etype = EPVec3;
            else if (type == "vec3color")
                etype = EPVec3Color;
            else if (type == "vec4color")
                etype = EPVec4Color;
            else if (type == "texture")
                etype = EPTexture;
            else 
            {
                printf("unknown param type %s\n", type.toLatin1().data());
                continue;
            }

            bool isUniform = true;
            QDomAttr kinda = e.attributeNode("kind");
            if (!kinda.isNull())
            {
                if (kinda.value() != "uniform")
                    isUniform = false;
            }

            ParamInput pi(namea.value(), etype, e.text(), isUniform, NULL);

            QDomAttr guia = e.attributeNode("gui");
            if (!guia.isNull())
            {
                QString guival = guia.value().toLower().trimmed();
                if (etype == EPVec2)
                {
                    Vec2GuiConf *g = new Vec2GuiConf;
                    if (guival == "screen")  // range is the screen size
                        g->relative = false;
                    else if (guival == "relative") // menas with range [0,1,0,1]
                        g->relative = true;
                    else // explicit range
                    {
                        QStringList mm = guival.split(",", QString::SkipEmptyParts);
                        if (mm.size() == 4)
                        {
                            g->xmin = mm[0].toFloat();
                            g->xmax = mm[1].toFloat();
                            g->ymin = mm[2].toFloat();
                            g->ymax = mm[3].toFloat();
                        }
                    }
                    pi.guiconf = g;
                }
                else if (etype == EPFloatRange)
                {
                    FloatGuiConf *g = new FloatGuiConf;
                    QStringList mm = guival.split(",", QString::SkipEmptyParts);
                    if (mm.size() == 2)
                    {
                        g->vmin = mm[0].toFloat();
                        g->vmax = mm[1].toFloat();
                    }
                    pi.guiconf = g;
                }
            }


            pass->params.append(pi);

        } // param
        else if (e.tagName() == "conf")
        {
            QDomAttr namea = e.attributeNode("name");
            if (namea.isNull())
                continue;
            QString name = namea.value();
            Prop *p = pass->conf->propByName(name);
            if (p == NULL)
            {
                printf("unknown config name %s\n", name.toLatin1().data());
                continue;
            }
            QString value = e.text();
            if (!p->fromString(value))
            {
                printf("Unable to understand config `%s` value `%s`\n", name.toLatin1().data(), value.toLatin1().data());
                continue;
            }

        }
        // TBD: read model?


    } // children
}


enum RunType
{
    RunNone,
    RunNormal, // 3d Scene rendered to the screen with prog
    RunQuadProcess,  // 3d scene rendered to texture 0, texture 0 rendered to a quad with prog
    RunTex2Tex
};

void ConfXmls::loadProgramElement(const QString& display, QDomElement &pe, ProgKeep *prog)
{
    prog->name = display;

    ProgKeep::RenderPassKeep *tempPass = NULL;
    RunType tempTemplate = RunNormal;

    for (QDomNode n = pe.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement e = n.toElement();
        QString tagName = e.tagName();
        if (tagName == "pass")
        {
            ProgKeep::RenderPassKeep *pass = new ProgKeep::RenderPassKeep();

            QDomAttr namea = e.attributeNode("name");
            if (namea.isNull())
                pass->name = QString("Pass %1").arg(prog->m_passes.size() + 1);
            else
                pass->name = namea.value();

            loadPassElement(pass, e);
            prog->m_passes.append(pass);

        }
        else if (tagName == "swappass") 
        {
            int a = e.attribute("a").toInt();
            int b = e.attribute("b").toInt();
            prog->m_passes.append(new ProgKeep::SwapPassKeep("Swap", a, b));
        }
        else if (tagName == "arg")
        {
            QDomAttr namea = e.attributeNode("name");
            if (namea.isNull())
                continue;
            QString name = namea.value();
            if (m_conf.propByName(name) == NULL)
                printf("No such property %s\n", name.toLatin1().data());
            else
                prog->args[name] = e.text();
        }
        // template support
        else if (tagName == "vtxtext" || tagName == "fragtext" || tagName == "geomtext" || tagName == "param")
        {
            if (tempPass == NULL)
            {
                tempPass = new ProgKeep::RenderPassKeep();
                loadPassElement(tempPass, pe);
            }
        }
        else if (tagName == "quadproc")
        {
            int op = e.text().toInt();
            tempTemplate = ((bool)op)?RunQuadProcess:RunNormal;
        }
        else if (tagName == "runtype")
        {
            QString opt = e.text().toLower().trimmed();
            if (opt == "quadproc")
                tempTemplate = RunQuadProcess;
            else if (opt == "normal")
                tempTemplate = RunNormal;
            else if (opt == "tex2tex")
                tempTemplate = RunTex2Tex;
            else
                printf("unknown runType option %s\n", opt.toLatin1().data());
        }

    }

    if (tempPass != NULL)
    {
        if (!prog->m_passes.isEmpty())
        {
            printf("error in prog %s, can't have passes and template at the same time\n", display.toLatin1().data());
            return;
        }

        switch(tempTemplate)
        {
        case RunNormal:
            tempPass->name = "Pass 1";
            prog->m_passes.append(tempPass);
            break;
        case RunQuadProcess:
            {
                ProgKeep::RenderPassKeep *pass1 = new ProgKeep::RenderPassKeep("Pass 1");
                pass1->conf->to = PassConf::Texture0;
                prog->m_passes.append(pass1);

                tempPass->name = "Pass 2";
                tempPass->conf->what = PassConf::Quad_Tex0;
                prog->m_passes.append(tempPass);
                break;
            }
        case RunTex2Tex:
            {
                tempPass->name = "Screen Pass";
                prog->m_passes.append(tempPass);
                tempPass->conf->what = PassConf::Quad_Tex1;
                tempPass->conf->to = PassConf::Texture0;
                tempPass->conf->toMultisample = false;

                ProgKeep::RenderPassKeep *model = new ProgKeep::RenderPassKeep("Model Pass");
                model->conf->what = PassConf::Model;
                model->conf->to = PassConf::Texture0;
                model->conf->clear = false;
                prog->m_passes.append(model);

                ProgKeep::RenderPassKeep *disp = new ProgKeep::RenderPassKeep("To Display Pass");
                disp->conf->what = PassConf::Quad_Tex0;
                disp->conf->to = PassConf::Display;
                prog->m_passes.append(disp);

                prog->m_passes.append(new ProgKeep::SwapPassKeep("Swap", 0, 1));
                break;
            }
        }
    }

}

void ConfXmls::loadProgramsElement(QMenu* menu, QDomElement &de)
{
    for (QDomNode pn = de.firstChild(); !pn.isNull(); pn = pn.nextSibling())
    {
        QDomElement pe = pn.toElement();
        if (pe.isNull())
            continue;
        QDomAttr disa = pe.attributeNode("display");
        if (disa.isNull())
            continue;
        QString display = disa.value();

        if (pe.tagName() == "program")
        {
            QDomText txt = pe.firstChild().toText();
            ProgKeep* prog = new ProgKeep;
            if (!txt.isNull()) // it's a filename
            {
                QString filename = txt.data();
                parseProgFile(filename, prog);
            }
            else // it's embedded
            {
                loadProgramElement(display, pe, prog);
            }
            menu->addAction(confAddProg(display, prog));

        }
        if (pe.tagName() == "group")
        {
            QMenu *popup = new QMenu(display);
            menu->addMenu(popup);
            loadProgramsElement(popup, pe);
        }
    }
}


void ConfXmls::parseProgFile(const QString& filename, ProgKeep* prog) 
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        printf("could not open examples file %s\n", filename.toLatin1().data());
        return;
    }
    QDomDocument doc("prog");
    if (!doc.setContent(&file))
    {
        file.close();
        printf("could not parse XML file %s\n", filename.toLatin1().data());
        return;
    }
    loadProgramElement(filename, doc.documentElement(), prog);
}


void ConfXmls::loadProgramsFile(const QString& filename, QMenu* menu)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        printf("could not open examples file %s\n", filename.toLatin1().data());
        return;
    }
    QDomDocument doc("conf");
    if (!doc.setContent(&file))
    {
        file.close();
        printf("could not parse XML file %s\n", filename.toLatin1().data());
        return;
    }

    menu->clear();
    foreach(ProgKeep* pi, m_progrep)
        delete pi;
    m_progrep.clear();


    QDomElement de = doc.documentElement();
    loadProgramsElement(menu, de);
}



QAction* ConfXmls::confAddModel(const QString& display, const QString& filename, bool isMesh)
{
    QAction *act = new QAction(display, m_parent);

    act->setData(QVariant::fromValue(ModelData(filename, isMesh)));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));

    m_userActions[display.toLower()] = act;
    return act;
}

QAction* ConfXmls::confAddProg(const QString& display, ProgKeep* prog)
{
    m_progrep.append(prog);

    QAction *act = new QAction(display, m_parent);
    act->setData((size_t)prog);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadProg()));

    m_userActions[display.toLower()] = act;
    return act;
}


void ConfXmls::myreadProg()
{
    QAction *send = qobject_cast<QAction*>(sender());
    ProgKeep *prog = (ProgKeep*)send->data().toInt();
    emit readProg(prog);
}
void ConfXmls::myreadModel()
{
    QAction *send = qobject_cast<QAction*>(sender());
    QString name = send->text();
    ModelData md = send->data().value<ModelData>();
    emit readModel(name, md);
}


void ConfXmls::activateAction(const QString& name)
{
    TActionMap::iterator it = m_userActions.find(name.toLower());
    if (it == m_userActions.end())
        return;
    (*it)->trigger();
}


//----------------------------------------------------------------------

const char* textDocName(ElementType t) {
    switch (t) {
    case SRC_VTX:  return "vtxtext";
    case SRC_FRAG: return "fragtext";
    case SRC_GEOM: return "geomtext";
    default:
        cout << "Unknown doc type " << t << endl;
        return NULL;
    }
}
const char* paramTypeName(EParamType t) {
    switch(t) {
    case EPFloat: return "float";
    case EPFloatRange: return "floatRange";
    case EPFloatTime: return "floatTime";
    case EPInt: return "int";
    case EPVec2: return "vec2";
    case EPVec3: return "vec3";
    case EPVec4: return "vec4";
    case EPVec3Color: return "vec3color";
    case EPVec4Color: return "vec4color";
    case EPTexture: return "texture";
    default:
        cout << "Unknown param type " << t << endl;
        return NULL;
    }
}

bool saveParam(QDomDocument& xml, QDomElement& xmlprm, const ParamInput& prm) {
    xmlprm.setAttribute("name", prm.name);
    auto typeName = paramTypeName(prm.type);
    if (!typeName)
        return false;
    xmlprm.setAttribute("type", typeName);
    xmlprm.setAttribute("kind", prm.isUniform ? "uniform":"attribute");
    xmlprm.appendChild(xml.createTextNode(prm.value));
    if (prm.guiconf != NULL) {
        QString gui;
        if (prm.type == EPVec2) {
            Vec2GuiConf* g = dynamic_cast<Vec2GuiConf*>(prm.guiconf);
            if (!g) {
                cout << "wrong gui type vec2" << endl;
                return false;
            }
            if (!g->relative)
                gui = "screen";
            else
                gui = QString("%1,%2,%3,%4").arg(g->xmin).arg(g->xmax).arg(g->ymin).arg(g->ymax);
        }
        else if (prm.type == EPFloatRange) {
            FloatGuiConf* g = dynamic_cast<FloatGuiConf*>(prm.guiconf);
            if (!g) {
                cout << "wrong gui type float" << endl;
                return false;
            }
            gui = QString("%1,%2").arg(g->vmin).arg(g->vmax);
        }
        else {
            cout << "Unknown gui conf" << endl;
            return false;
        }
        xmlprm.setAttribute("gui", gui);
    }
    return true;
}


bool ConfXmls::saveProg(const Document& doc, const QString& filename)
{
    QDomDocument xml("prog");
    QDomElement main = xml.createElement("program");
    xml.appendChild(main);

    foreach(const PassPtr &pass, doc.m_passes)
    {
        RenderPassPtr rpass = dynamic_pointer_cast<RenderPass>(pass);
        if (rpass) {
            QDomElement xmlpass = xml.createElement("pass");
            main.appendChild(xmlpass);
            xmlpass.setAttribute("name", rpass->name());
            foreach(DocSrcPtr doc, rpass->shaders) {
                const char* name = textDocName(doc->type);
                if (!name)
                    return false;               
                QDomElement xmldoc = xml.createElement(name);
                xmlpass.appendChild(xmldoc);
                QString t = "\n" + doc->text;
                if (t.size() > 0 && t.right(1) != "\n")
                    t += "\n";
                xmldoc.appendChild(xml.createCDATASection(t));
                xmldoc.setAttribute("name", doc->name());
            }
            foreach(const ParamInput& prm, rpass->params) {
                QDomElement xmlprm = xml.createElement("param");
                xmlpass.appendChild(xmlprm);
                if (!saveParam(xml, xmlprm, prm))
                    return false;
            }
            const auto& conf = rpass->conf.props();
            int xxx = rpass->conf.what;
            for (auto it = conf.constBegin(); it != conf.constEnd(); ++it) {
                Prop* p = it.value();
                if (!p->isEqualToDefault()) {
                    QDomElement xmlcfg = xml.createElement("conf");
                    xmlpass.appendChild(xmlcfg);
                    QString k = p->mname();
                    QString v = p->toString();
                    xmlcfg.setAttribute("name", k);
                    xmlcfg.appendChild(xml.createTextNode(v));
                }
            }
            continue;
        }

        SwapOpPassPtr spass = dynamic_pointer_cast<SwapOpPass>(pass);
        if (spass) {
            QDomElement xmlpass = xml.createElement("swappass");
            main.appendChild(xmlpass);
            xmlpass.setAttribute("a", spass->a);
            xmlpass.setAttribute("b", spass->b);
            continue;
        }
        cout << "Unknown pass" << endl;
        return false;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        printf("could not open save file %s\n", filename.toLatin1().data());
        return false;
    }
    file.write(xml.toByteArray());
    file.close();


    return true;
}