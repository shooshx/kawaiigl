#ifndef PROGINPUT_H_INCLUDED
#define PROGINPUT_H_INCLUDED

#include <QString>
#include <QMap>

#include "DisplayConf.h"

// properties in rmesh we're using
#define MPROP_TEXCOORD (0)
#define MPROP_TANGENT (1)
#define MPROP_BITANGENT (2)


enum EParamType
{
	EPFloat = 0, 
	EPFloatRange,
	EPFloatTime,
	EPInt,
	EPVec2,
	EPVec3,
	EPVec4,
	EPVec3Color,
	EPVec4Color,
	EPTexture
}; // ADDTYPE tag to search for


template<typename T>
bool isEType(EParamType t, const T& v);
EParamType getBaseType(EParamType t);


struct GuiConf
{};
struct Vec2GuiConf : public GuiConf
{
	Vec2GuiConf() : relative(true), xmin(0.0), xmax(1.0), ymin(0.0), ymax(1.0) {}
	bool relative;
	float xmin, xmax, ymin, ymax;
};
struct FloatGuiConf : public GuiConf
{
	FloatGuiConf() : vmin(0.0), vmax(1.0) {}
	float vmin, vmax;
};

struct ParamInput
{
	ParamInput() {}
	ParamInput(const QString& _name, EParamType _type, const QString& _value, bool _isUniform)
		:name(_name), type(_type), value(_value), isUniform(_isUniform), index(-1), guiconf(NULL) {}
	QString name;
	QString value;
	EParamType type;
	bool isUniform; // false if attribute

	GuiConf* guiconf; // gui arguments 

	mutable int index; // in the program registry.
};



struct ProgInput
{
	ProgInput() : runType(DisplayConf::RunNormal) {}

	QString vtxProg;
	QString fragProg;
	QVector<ParamInput> params; 
	DisplayConf::RunType runType;
};

// a program that is loaded from the XML
struct ProgKeep : public ProgInput
{
	typedef QMap<QString, QString> TArgsMap;
	QString name;
	TArgsMap args; // any name from DisplayConf and a value as string
};


#endif // PROGINPUT_H_INCLUDED

