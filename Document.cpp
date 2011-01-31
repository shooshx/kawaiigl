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


#define EDIT_CONF_FILE ("conf.edit.xml")
#define PROG_CONF_FILE ("conf.programs.xml")


Document::Document(KawaiiGL* mainc)
	:m_nPoly(0), m_nPoints(0), m_errAct(NULL), m_frameObj(NULL), m_obj(NULL)
	,m_conf(mainc->sett().disp), m_main(mainc)
	//,m_inputUnit(-1), m_outputUnit(-1),
	,m_confxmls(mainc)
	,m_shaderEnabled(false)
	,m_inCalc(false)
{
	connect(&m_conf.addFace, SIGNAL(changed()), this, SLOT(setAddTrack()));
	connect(&m_conf.materialCol, SIGNAL(changed()), this, SLOT(calcNoParse())); 

	m_confxmls.init(EDIT_CONF_FILE, PROG_CONF_FILE);
	connect(&m_confxmls, SIGNAL(readProg(ProgKeep*)), this, SLOT(readProg(ProgKeep*)));
	connect(&m_confxmls, SIGNAL(readModel(const QString&, const ModelData&)), this, SLOT(readModel(const QString&, const ModelData&)));


	m_kparser.addFunction("wholeScreenQuad");

	// default empty program
	RenderPassPtr p(new RenderPass("Pass 1"));
	p->model = shared_ptr<DocSrc>(new DocSrc("Model", false, SRC_MODEL));
	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Vertex Shader", false, SRC_VTX)));
	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Geometry Shader", false, SRC_GEOM)));
	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Fragment Shader", false, SRC_FRAG)));
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
		printf("can't open %s\n", filename.toAscii().data());
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
		printf("can't open %s\n", filename.toAscii().data());
	}
	file.setTextModeEnabled(true);
	QTextStream out(&file);
	out << text;
}

// find the pass where the model should go
RenderPassPtr Document::passForModel()
{
	RenderPassPtr rpass;
	for (TPasses::iterator it = m_passes.begin(); it != m_passes.end(); ++it)
	{
		rpass = dynamic_pointer_cast<RenderPass>(*it);
		if (rpass && rpass->conf.what == PassConf::Model)
			break;
	}
	return rpass;
}



// from config
void Document::readProg(ProgKeep* prog)
{
	// remove everything except the model of the first pass
	DocSrcPtr model;
	
	for (TPasses::iterator it = m_passes.begin(); it != m_passes.end(); ++it) // need to find the model
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
			DocSrc *d = new DocSrc(sk.name, true, sk.type);
			readToString(d->name(), d->text);
			p->shaders.append(shared_ptr<DocSrc>(d));
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
	for(ProgKeep::TArgsMap::iterator it = prog->args.begin(); it != prog->args.end(); ++it)
	{
		Prop* prop = m_conf.propByName(it.key());
		prop->fromString(*it);
	}

	emit didReadProg(prog);
}


void Document::addNewShader(RenderPass* pass, ElementType type)
{
	DocSrc *d = new DocSrc("Shader", false, type);
	pass->shaders.append(shared_ptr<DocSrc>(d));
}

void Document::	loadShaderFile(RenderPass* pass, const QString& filename, ElementType type)
{
	DocSrc *d = new DocSrc(filename, true, type);
	readToString(d->name(), d->text);
	pass->shaders.append(shared_ptr<DocSrc>(d));
}


void Document::removeShader(RenderPass* pass, DocSrc* src)
{
	// The list contains shared_ptrs, we have a pointer, need to look for it manually.
	RenderPass::TDocSrcList &shaders = pass->shaders;
	RenderPass::TDocSrcList::iterator it = shaders.begin();
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
	RenderPassPtr p(new RenderPass(QString("Pass %1").arg(m_passes.size() + 1) ));
	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Vertex Shader", false, SRC_VTX)));
//	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Geometry Shader", false, SRC_GEOM)));
	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Fragment Shader", false, SRC_FRAG)));
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
	TPasses::iterator it = m_passes.begin();
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


void Document::readModel(const QString& name, const ModelData& md)
{
	RenderPassPtr pass = passForModel();
	if (!pass)
		return;

	QString text; 

	if (name == "Generate from File")
	{
		text = generateFromFile();
		pass->model.reset(new DocSrc(text, "Generated", false, SRC_MODEL));
	}
	else if (name == "Load File")
	{
		QString filename = QFileDialog::getOpenFileName(m_main, "Load Model", m_conf.lastDir, "model files (*.obj *.off *.ply2 *.gsd)");
		if (filename.isEmpty())
			return;
		m_conf.lastDir = QFileInfo(filename).absolutePath();
		filename = QDir::current().relativeFilePath(filename);

		text = "load(" + filename + ")";
		pass->model.reset(new DocSrc(text, "Loaded Mesh", false, SRC_MODEL));
	}
	else if (name == "clear")
	{
		text = "";
		pass->model.reset(new DocSrc(text, "Model", false, SRC_MODEL));
	}
	else
	{
		//ModelData md = send->data().value<ModelData>();
		if (!md.isMesh) 
		{
			Document::readToString(md.filename, text);
			pass->model.reset(new DocSrc(text, md.filename, true, SRC_MODEL));
		}
		else
		{
			text = "load(" + md.filename + ")";
			pass->model.reset(new DocSrc(text, "Loaded Mesh", false, SRC_MODEL));
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


	QString prog;


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
	m_addTrack.reset();
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

	if (!m_obj)
		calc(NULL, false); // can this happen?

	
	QFile file(filename);
	if (!file.open(QFile::WriteOnly))
	{
		printf("unable to open file %s\n", filename.toAscii().data());
		return;
	}

	file.setTextModeEnabled(true);
	QTextStream out(&file);

	if (selectedFilter == FILTER_OBJ)
	{
		m_obj->saveAs(out, "obj");
	}
	else if (selectedFilter == FILTER_JSON_TRI)
	{
		m_obj->saveAs(out, "json", MyObject::SaveTriangles);
		foreach(const shared_ptr<Mesh>& mesh, m_meshs)
		{
			JsonWriter writer(mesh.get());
			writer.write(out);
		}
	}
	else if (selectedFilter == FILTER_JSON_QUADS)
	{
		m_obj->saveAs(out, "json", MyObject::SaveQuads);
	}
	else if (selectedFilter == FILTER_JSON_LINES)
	{
		m_obj->saveAs(out, "json", MyObject::SaveEdges);	
	}
	else
	{
		printf("unknown filter\n");
	}


}


MyAllocator g_alloc(50000, 50000, 50000);


struct MyObjAdder : public PolyAdder
{
	MyObjAdder(MyObject* obj, DisplayConf& conf) : m_obj(obj), m_defColor(conf.materialCol)
	{}
	virtual void operator()(vector<IPoint*>& v)
	{
		if (v.size() < 3)
			return;
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
	static TexAnchor constAncs[4];
};

TexAnchor MyObjAdder::constAncs[4] = { TexAnchor(0,0), TexAnchor(1,0), TexAnchor(1,1), TexAnchor(0,1) };


struct MeshAdder : public StringAdder
{
	MeshAdder(Document* doc) : m_doc(doc) {}
	virtual void operator()(const string& s)
	{
		QString filename = QString(s.c_str()).toLower();
		Document::TMeshIndex::iterator it = m_doc->m_meshIndex.find(filename);
		shared_ptr<Mesh> mesh;
		if (it != m_doc->m_meshIndex.end())
		{  // TBD: compare date as well
			mesh = it.value();
		}
		else
		{
			mesh.reset(new Mesh(filename.toAscii().data()));
			if (MeshIO::read_Ext(filename, mesh.get()))
			{
				mesh->finalize(false);
				//mesh->translateCenter();
				mesh->rescaleAndCenter(6.0f);
			}
			m_doc->m_meshIndex[filename] = mesh;
		}

		m_doc->m_meshs.append(mesh);
	}
	Document *m_doc;
};

struct FuncAdder : public StringAdder
{
	FuncAdder(Document* doc) : m_doc(doc) {}
	virtual void operator()(const string& s)
	{
		if (s == "wholeScreenQuad")
			m_doc->m_rends.append(shared_ptr<Renderable>(new WholeScreenQuad(NULL)));
	}
	Document *m_doc;
};

void Document::calc(DocSrc* src, bool doParse)
{
	// prevent recursive calls which occur due to syntax highlighing
	if (m_inCalc)
		return;
	m_inCalc = true; 

	if (doParse)
	{
		m_meshs.clear();
		m_rends.clear();

		QByteArray ba;
		if (src != NULL)
		{
			ba = src->text.toAscii();
		} // otherwise it's empty

		const char* iter = ba.data();
		const char* end = ba.data() + ba.size();
		m_errAct->clear(src);
		m_kparser.kparse(iter, end, false, m_errAct);
		m_errAct->finish();

		m_kparser.creator()->addMeshes(&MeshAdder(this));
		m_kparser.creator()->callFuncs(&FuncAdder(this));

		emit loaded();
	}
	else
	{
		if (!m_kparser.isValid())
			return;
	}

	//m_kparser.creator()->printTree();
	
	g_alloc.clear();
	m_frameObj = new MyObject(&g_alloc);
	

	MyObjAdder adder(m_frameObj, m_conf);
	m_kparser.creator()->createPolygons(&adder);
	updateParams(m_onCalcEvals); // if there's a prog active, it might depend on the variables.

	//	currentDecompile.clear();


	//	emitDecompile(); 
	//pntDlg->updatePoint();
	//	if (bSymLoad)
	//		updateSelPointColor();// take the color from the text

	m_frameObj->vectorify();
	m_frameObj->clacNormals(m_conf.bVtxNormals);

	delete m_obj;
	m_obj = new MyObject(*m_frameObj); // copy it
	m_obj->detachPoints();

	for (int i = 0; i < m_conf.numberOfPasses; ++i)
	{
		if (!m_obj->subdivide(*m_conf.passRound[i]))
			break; // stop it
	}
	m_nPoly = m_obj->nPolys;
	m_nPoints = m_obj->nPoints;
	m_obj->clacNormals(m_conf.bVtxNormals);



	g_alloc.checkMaxAlloc(); // pool maintainance

	emit modelChanged();
	m_inCalc = false;
}


void Document::updateTrack(IPoint* sel)
{
	if (!m_conf.addFace)
		return;

	if (m_addTrack.add(sel))
	{
		int size = m_addTrack.m_added.size();
		if (size == 3 || size == 4)
		{
			QString line;
			line += "add(";
			line += m_addTrack.m_added[0]->getName().c_str();
			for(int i = 1; i < m_addTrack.m_added.size(); ++i)
			{
				IPoint *p = m_addTrack.m_added[i];
				line += ", ";
				line += p->getName().c_str();
			}
			line += ")\n";

			m_addTrack.reset();
			emit addModelLine(line);
		}

		m_addTrack.reset();
		
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
	if (!isValid())
	{ // we did not parse any model yet so the parser is invalid. wake it up.
		calc(NULL);
	}


	m_onCalcEvals.clear();
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
			ShaderParam *sp;
			if (pi.isUniform)
				sp = new UniformParam(pi.name);
			else
				sp = new AttribParam(pi.name);
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
	if (!pi.mypass->prog.isOk())
		return false;

	bool ok;
	{
		ProgramUser use(&pi.mypass->prog);
		ok = parseParam(pi);
	}

	mglCheckErrorsC("parseSingleParam");

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
	if (!pi.isUniform)
	{
		pi.lastParseOk = parseAttrib(pi);
		return pi.lastParseOk;
	}
	

	QByteArray ba = (pi.name + "=" + pi.value).toAscii();
	const char* iter = ba.data();
	const char* end = ba.data() + ba.size();
	const char* nameend = ba.data() + pi.name.size();

	GenericShader &prog = pi.mypass->prog;

	bool ok = true;
	switch(pi.type) // ADDTYPE
	{
	case EPFloat: 
	case EPFloatRange:
	case EPFloatTime:
		{
			float f = 0.0f;
			if (ok = m_kparser.kparseFloat(iter, end, nameend, f))
			{
				prog.setUniform(f, pi.index);
			}
		}
		break;
	case EPInt:
		{
			float f = 0.0f;
			if (ok = m_kparser.kparseFloat(iter, end, nameend, f))
			{
				int i = (int)f;
				prog.setUniform(i, pi.index);
			}
		}
		break;
	case EPVec2:
		{
			Vec2 v;
			if (ok = m_kparser.kparseVec2(nameend + 1, end, v))
			{
				prog.setUniform(v, pi.index);
			}
		}
		break;
	case EPVec3:
	case EPVec3Color:
		{
			IPoint *v = NULL;
			if (ok = m_kparser.kparseVec(iter, end, nameend, v))
			{
				shared_ptr<ParamAdapter> pa(new VecParamAdapter(v, pi.prop, pi.index));
				m_onCalcEvals[uniqueParamId(pi)] = pa;

				pa->update(prog);
			}
		}
		break;
	case EPVec4:
	case EPVec4Color:
		{
			Vec4 v;
			if (ok = m_kparser.kparseVec4(nameend + 1, end, v))
			{
				prog.setUniform(v, pi.index);
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
				prog.setUniform(ti, pi.index);
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
