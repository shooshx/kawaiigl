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




Document::Document(KawaiiGL* mainc)
	:m_nPoly(0), m_nPoints(0), m_errAct(NULL), m_frameObj(NULL), m_obj(NULL),
	 conf(mainc->sett().disp), m_main(mainc), m_quadProcess(false)
{
	connect(&conf.addFace, SIGNAL(changed()), this, SLOT(setAddTrack()));

	connect(&conf.materialCol, SIGNAL(changed()), this, SLOT(calcNoParse())); 

	m_kparser.addFunction("wholeScreenQuad");
}



void Document::setAddTrack()
{
	m_addTrack.reset();
}

void Document::calcNoParse() 
{ 
	calc(QString(), false); 
}

void Document::calcSave()
{
	QString filename = QFileDialog::getSaveFileName(m_main, "Save Object", "", "object files (*.obj)");
	if (filename.isEmpty())
		return;

	calc(QString(), false, filename);
}


MyAllocator g_alloc(50000, 50000, 50000);


struct MyObjAdder : public PolyAdder
{
	MyObjAdder(MyObject* obj, DisplayConf& conf) : m_obj(obj), m_defColor(conf.materialCol)
	{}
	virtual void operator()(vector<IPoint*>& v)
	{
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

void Document::calc(const QString& qstr, bool doParse, QString saveAs)
{
	if (doParse)
	{
		m_curtext = qstr;
		QByteArray ba = qstr.toAscii();

		const char* iter = ba.data();
		const char* end = ba.data() + ba.size();
		m_errAct->clear();
		m_kparser.kparse(iter, end, false, m_errAct);
		m_errAct->finish();

		m_meshs.clear();
		m_rends.clear();
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
	

	MyObjAdder adder(m_frameObj, conf);
	m_kparser.creator()->createPolygons(&adder);
	updateParams(); // if there's a prog active, it might depend on the variables.

	//	currentDecompile.clear();


	//	emitDecompile(); 
	//pntDlg->updatePoint();
	//	if (bSymLoad)
	//		updateSelPointColor();// take the color from the text

	m_frameObj->vectorify();
	m_frameObj->clacNormals(conf.bVtxNormals);

	m_obj = new MyObject(*m_frameObj); // copy it
	m_obj->detachPoints();

	for (int i = 0; i < conf.numberOfPasses; ++i)
	{
		if (!m_obj->subdivide(*conf.passRound[i]))
			break; // stop it
	}
	m_nPoly = m_obj->nPolys;
	m_nPoints = m_obj->nPoints;
	m_obj->clacNormals(conf.bVtxNormals);

	if (!saveAs.isEmpty())
	{
		m_obj->saveAs(saveAs);
	}


	g_alloc.checkMaxAlloc(); // pool maintainance

	emit modelChanged();
}


void Document::updateTrack(IPoint* sel)
{
	if (!conf.addFace)
		return;
	if (m_addTrack.add(sel))
	{
		int size = m_addTrack.m_added.size();
		if (size == 3 || size == 4)
		{
			QString line;
			if (m_curtext[m_curtext.size() - 1] != '\n')
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

			m_curtext += line;
			emit textChanged(m_curtext);
		}

		m_addTrack.reset();
		
	}

}


void Document::compileShaders(const ProgInput& in)
{
	m_prog.clear();

	if (!in.vtxProg.isEmpty())
		m_prog.vtxprogs() += in.vtxProg;
	if (!in.fragProg.isEmpty())
		m_prog.fragprogs() += in.fragProg;

	m_quadProcess = in.quadProcess;

	m_paramsEvals.clear();
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
		m_paramsEvals.append(ParamAdapter(NULL, i));
		pi.index = i;
	}

	if (m_prog.init())
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
				ParamAdapter& pa = m_paramsEvals[pi.index];
				pa.pe = v;
				pa.toprop = toprop;

				pa.updateVec(m_prog);
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
			}
		}
		break;
	}

	return ok;
}


void Document::ParamAdapter::updateVec(GenericShader &prog) const
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


void Document::updateParams()
{
	mglCheckErrorsC("startUpdateParams");
	{
		ProgramUser use(&m_prog);
		foreach(const ParamAdapter& pa, m_paramsEvals)
		{
			pa.updateVec(m_prog); // there are only Vec that need updates at the moment.
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
