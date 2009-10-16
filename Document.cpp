#include "Document.h"
#include "ShapeIFS.h"
#include "DisplayConf.h"
#include "KawaiiGL.h"

#include "MyLib/MeshIO.h"
#include "OpenGL/glGlob.h"
#include "ProgInput.h"
#include "ShapeIFS.h"

#include <QFileDialog>

using namespace boost; // shared_ptr

// ADDTYPE
template<> bool isEType(EParamType t, const float&) { return (t == EPFloatRange || t == EPFloat || t == EPFloatTime); }
template<> bool isEType(EParamType t, const int& v) { return (t == EPInt || t == EPTexture); }
template<> bool isEType(EParamType t, const Vec&) { return (t == EPVec3 || t == EPVec3Color); }
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
	,m_inputUnit(-1), m_outputUnit(-1), m_confxmls(mainc)
	,m_shaderEnabled(false)
{
	connect(&m_conf.addFace, SIGNAL(changed()), this, SLOT(setAddTrack()));
	connect(&m_conf.materialCol, SIGNAL(changed()), this, SLOT(calcNoParse())); 

	m_confxmls.init(EDIT_CONF_FILE, PROG_CONF_FILE);
	connect(&m_confxmls, SIGNAL(readProg(ProgKeep*)), this, SLOT(readProg(ProgKeep*)));
	connect(&m_confxmls, SIGNAL(readModel(const QString&, const ModelData&)), this, SLOT(readModel(const QString&, const ModelData&)));


	m_kparser.addFunction("wholeScreenQuad");

	// default empty program
	PassPtr p = newPass("Pass 1");
	p->model = shared_ptr<DocSrc>(new DocSrc("Model", false, SRC_MODEL));
	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Vertex Shader", false, SRC_VTX)));
	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Geometry Shader", false, SRC_GEOM)));
	p->shaders.append(shared_ptr<DocSrc>(new DocSrc("Fragment Shader", false, SRC_FRAG)));
	m_passes.append(p);
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

PassPtr Document::newPass(const QString& name)
{
	PassPtr p(new Pass(name));
	connect(&p->conf, SIGNAL(changed()), this, SIGNAL(progParamChanged()));
	return p;
}

// from config
void Document::readProg(ProgKeep* prog)
{
	PassPtr p = newPass("Pass 1");

	// remove everything except the model of the first pass
	if (!m_passes.isEmpty())
		p->model = m_passes[0]->model;
	m_passes.clear(); // removes them from the edit window as well

	// read texts
	DocSrc *d;
	
	if (!prog->vtxProg.isEmpty())
	{
		d = new DocSrc(prog->vtxProg, true, SRC_VTX);
		readToString(d->name(), d->text);
		p->shaders.append(shared_ptr<DocSrc>(d));
	}
	if (!prog->fragProg.isEmpty())
	{
		d = new DocSrc(prog->fragProg, true, SRC_FRAG);
		readToString(d->name(), d->text);
		p->shaders.append(shared_ptr<DocSrc>(d));
	}
	if (!prog->geomProg.isEmpty())
	{
		d = new DocSrc(prog->geomProg, true, SRC_GEOM);
		readToString(d->name(), d->text);
		p->shaders.append(shared_ptr<DocSrc>(d));
	}
	m_passes.append(p);

	p->conf.runType = prog->runType;

	// set config params
	for(ProgKeep::TArgsMap::iterator it = prog->args.begin(); it != prog->args.end(); ++it)
	{
		Prop* prop = m_conf.propByName(it.key());
		prop->fromString(*it);
	}

	emit didReadProg(prog);
}


void Document::addNewShader(Pass* pass, ElementType type)
{
	DocSrc *d = new DocSrc("Shader", false, type);
	pass->shaders.append(shared_ptr<DocSrc>(d));
}

void Document::removeShader(Pass* pass, DocSrc* src)
{
	// The list contains shared_ptrs, we have a pointer, need to look for it manually.
	Pass::TDocSrcList &shaders = pass->shaders;
	Pass::TDocSrcList::iterator it = shaders.begin();
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


void Document::readModel(const QString& name, const ModelData& md)
{
	if (m_passes.isEmpty())
		return;
	PassPtr pass = m_passes[0];

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
	default: return QIcon();
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

void Document::calcSave()
{
	QString filename = QFileDialog::getSaveFileName(m_main, "Save Object", "", "object files (*.obj)");
	if (filename.isEmpty())
		return;

	calc(NULL, false, filename);
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
		MyPolygon *poly = m_obj->AddPoly(&v[0]->getCoord(), &v[1]->getCoord(), &v[2]->getCoord(), (v.size() > 3)?(&v[3]->getCoord()):NULL, constAncs);
		for(size_t i = 0; i < v.size(); ++i)
		{
			Vec gc = v[i]->getColor();
			if (!gc.isValidColor())
				gc = m_defColor;
			poly->vtx[i]->col = gc;
		}
	}

	Vec m_defColor; // default color;
	MyObject* m_obj;
	static TexAnchor constAncs[4];
};

TexAnchor MyObjAdder::constAncs[4] = { TexAnchor(0,0), TexAnchor(1,0), TexAnchor(1,1), TexAnchor(0,1) };


struct MeshAdder : public StringAdder
{
	MeshAdder(Document* doc) : m_doc(doc) {}
	virtual void operator()(const string& s)
	{
		QString filename = QString::fromStdString(s).toLower();
		Document::TMeshIndex::iterator it = m_doc->m_meshIndex.find(filename);
		shared_ptr<RMesh> mesh;
		if (it != m_doc->m_meshIndex.end())
		{  // TBD: compare date as well
			mesh = it.value();
		}
		else
		{
			mesh.reset(new RMesh(filename));
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

void Document::calc(DocSrc* src, bool doParse, QString saveAs)
{
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

	if (!saveAs.isEmpty())
	{
		m_obj->saveAs(saveAs);
	}


	g_alloc.checkMaxAlloc(); // pool maintainance

	emit modelChanged();
}


void Document::updateTrack(IPoint* sel)
{
	if (!m_conf.addFace)
		return;

	QString curtext;
	if (m_addTrack.add(sel))
	{
		int size = m_addTrack.m_added.size();
		if (size == 3 || size == 4)
		{
			QString line;
			if (curtext.size() > 0 && curtext[curtext.size() - 1] != '\n')
				line += '\n';
			line += "add(";
			line += m_addTrack.m_added[0]->getName().c_str();
			for(int i = 1; i < m_addTrack.m_added.size(); ++i)
			{
				IPoint *p = m_addTrack.m_added[i];
				line += ", ";
				line += p->getName().c_str();
			}
			line += ")\n";

			curtext += line;
			m_addTrack.reset();
			emit textChanged(curtext);
		}

		m_addTrack.reset();
		
	}

}


void Document::compileShaders(const ProgInput& in)
{
	m_prog.clear();
	if (!m_shaderEnabled)
		return;

	if (!isValid())
	{ // we did not parse any model yet so the parser is invalid. wake it up.
		calc(NULL);
	}

	if (m_passes.isEmpty())
		return;
	const PassPtr &pass = m_passes[0];

	foreach(const shared_ptr<DocSrc>& src, pass->shaders)
	{
		if (src->text.trimmed().isEmpty())
			continue;
		if (src->type == SRC_VTX)
			m_prog.vtxprogs() += src->text;
		else if (src->type == SRC_FRAG)
			m_prog.fragprogs() += src->text;
		else if (src->type == SRC_GEOM)
			m_prog.geomprogs() += src->text;
	}

	if (m_prog.vtxprogs().size() + m_prog.fragprogs().size() + m_prog.geomprogs().size() == 0)
		return;

	m_conf.runType = pass->conf.runType.val();

	m_onCalcEvals.clear();
	m_onCalcEvals.resize(in.params.size());
	m_onFrameEvals.clear();
	m_onFrameEvals.resize(in.params.size());
	m_attribEval.clear();

	for(int i = 0; i < in.params.size(); ++i)
	{
		const ParamInput &pi = in.params[i];
		ShaderParam *sp;
		if (pi.isUniform)
			sp = new UniformParam(pi.name);
		else
			sp = new AttribParam(pi.name);
		m_prog.addParam(sp, i);
		pi.index = i;
	}

	ProgCompileConf conf(pass->conf.geomInput, pass->conf.geomOutput, pass->conf.geomVtxCount);
	if (m_prog.init(conf))
	{
	//	parseAllParams(in);
	}

	//emit progChanged();
}




bool Document::parseSingleParam(const ParamInput& pi, Prop* toprop, bool update)
{
	if (!m_prog.isOk())
		return false;

	bool ok;
	{
		ProgramUser use(&m_prog);
		ok = parseParam(pi, toprop);
	}

	mglCheckErrorsC("parseSingleParam");

	if (update)
		emit progParamChanged(); // causes updateGL
	return ok;
}

// parse uniform. this defines what a uniform variable can be.
bool Document::parseParam(const ParamInput& pi, Prop* toprop)
{
	if (!pi.isUniform)
	{
		return parseAttrib(pi);
	}
	

	QByteArray ba = (pi.name + "=" + pi.value).toAscii();
	const char* iter = ba.data();
	const char* end = ba.data() + ba.size();
	const char* nameend = ba.data() + pi.name.size();

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
				m_prog.setUniform(f, pi.index);
			}
		}
		break;
	case EPInt:
		{
			float f = 0.0f;
			if (ok = m_kparser.kparseFloat(iter, end, nameend, f))
			{
				int i = (int)f;
				m_prog.setUniform(i, pi.index);
			}
		}
		break;
	case EPVec2:
		{
			Vec2 v;
			if (ok = m_kparser.kparseVec2(nameend + 1, end, v))
			{
				m_prog.setUniform(v, pi.index);
			}
		}
		break;
	case EPVec3:
	case EPVec3Color:
		{
			IPoint *v = NULL;
			if (ok = m_kparser.kparseVec(iter, end, nameend, v))
			{
				shared_ptr<ParamAdapter> pa(new VecParamAdapter(v, toprop, pi.index));
				m_onCalcEvals[pi.index] = pa;

				pa->update(m_prog);
			}
		}
		break;
	case EPVec4:
	case EPVec4Color:
		{
			Vec4 v;
			if (ok = m_kparser.kparseVec4(nameend + 1, end, v))
			{
				m_prog.setUniform(v, pi.index);
			}
		}
		break;
	case EPTexture:
		{
			int ti = pi.value.toInt(&ok);
			if (ok)
			{
				m_prog.setUniform(ti, pi.index);
				m_onFrameEvals[pi.index].reset();
			}
			else
			{
				QString v = pi.value.trimmed().toLower();
				if (v.startsWith("input"))
				{
					shared_ptr<ParamAdapter> pa(new TexParamAdapter(this, pi.index));
					m_onFrameEvals[pi.index] = pa;

					pa->update(m_prog);
					ok = true;
				}
			}
		}
		break;
	}

	return ok;
}


void Document::VecParamAdapter::update(GenericShader &prog) const
{
	if (pe == NULL)
		return;
	Vec ev = pe->getCoord();
	prog.setUniform(ev, index);

	if (toprop != NULL)
	{
		TypeProp<QColor> *cprop = dynamic_cast<TypeProp<QColor> *>(toprop);
		if (cprop != NULL)
			*cprop = ev.toColor(); 
	}
}

void Document::TexParamAdapter::update(GenericShader &prog) const
{
	int tex = m_doc->m_inputUnit;
	if (tex == -1)
		return;
	prog.setUniform(tex, index);
}


void Document::updateParams(TAdaptersList &adapters)
{
	mglCheckErrorsC("startUpdateParams");
	{
		ProgramUser use(&m_prog);
		foreach(const shared_ptr<ParamAdapter>& pa, adapters)
		{
			if (pa.get() != NULL)
				pa->update(m_prog); // there are only Vec that need updates at the moment.
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
	virtual bool initWithMesh(const RMesh *rmesh)
	{
		m_enabled = rmesh->hasVtxProp(MPROP_TANGENT);
		return m_enabled;
	}
	virtual void setAttribVal(RMesh::Vertex_const_handle vh)
	{
		if (!m_enabled)
			return;
		m_prog->setAttrib(vh->prop<Vec>(MPROP_TANGENT), index);
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
	virtual bool initWithMesh(const RMesh *rmesh)
	{
		m_enabled = rmesh->hasVtxProp(MPROP_BITANGENT);
		return m_enabled;
	}
	virtual void setAttribVal(RMesh::Vertex_const_handle vh)
	{
		if (!m_enabled)
			return;
		m_prog->setAttrib(vh->prop<Vec>(MPROP_BITANGENT), index);
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

	atte->setEnv(&m_prog, pi.index);
	m_attribEval.append(shared_ptr<AttribEval>(atte));
	return true;
}

void Document::initAttribMesh(const RMesh* rmesh)
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

void Document::setAttribs(RMesh::Vertex_const_handle vh)
{
	for(int i = 0; i < m_attribEval.size(); ++i)
	{
		m_attribEval[i]->setAttribVal(vh);
	}
}
