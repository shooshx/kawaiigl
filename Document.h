#pragma once
#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED

#include <QObject>
#include <boost/shared_ptr.hpp>

#include "KwParser.h"
#include "ProgInput.h"
#include "MyLib/RMesh.h"

#include "OpenGL/Shaders.h"
#include "Renderable.h"
#include "ConfXmls.h"
#include "Pass.h"

class MyObject;
class DisplayConf;
class KawaiiGL;
class Prop;
class MyPolygon;

// track interactive adding of points
class AddTracker
{
public:
	void reset()
	{
		m_added.clear();
	}
	// returns true if done
	bool add(IPoint* p)
	{
		if (!m_added.isEmpty() && p == m_added.first())
			return true;
		m_added.append(p);
		return false;
	}

	QVector<IPoint*> m_added;
};




class Document : public QObject
{
	Q_OBJECT
public:
	Document(KawaiiGL* mainc);
	~Document() {}

	int numPoly() { return m_nPoly; }
	int numPoints() { return m_nPoints; }
	bool isValid() { return m_kparser.isValid(); }

	void emitProgChanged() { emit progChanged(); }

	template<typename T>
	void directUpdate(const ParamInput& pi, const T& val)
	{
		if (!isEType(pi.type, val))
			return;
		{
			ProgramUser use(&pi.mypass->prog);
			pi.mypass->prog.setUniform(val, pi.index);
		}
		emit progParamChanged(); // causes updateGL
	}

	static QIcon getTypeIcon(ElementType);
	static QString getTypeName(ElementType);

	void addNewShader(RenderPass* pass, ElementType type);
	void loadShaderFile(RenderPass* pass, const QString& filename, ElementType type);
	void removeShader(RenderPass* pass, DocSrc* src);

	void addNewPass();
	void removePass(Pass* pass);
	void movePass(Pass* pass, int delta);

	void addNewSwap();

	bool isProgEnabled() const { return m_shaderEnabled; }

	void addPass(PassPtr pass);
	void clearPasses();
	RenderPassPtr passForModel(); // find the pass where the model should go.

public slots:
	void calc(DocSrc* qstr, bool doParse = true, QString saveAs = QString());
	void calcNoParse();
	void calcSave();

	void compileShaders();

	bool parseSingleParam(const ParamInput& in, bool update);

	
	void updateTrack(IPoint* sel);

	bool parseAttrib(const ParamInput &pi);
	void setAttribs(MyPolygon* poly, int vi); // set values to attributes
	void setAttribs(RMesh::Vertex_const_handle vh);
	void initAttribMesh(const RMesh* rmesh);

private slots:
	void setAddTrack();
	void readProg(ProgKeep* prog); // fcom config
	void readModel(const QString& name, const ModelData& md);

signals:
	void loaded();
	void modelChanged();
	void progChanged(); // just compiled a program, update the view
	void progParamChanged();
	void textChanged(const QString& text);

	void goingToClearProg();
	void didReadProg(ProgKeep* prog);
	void didReadModel(DocSrc* src);

private:
	bool parseParam(const ParamInput& in); // if not NULL this prop should get the value as well.
	void parseAllParams(const RenderPassPtr &pass);
	
	QString generateFromFile();

	int m_nPoly, m_nPoints;
	DisplayConf &m_conf;
	KawaiiGL *m_main;

public:
	ErrorActor *m_errAct;
	MyObject *m_frameObj, *m_obj;
	QVector<boost::shared_ptr<RMesh> > m_meshs;
	QVector<boost::shared_ptr<Renderable> > m_rends;

	AddTracker m_addTrack;

	// this connects the parser with the program. changes in the parser to the program
	struct ParamAdapter
	{
		ParamAdapter(int _index) :index(_index) {}
		virtual ~ParamAdapter() {}
		virtual void update(GenericShader &prog) const = 0;

		int index; // index of the program parameter we need to update.
	};

	struct VecParamAdapter : public ParamAdapter
	{
		VecParamAdapter(IPoint* _pe, Prop* _prop, int _i) : pe(_pe), toprop(_prop), ParamAdapter(_i) {}
		IPoint* pe;
		Prop* toprop; // if not NULL this prop should get the value as well.
		
		virtual void update(GenericShader &prog) const;
	};

	struct TexParamAdapter : public ParamAdapter
	{
		TexParamAdapter(Document* doc, int _i) : m_doc(doc), ParamAdapter(_i) {}
		virtual void update(GenericShader &prog) const;

		Document* m_doc;
	};

	// this maps passName + paramNumbae (string) to the adapter.
	// the mapping is done to avoid multiple adapters for the same parameter
	// TBD: need to get rid of them when they change type?
	typedef QMap<QString, boost::shared_ptr<ParamAdapter> >  TAdaptersList;
	TAdaptersList m_onCalcEvals; // things that are re-evaluated when there's a calc() call
	TAdaptersList m_onFrameEvals; // things that are reeveluated every frame

	struct AttribEval
	{
		AttribEval() : m_prog(NULL), index(-1) {}
		void setEnv(GenericShader *prog, int i) 
		{
			m_prog = prog;
			index = i;
		}

		int index; // index of the program parameter we need to update.
		GenericShader *m_prog;

		virtual void setAttribVal(const MyPolygon* poly, int vindex) = 0;
		virtual bool initWithMesh(const RMesh *rmesh) = 0;
		virtual void setAttribVal(RMesh::Vertex_const_handle vh) = 0;
	};


	QVector<boost::shared_ptr<AttribEval> > m_attribEval;

	typedef QMap<QString, boost::shared_ptr<RMesh> > TMeshIndex;
	TMeshIndex m_meshIndex;

	//QString m_curtext; // of model
	KwParser m_kparser;

	//GenericShader m_prog;

	//int m_inputUnit, m_outputUnit; // used in Tex2Tex; not any more
	// these are here since they need to be sent as uniforms and we need access to them.

	typedef QList<PassPtr> TPasses;
	TPasses m_passes;

	bool m_shaderEnabled;

	ConfXmls m_confxmls;


public:
	void updateParams(TAdaptersList& list); // updates the values of all the parsed params according to the new cache.
	void updateFrameParams()
	{
		updateParams(m_onFrameEvals);
	}

	static void readToString(const QString& filename, QString& into);
	static void writeToFile(const QString& text, const QString filename);

};

#endif // DOCUMENT_H_INCLUDED