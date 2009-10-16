#ifndef PASS_H_INCLUDED
#define PASS_H_INCLUDED
#pragma once

#include "gl/glew.h"
#include <QObject>
#include <QString>
#include <QList>
#include <boost/shared_ptr.hpp>

#include "MyLib/ParamBase.h"
#include "DisplayConf.h"
#include "ProgInput.h"

enum ElementType
{
	SRC_VTX,
	SRC_GEOM,
	SRC_FRAG,
	SRC_MODEL,
	SRC_CONF
};

// things that can have a page in the edit window
class DocElement : public QObject
{
	Q_OBJECT
public:
	DocElement(ElementType _type, const QString _name) : type(_type), m_name(_name) {}
	virtual ~DocElement()
	{
		emit removed(this);
	}
	bool textual() const { return type != SRC_CONF; }
	virtual QString displayName() const = 0;

signals:
	void removed(DocElement*);

public:
	ElementType type;

protected:
	QString m_name; // just name or filename;

private:
	Q_DISABLE_COPY(DocElement);
};

// DocSrc doesn't know in which Pass he is!
class DocSrc :  public DocElement
{
	Q_OBJECT
public:
	DocSrc(const QString& _name, bool isFile, ElementType _type) 
		:DocElement(_type, _name), isFilename(isFile), changedSinceLoad(false)
	{} 
	DocSrc(const QString& _text, const QString& _name, bool isFile, ElementType _type) 
		:DocElement(_type, _name), text(_text), isFilename(isFile), changedSinceLoad(false)
	{} 


	virtual QString displayName() const { return changedSinceLoad?(m_name + "*"):m_name; }

	// returns true if it really changed first now.
	bool setChangedSinceLoad(bool v)
	{
		if (v == changedSinceLoad)
			return false;
		changedSinceLoad = v;
		emit nameChanged(displayName());
		return true;
	}

	void setName(const QString& newname)
	{
		if (m_name == newname)
			return;
		m_name = newname;
		emit nameChanged(displayName());
	}
	const QString name() const { return m_name; }

	bool isFilename;
	QString text;


signals:
	void nameChanged(const QString&);
	void externalTextChange();

private:
	bool changedSinceLoad;
};



class PassConf : public ParamBase
{
	Q_OBJECT
public:

	enum EGeomInType
	{
		POINTS = GL_POINTS, 
		LINES = GL_LINES, 
	//	LINES_ADJACENCY_EXT = GL_LINES_ADJACENCY_EXT, 
		TRIANGLES = GL_TRIANGLES, 
	//	TRIANGLES_ADJACENCY_EXT = GL_TRIANGLES_ADJACENCY_EXT
	};
	Q_ENUMS(EGeomInType);
	enum EGeomOutType
	{
		_POINTS = GL_POINTS, 
		LINE_STRIP = GL_LINE_STRIP, 
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP 
	};
	Q_ENUMS(EGeomOutType);


	TypeProp<bool> incPoly; // include polygons in program
	TypeProp<bool> incPoints; // include points in program
	
	TypeProp<bool> vertexPointSize;
	TypeProp<bool> pointSpirits;
	TypeProp<bool> spiriteReplace; // replace texture coordinates for spirite points

	TypeProp<EGeomInType> geomInput;
	TypeProp<EGeomOutType> geomOutput;
	TypeProp<int> geomVtxCount;
	TypeProp<DisplayConf::RunType> runType;


	PassConf() 
		:incPoly(this, "includePoly", true)
		,incPoints(this, "includePoint", false)
		,vertexPointSize(this, "vertexPointSize", true)
		,pointSpirits(this, "pointSpirits", false)
		,spiriteReplace(this, "spiriteReplace", true)
		,geomInput(this, "geomInput", POINTS)
		,geomOutput(this, "geomOutput", _POINTS)
		,geomVtxCount(this, "geomVtxCount", 3)
		,runType(this, "runType", DisplayConf::RunNormal)
	{}

};

Q_DECLARE_METATYPE(PassConf::EGeomInType);
Q_DECLARE_METATYPE(PassConf::EGeomOutType);


class Pass : public DocElement 
{
public:
	Pass(const QString& name) : DocElement(SRC_CONF, name) 
	{
		conf.reset();
	}
	virtual QString displayName() const { return m_name; }
	
	typedef QList<boost::shared_ptr<DocSrc> > TDocSrcList;
	TDocSrcList shaders;

	boost::shared_ptr<DocSrc> model;
	QVector<ParamInput> params; 

	PassConf conf;
private:
	Q_DISABLE_COPY(Pass);
};

typedef boost::shared_ptr<Pass> PassPtr;


#endif // PASS_H_INCLUDED