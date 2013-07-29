#ifndef PASS_H_INCLUDED
#define PASS_H_INCLUDED
#pragma once

#include "gl/glew.h"
#include <QObject>
#include <QString>
#include <QList>
#include <general.h>

#include "MyLib/ParamBase.h"
#include "DisplayConf.h"
#include "ProgInput.h"
#include "OpenGL/Shaders.h"

enum ElementType
{
    SRC_TEXTUAL = 0x100,
    SRC_VTX =    0x101,
    SRC_GEOM =   0x102,
    SRC_FRAG =   0x103,
    SRC_MODEL =  0x104,

    SRC_DIALOG =  0x200,
    SRC_RENDER =   0x201,
    SRC_OP_SWAP= 0x202,
    SRC_NONE
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
    bool textual() const { return (type & SRC_TEXTUAL) == SRC_TEXTUAL; }
    virtual QString displayName() const = 0;

    void setName(const QString& newname)
    {
        if (m_name == newname)
            return;
        m_name = newname;
        emit nameChanged(displayName());
    }
    const QString& name() const { return m_name; }

signals:
    void removed(DocElement*);
    void nameChanged(const QString&);

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
    DocSrc(const QString& _name, ElementType _type) 
        :DocElement(_type, _name), changedSinceLoad(false)
    {} 
    DocSrc(const QString& _text, const QString& _name, ElementType _type) 
        :DocElement(_type, _name), text(_text), changedSinceLoad(false)
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

    QString text;

private:
    bool changedSinceLoad;
};

typedef shared_ptr<DocSrc> DocSrcPtr;

class PassConf : public ParamBase
{
    Q_OBJECT
public:

    enum EGeomInType
    {
        POINTS = GL_POINTS, 
        LINES = GL_LINES, 
    //	LINES_ADJACENCY_EXT = GL_LINES_ADJACENCY_EXT,  TBD
        TRIANGLES = GL_TRIANGLES, 
    //	TRIANGLES_ADJACENCY_EXT = GL_TRIANGLES_ADJACENCY_EXT
    };
    Q_ENUMS(EGeomInType);
    enum EGeomOutType
    {
        POINTS_ = GL_POINTS, 
        LINE_STRIP = GL_LINE_STRIP, 
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP 
    };
    Q_ENUMS(EGeomOutType);

    enum ERenderWhat
    {
        Model = -1,
        Model_From_Light = -2,
        Quad_Tex0 = 0,
        Quad_Tex1,
        Quad_Tex2,
        Quad_Tex3
    };
    Q_ENUMS(ERenderWhat);
    enum ERenderTo
    {
        Display = -1,
        Texture0 = 0,
        Texture1,
        Texture2,
        Texture3
    };
    Q_ENUMS(ERenderTo)

    TypeProp<bool> incPoly; // include polygons in program
    TypeProp<bool> incPoints; // include points in program
    
    TypeProp<bool> vertexPointSize;
    TypeProp<bool> pointSpirits;
    TypeProp<bool> spiriteReplace; // replace texture coordinates for spirite points

    TypeProp<EGeomInType> geomInput;
    TypeProp<EGeomOutType> geomOutput;
    TypeProp<int> geomVtxCount;
    //TypeProp<DisplayConf::RunType> runType;

    TypeProp<ERenderWhat> what;
    TypeProp<ERenderTo> to;
    TypeProp<bool> toMultisample;  // relevant only if to is not display
    TypeProp<bool> clear; // relevant only if we're printing a model

    PassConf() 
        :incPoly(this, "includePoly", true)
        ,incPoints(this, "includePoint", false)
        ,vertexPointSize(this, "vertexPointSize", true)
        ,pointSpirits(this, "pointSpirits", false)
        ,spiriteReplace(this, "spiriteReplace", true)
        ,geomInput(this, "geomInput", POINTS)
        ,geomOutput(this, "geomOutput", POINTS_)
        ,geomVtxCount(this, "geomVtxCount", 3)
        //,runType(this, "runType", DisplayConf::RunNormal)
        ,what(this, "what", Model)
        ,to(this, "to", Display)
        ,toMultisample(this, "toMultisample", true)
        ,clear(this, "clear", true)
    {}

};

Q_DECLARE_METATYPE(PassConf::EGeomInType);
Q_DECLARE_METATYPE(PassConf::EGeomOutType);
Q_DECLARE_METATYPE(PassConf::ERenderWhat);
Q_DECLARE_METATYPE(PassConf::ERenderTo);

class Pass : public DocElement
{
public:
    Pass(ElementType type, const QString& name) : DocElement(type, name)
    {}
    virtual ~Pass() {}

    virtual QString displayName() const { return m_name; }
};


class RenderPass : public Pass 
{
public:
    RenderPass(const QString& name) : Pass(SRC_RENDER, name) 
    {
        conf.reset();
    }
    
    QList<DocSrcPtr> shaders;

    DocSrcPtr model;
    QVector<ParamInput> params; 

    PassConf conf;
    GenericShader prog;

private:
    Q_DISABLE_COPY(RenderPass);
};

class SwapOpPass : public Pass
{
public:
    SwapOpPass(const QString& name) : Pass(SRC_OP_SWAP, name), a(PassConf::Texture0), b(PassConf::Texture1)
    {}
    PassConf::ERenderTo a,b; // indexes to swap
};


typedef shared_ptr<Pass> PassPtr;
typedef shared_ptr<RenderPass> RenderPassPtr;
typedef shared_ptr<SwapOpPass> SwapOpPassPtr;


class ProgKeep
{
public:
    QString name;

    struct SrcKeep
    {
        SrcKeep() :type(SRC_NONE) {}
        SrcKeep(const QString& filename, const QString& _text, ElementType _type)
            :name(filename), text(_text), type(_type)
        {}

        QString name;
        QString text;
        ElementType type;
    };

    struct PassKeep
    {
        PassKeep(const QString& _name) : name(_name) {}
        virtual ~PassKeep() {}
        QString name;
    };
    struct RenderPassKeep : public PassKeep
    {
        RenderPassKeep(const QString& _name = QString()) : PassKeep(_name)
        {
            conf = new PassConf();
            conf->reset();
        }
        
        QVector<SrcKeep> shaders;
        SrcKeep model; // optional...
        QVector<ParamInput> params; 
        PassConf *conf; // can't be copied
    };
    struct SwapPassKeep : public PassKeep
    {
        SwapPassKeep(const QString& name, int _a, int _b) :PassKeep(name), a(_a), b(_b) {}
        int a, b;
    };
    QVector<PassKeep*> m_passes; 

    QMap<QString, QString> args; // any name from DisplayConf and a value as string

};




#endif // PASS_H_INCLUDED