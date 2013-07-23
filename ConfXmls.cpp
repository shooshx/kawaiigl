#include "ConfXmls.h"

#include <QDomDocument>
#include <QMenu>
#include <QFile>

#include "KwEdit.h"
#include "DisplayConf.h"
#include "KawaiiGL.h"
#include "Pass.h"


ConfXmls::ConfXmls(KawaiiGL* mainc) :m_conf(mainc->sett().disp), m_parent(mainc) {}


void ConfXmls::init(const QString& modelsFile, const QString& progFiles)
{
	m_modelsFile = modelsFile;
	m_progFiles = progFiles;

	m_progMenu = new QMenu(m_parent);
	m_modelsMenu = new QMenu(m_parent);

	loadModelsFile();
	loadProgramsFile(m_progMenu);
}

void ConfXmls::loadModelsFile()
{
	QFile file(m_modelsFile);
	if (!file.open(QIODevice::ReadOnly))
	{
		printf("could not open examples file %s\n", m_modelsFile.toLatin1().data());
		return;
	}
	QDomDocument doc("conf");
	if (!doc.setContent(&file))
	{
		file.close();
		printf("could not parse XML file %s\n", m_modelsFile.toLatin1().data());
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

		m_modelsMenu->addAction(confAddModel(display, filename, isMesh));
	}

	// other actions
	QAction *act;
	m_modelsMenu->addAction(act = new QAction("", m_parent));
	act->setSeparator(true);
	m_modelsMenu->addAction(act = new QAction("clear", m_parent));
	connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));
	m_modelsMenu->addAction(act = new QAction("Generate from File", m_parent));
	connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));
	m_modelsMenu->addAction(act = new QAction("Load File", m_parent));
	connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));

	m_modelsMenu->addAction(act = new QAction("Dedicated processing", m_parent));
	connect(act, SIGNAL(triggered(bool)), this, SLOT(myreadModel()));

}

void ConfXmls::loadPassElement(ProgKeep::RenderPassKeep* pass, QDomElement &pe)
{
	for (QDomNode n = pe.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement e = n.toElement();
		if (e.isNull())
			continue;
		if (e.tagName() == "vtxtext")
			pass->shaders.append(ProgKeep::SrcKeep(e.text(), SRC_VTX)); // contains the filenames
		else if (e.tagName() == "fragtext")
			pass->shaders.append(ProgKeep::SrcKeep(e.text(), SRC_FRAG));
		else if (e.tagName() == "geomtext")
			pass->shaders.append(ProgKeep::SrcKeep(e.text(), SRC_GEOM));
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
					if (guival == "screen")
						g->relative = false;
					else if (guival == "relative")
						g->relative = true;
					else
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

void ConfXmls::loadProgramElement(QMenu* menu, const QString& display, QDomElement &pe)
{
	ProgKeep *prog = new ProgKeep;
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

			QDomAttr namea = pe.attributeNode("name");
			if (namea.isNull())
				pass->name = QString("Pass %1").arg(prog->m_passes.size() + 1);
			else
				pass->name = namea.value();

			loadPassElement(pass, e);
			prog->m_passes.append(pass);

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

	menu->addAction(confAddProg(display, prog));

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
			if (!txt.isNull()) // it's a filename
			{
				QString filename = txt.data();
				QFile file(filename);
				if (!file.open(QIODevice::ReadOnly))
				{
					printf("could not open examples file %s\n", filename.toLatin1().data());
					continue;
				}
				QDomDocument doc("program");
				if (!doc.setContent(&file))
				{
					file.close();
					printf("could not parse XML file %s\n", filename.toLatin1().data());
					continue;
				}	
				loadProgramElement(menu, display, doc.documentElement());
			}
			else // it's embedded
			{
				loadProgramElement(menu, display, pe);
			}
		}
		if (pe.tagName() == "group")
		{
			QMenu *popup = new QMenu(display);
			menu->addMenu(popup);
			loadProgramsElement(popup, pe);
		}
	}
}

void ConfXmls::loadProgramsFile(QMenu* menu)
{
	QFile file(m_progFiles);
	if (!file.open(QIODevice::ReadOnly))
	{
		printf("could not open examples file %s\n", m_progFiles.toLatin1().data());
		return;
	}
	QDomDocument doc("conf");
	if (!doc.setContent(&file))
	{
		file.close();
		printf("could not parse XML file %s\n", m_progFiles.toLatin1().data());
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
	act->setData((int)prog);
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