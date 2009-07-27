#include "ConfXmls.h"

#include <QDomDocument>
#include <QMenu>
#include <QFile>

#include "KwEdit.h"
#include "DisplayConf.h"

void ConfXmls::loadModelsFile(QWidget *w)
{
	QFile file(m_modelsFile);
	if (!file.open(QIODevice::ReadOnly))
	{
		printf("could not open examples file %s\n", m_modelsFile.toAscii().data());
		return;
	}
	QDomDocument doc("conf");
	if (!doc.setContent(&file))
	{
		file.close();
		printf("could not parse XML file %s\n", m_modelsFile.toAscii().data());
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

		w->addAction(m_host->confAddModel(display, filename, isMesh));


	}
}


void ConfXmls::loadProgramElement(QMenu* menu, const QString& display, QDomElement &pe)
{
	ProgKeep *prog = new ProgKeep;
	prog->name = display;

	for (QDomNode n = pe.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement e = n.toElement();
		if (e.isNull())
			continue;
		if (e.tagName() == "vtxtext")
			prog->vtxProg = e.text(); // contains the filenames
		else if (e.tagName() == "fragtext")
			prog->fragProg = e.text();
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
				printf("unknown type %s\n", type.toAscii().data());

			bool isUniform = true;
			QDomAttr kinda = e.attributeNode("kind");
			if (!kinda.isNull())
			{
				if (kinda.value() != "uniform")
					isUniform = false;
			}

			ParamInput pi(namea.value(), etype, e.text(), isUniform);
			
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


			prog->params.append(pi);

		}
		else if (e.tagName() == "quadproc")
		{
			int op = e.text().toInt();
			prog->quadProcess = (bool)op;
		}
		else if (e.tagName() == "arg")
		{
			QDomAttr namea = e.attributeNode("name");
			if (namea.isNull())
				continue;
			QString name = namea.value();
			if (m_host->m_conf.propByName(name) == NULL)
				printf("No such property %s\n", name.toAscii().data());
			else
				prog->args[name] = e.text();
		}

	}

	menu->addAction(m_host->confAddProg(display, prog));

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
			loadProgramElement(menu, display, pe);
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
		printf("could not open examples file %s\n", m_progFiles.toAscii().data());
		return;
	}
	QDomDocument doc("conf");
	if (!doc.setContent(&file))
	{
		file.close();
		printf("could not parse XML file %s\n", m_progFiles.toAscii().data());
		return;
	}

	menu->clear();
	foreach(ProgInput* pi, m_host->m_progrep)
		delete pi;
	m_host->m_progrep.clear();


	QDomElement de = doc.documentElement();
	loadProgramsElement(menu, de);


}

