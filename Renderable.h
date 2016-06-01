#pragma once
#ifndef KRENDERABLE_H_INCLUDED
#define KRENDERABLE_H_INCLUDED

#include "MyLib/Vec.h"
#include <vector>

using namespace std;

class GLWidget;
class Document;

class Renderable
{
public:
    Renderable() {}
    virtual ~Renderable() {}
    
    virtual void render(GLWidget *m_glw) = 0;
};


class WholeScreenQuad : public Renderable
{
public:
    WholeScreenQuad(int _texUnit, const Vec3& _backCol = Vec3(1.0, 1.0, 1.0)) 
        : backCol(_backCol), texUnit(_texUnit) {}

    WholeScreenQuad(const Vec3& _backCol = Vec3(1.0, 1.0, 1.0), int _texUnit = 1) 
        : backCol(_backCol), texUnit(_texUnit) {}

    virtual void render(GLWidget *glw);

    Vec3 backCol;
    uint texUnit;
};

class ModelDocument;

class ArrowRenderable : public Renderable
{
public:
    ArrowRenderable();
    bool init(const vector<string>& sep, ModelDocument* doc);
    void init(const Vec3 _from, const Vec3& _to, const Vec3 _color);

    virtual void render(GLWidget *glw);

    Vec3 from, to, color;
    float barRadius, tipRadius, tipLen, barLen;
    int slices;
};



#endif // KRENDERABLE_H_INCLUDED