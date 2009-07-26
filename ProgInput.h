#ifndef PROGINPUT_H_INCLUDED
#define PROGINPUT_H_INCLUDED

#include <QString>
#include <QMap>

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

struct ParamInput
{
	ParamInput() {}
	ParamInput(const QString& _name, EParamType _type, const QString& _value, bool _isUniform)
		:name(_name), type(_type), value(_value), isUniform(_isUniform), index(-1), guiA(0.0), guiB(0.0) {}
	QString name;
	QString value;
	EParamType type;
	bool isUniform; // false if attribute

	float guiA, guiB; // gui arguments [Vec2:A->0.0 relative,1.0 absolute] [float:min, max]

	mutable int index; // in the program registry.
};

struct ProgInput
{
	ProgInput() : quadProcess(false) {}

	QString vtxProg;
	QString fragProg;
	QVector<ParamInput> params; 
	bool quadProcess;
};

// a program that is loaded from the XML
struct ProgKeep : public ProgInput
{
	typedef QMap<QString, QString> TArgsMap;
	QString name;
	TArgsMap args; // any name from DisplayConf and a value as string
};


#endif // PROGINPUT_H_INCLUDED

