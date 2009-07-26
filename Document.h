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
			ProgramUser use(&m_prog);
			m_prog.setUniform(val, pi.index);
		}
		emit progParamChanged(); // causes updateGL
	}


public slots:
	void calc(const QString& qstr, bool doParse = true, QString saveAs = QString());
	void calcNoParse();
	void calcSave();

	void compileShaders(const ProgInput& in);

	bool parseSingleParam(const ParamInput& in, Prop* toprop, bool update);

	void updateParams(); // updates the values of all the parsed params according to the new cache.
	
	void updateTrack(IPoint* sel);

	bool parseAttrib(const ParamInput &pi);
	void setAttribs(MyPolygon* poly, int vi); // set values to attributes
	void setAttribs(RMesh::Vertex_const_handle vh);
	void initAttribMesh(const RMesh* rmesh);

private slots:
	void setAddTrack();

signals:
	void loaded();
	void modelChanged();
	void progChanged(); // just compiled a program, update the view
	void progParamChanged();
	void textChanged(const QString& text);

private:
	bool parseParam(const ParamInput& in, Prop* toprop); // if not NULL this prop should get the value as well.

	int m_nPoly, m_nPoints;
	DisplayConf &conf;
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
		ParamAdapter() : pe(NULL), toprop(NULL), index(-1) {}
		ParamAdapter(IPoint* _pe, int _i) : pe(_pe), index(_i) {}
		IPoint* pe;
		Prop* toprop; // if not NULL this prop should get the value as well.
		
		int index; // index of the program parameter we need to update.

		void updateVec(GenericShader &prog) const;
	};

	QVector<ParamAdapter> m_paramsEvals;

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

	QString m_curtext;
	KwParser m_kparser;

	GenericShader m_prog;
	bool m_quadProcess;
};

#endif // DOCUMENT_H_INCLUDED