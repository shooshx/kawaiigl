#ifndef PROGINPUT_H_INCLUDED
#define PROGINPUT_H_INCLUDED

#include <QString>
#include <QMap>

#include "DisplayConf.h"


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

enum EParamDest
{
    EDAttribute = 0,
    EDUniform,
    EDModel
};

template<typename T>
bool isEType(EParamType t, const T& v);
EParamType getBaseType(EParamType t);


struct GuiConf
{
    virtual ~GuiConf() {}
};
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

class RenderPass;

// trivially copyable.
// single parameter definition
struct ParamInput
{
    ParamInput() {}
    ParamInput(const QString& _name, EParamType _type, const QString& _value, EParamDest _dest, RenderPass* _mypass)
        :name(_name), type(_type), value(_value), dest(_dest), index(-1), guiconf(NULL), 
         prop(NULL), lastParseOk(true), mypass(_mypass)
    {}
    
    QString name;
    QString value;
    EParamType type;
    EParamDest dest; // false if attribute

    GuiConf* guiconf; // gui arguments (loaded from the xml and never destroyed)

    mutable Prop* prop; // the prop to update, if it exists (share with another widget using this) set by the PParamWidget
    mutable int index; // in the program registry.
    mutable bool lastParseOk;

    RenderPass* mypass; // the pass this param is part of.
};



// a program that is loaded from the XML
/*
struct ProgKeep 
{
    ProgKeep() : runType(DisplayConf::RunNormal) {}
    typedef QMap<QString, QString> TArgsMap;

    QString vtxProg, fragProg, geomProg;
    QString name;
    TArgsMap args; // any name from DisplayConf and a value as string
    DisplayConf::RunType runType;
    QVector<ParamInput> params; 
};
*/


struct ModelData
{
    ModelData() {}
    ModelData(QString name, bool _isMesh) :filename(name), isMesh(_isMesh) {}
    QString filename;
    bool isMesh;
};
Q_DECLARE_METATYPE(ModelData);



#endif // PROGINPUT_H_INCLUDED

