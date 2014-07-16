#pragma once
#ifndef KWPARSET_H_INCLUDED
#define KWPARSET_H_INCLUDED

//#include <memory> // auto_ptr
#include <vector>
#include <string>

#include "MyLib/Vec.h"


class QString;


struct IPoint
{
    virtual void setCoord(const Vec3& v) = 0;
    virtual const Vec3& getCoord() const = 0;
    virtual void setColor(const Vec3& c) = 0;
    virtual const Vec3& getColor() const = 0;
    virtual const std::string& getName() const = 0;
    virtual void purgeCache() = 0;
};

struct PolyAdder
{
    virtual void operator()(std::vector<IPoint*>& pnts) = 0;
};

struct StringAdder
{
    virtual void operator()(const std::string& s) = 0;
};
struct MultiStringAdder
{
    virtual void operator()(const std::vector<std::string>& s) = 0;
};

struct PointActor
{
    virtual void operator()(const std::string& name, const Vec3& p, bool used, IPoint* handle) = 0;
};

class IPolyCreator
{
public:
    virtual ~IPolyCreator() {}
    virtual void cacheVecs() = 0;
    virtual void printTree() = 0;
    virtual void createPolygons(PolyAdder *adder) = 0;
    virtual void addMeshes(StringAdder *adder) = 0;
    virtual void callFuncs(MultiStringAdder *adder) = 0;
    virtual void foreachPoints(PointActor &actor) = 0;
    // get a point by its name or NULL if it doesn't exist
    virtual IPoint* lookupSymbol(const std::string& pname) = 0;
};

class DocSrc;
struct ErrorActor
{
    virtual void clear(DocSrc* src) = 0;
    virtual void operator()(int start, int end) = 0;
    virtual void finish() = 0;
};

template <typename Iterator>
struct kwprog;
template<typename T>
struct Evalable;

class Model;

class KwParser
{
public:
    KwParser();
    ~KwParser();
    void addFunction(const std::string& s);
    
    bool kparse(const char* iter, const char* end, bool verbose, ErrorActor* error);

    // float assignment
    bool kparseFloat(const char* iter, const char* end, const char* nameend, float& f);   
    Evalable<float>* kparseFloat(const char* iter, const char* end, const char* nameend);

    // Vec3 assignment
    bool kparseVec(const char* iter, const char* end, const char* nameend, IPoint*& p);
    
    // Vec2,4 expression
    bool kparseVec2(const char* iter, const char* end, Vec2& p);
    bool kparseVec4(const char* iter, const char* end, Vec4& p);

    // float expresssion
    bool kparseFloatExp(const std::string& expression, float& f);


    bool isValid() const { return m_kg.get() != NULL; }
    IPolyCreator* creator();
    Model* model() {
        return m_model;
    }

    // used in iso func evaluation
    void defineFloatSym(const std::string& s);

private:
    typedef const char* iterator_type;
    std::auto_ptr<Model> m_modelOwn;
    Model* m_model;

    std::auto_ptr<kwprog<iterator_type>> m_kg;
    typedef std::vector<std::string> TFuncs;
    TFuncs m_knownFunctions;
};

#endif // KWPARSET_H_INCLUDED
