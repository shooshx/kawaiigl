#pragma once
#include <vector>
#include <map>
#include <string>

#include "MyLib/Vec.h"

#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/bind.hpp>

using std::string;
using std::vector;

template<typename T>
struct Evalable
{
    virtual ~Evalable() {}
    virtual T eval() = 0;
};


template<typename T>
struct BinaryOp : public Evalable<T>
{
    BinaryOp(Evalable<T>* _leftexpr, Evalable<T>* _rightexpr)
        : left(_leftexpr), right(_rightexpr) 
    {}
    virtual T eval() = 0;
    Evalable<T> *left;
    Evalable<T> *right;
};

template<typename T>
struct BinaryAdd : public BinaryOp<T> {
    BinaryAdd(Evalable<T>* _left, Evalable<T>* _right) : BinaryOp(_left, _right) {}
    virtual T eval() {
        return left->eval() + right->eval();
    }
};
template<typename T>
struct BinarySub : public BinaryOp<T> {
    BinarySub(Evalable<T>* _left, Evalable<T>* _right) : BinaryOp(_left, _right) {}
    virtual T eval() {
        return left->eval() - right->eval();
    }
};
template<typename T>
struct BinaryMult : public BinaryOp<T> {
    BinaryMult(Evalable<T>* _left, Evalable<T>* _right) : BinaryOp(_left, _right) {}
    virtual T eval() {
        return left->eval() * right->eval();
    }
};
template<typename T>
struct BinaryDiv : public BinaryOp<T> {
    BinaryDiv(Evalable<T>* _left, Evalable<T>* _right) : BinaryOp(_left, _right) {}
    virtual T eval() {
        return left->eval() / right->eval();
    }
};

struct FloatPow : public BinaryOp<float> {
    FloatPow(Evalable<float>* _left, Evalable<float>* _right) : BinaryOp(_left, _right) {}
    virtual float eval() {
        return pow(left->eval(), right->eval());
    }
};


struct BinaryOpVec3Float : public Evalable<Vec3>
{
    BinaryOpVec3Float(Evalable<Vec3>* _leftexpr, Evalable<float>* _rightexpr)
        : left(_leftexpr), right(_rightexpr) 
    {}
    virtual Vec3 eval() = 0;
    Evalable<Vec3> *left;
    Evalable<float> *right;
};
struct BinaryVFMult : public BinaryOpVec3Float {
    BinaryVFMult(Evalable<Vec3>* _left, Evalable<float>* _right) : BinaryOpVec3Float(_left, _right) {}
    virtual Vec3 eval() {
        return left->eval() * right->eval();
    }
};
struct BinaryVFDiv : public BinaryOpVec3Float {
    BinaryVFDiv(Evalable<Vec3>* _left, Evalable<float>* _right) : BinaryOpVec3Float(_left, _right) {}
    virtual Vec3 eval() {
        return left->eval() / right->eval();
    }
};

template<typename T>
struct UnaryOp : public Evalable<T> {
    UnaryOp(Evalable<T>* _v) : v(_v) {}
    Evalable<T> *v;
};

template<typename T>
struct UnaryNeg : public UnaryOp<T> {
    UnaryNeg(Evalable<T>* _v) : UnaryOp(_v)  {}
    virtual T eval() {
        return -(v->eval());
    }
};

struct FloatSin : public UnaryOp<float> {
    FloatSin(Evalable<float>* _v) : UnaryOp(_v) {}
    virtual float eval() {
        return sin(v->eval());
    }
};
struct FloatCos : public UnaryOp<float> {
    FloatCos(Evalable<float>* _v) : UnaryOp(_v) {}
    virtual float eval() {
        return cos(v->eval());
    }
};
struct FloatAbs : public UnaryOp<float> {
    FloatAbs(Evalable<float>* _v) : UnaryOp(_v) {}
    virtual float eval() {
        return abs(v->eval());
    }
};


template<typename T>
struct VecVal : public Evalable<T>
{
    VecVal(const Vec3& v) : val(v) {}
    virtual T eval();
    Vec3 val;
};

template<> inline Vec3 VecVal<Vec3>::eval() {
    return val;
}
template<> inline float VecVal<float>::eval() {
    throw std::exception("can't get a float from a vec3");
    //return val;
}

template<typename T>
struct FloatVal : public Evalable<T>
{
    FloatVal(float v) : val(v) {}
    virtual T eval();
    float val;
};

template<> inline Vec3 FloatVal<Vec3>::eval() {
    return Vec3(val, val, val);
}
template<> inline float FloatVal<float>::eval() {
    return val;
}

struct MakeVec3 : public Evalable<Vec3> {
    MakeVec3(Evalable<float>* _x, Evalable<float>* _y, Evalable<float>* _z) : x(_x), y(_y), z(_z)
    {}
    virtual Vec3 eval() {
        return Vec3(x->eval(), y->eval(), z->eval());
    }
    Evalable<float> *x, *y, *z;
};

template<typename T>
class SymTable
{
public:
    uint makeid(const string& name) {
        auto it = m_nameToIndex.find(name);
        if (it == m_nameToIndex.end()) {
            int id = m_index.size();
            m_nameToIndex[name] = id;
            m_index.push_back(NULL);
            return id;
        }
        int id = it->second;
        return id;
    }

    uint set(const string& name, const T& v) {
        uint id = makeid(name);
        m_index[id] = v;
        return id;
    }
    T* find(const string& name)  {
        auto it = m_nameToIndex.find(name);
        if (it == m_nameToIndex.end())
            return NULL;
        return &m_index[it->second];
    }
    T* find(int i)  {
        if (i >= m_index.size())
            return NULL;
        return &m_index[i];
    }
    template<typename FN>
    void for_each(const FN& fn) {
        foreach(auto p, m_index) {
            fn(p);
        }
    }

private:
    vector<T> m_index; //every symbol has an index number
    std::map<string, int> m_nameToIndex;
};


// this class is saved in the vecsym symbols table
struct NamedPoint : public IPoint
{
//    NamedPoint() : color(INVALID_COLOR), expr(NULL), used(false), isCached(false)  
//    {}
//    NamedPoint(const vec_ast& v) : vec_ast(v), color(INVALID_COLOR)
//    {}
    NamedPoint(Evalable<Vec3>* v, const std::string& name) 
        : expr(v), color(INVALID_COLOR), myname(name), used(false), isCached(false) 
    {}

    virtual void setCoord(const Vec3& v)
    {
        expr = new VecVal<Vec3>(v); //mem leak
    }
    virtual const Vec3& getCoord() const
    {
        if (!isCached)
            doCache();
        return cache;
    }
    virtual void setColor(const Vec3& c) {
        color = c;
    }
    virtual const Vec3& getColor() const {
        return color;
    }
    virtual const std::string& getName() const {
        return myname;
    }
    virtual void purgeCache() {
        isCached = false;
    }
    void doCache() const {
        cache = expr->eval();
        isCached = true;
    }

    void print() {}

    std::string myname;
    Vec3 color;
    Evalable<Vec3>* expr;

    mutable Vec3 cache;
    mutable bool isCached;
    bool used;
};

class Model : public IPolyCreator
{
public:
    Model() {}
    virtual ~Model() {}

    virtual void addMeshes(StringAdder *adder) {
        for(vector<string>::iterator it = meshs.begin(); it != meshs.end(); ++it) {
            (*adder)(*it);
        }
    }
    virtual void callFuncs(MultiStringAdder *adder) {
        for(vector<vector<string> >::iterator it = invoked.begin(); it != invoked.end(); ++it) {
            (*adder)(*it);
        }
    }

    void add_point(const std::string& name, Evalable<Vec3>* expr) {
        m_pnts.set(name, new NamedPoint(expr, name));
    }

    virtual void createPolygons(PolyAdder *adder)
    {
        vector<IPoint*> pnts;
        for(size_t i = 0; i < polygons.size(); ++i)
        {
            vector<string> &pol = polygons[i];
            pnts.clear();
            for(size_t p = 0; p < pol.size(); ++p)
            {
                string& pname = pol[p];
                IPoint* va = lookupSymbol(pname);
                if (va != NULL) {
                    pnts.push_back(va);
                }
            }
            (*adder)(pnts);
        }
    }

    virtual IPoint* lookupSymbol(const string& pname) 
    {
        NamedPoint** va = m_pnts.find(pname);
        if (va != NULL) {
            (*va)->used = true;
        }
        return *va;
    }

    virtual void cacheVecs()
    {
        m_pnts.for_each(boost::bind(&NamedPoint::doCache, boost::arg<1>() ));
        //vecsym.for_each(bind(&vec_ast::doCache, _2 ));
    }

    
    virtual void printTree()
    {
        m_pnts.for_each(boost::bind(&NamedPoint::print, boost::arg<1>() ));
    }
    
    /*
    struct vec_adapter
    {
        vec_adapter(PointActor &actor) :m_actor(actor) {}
        void operator()(string& name, symbol_ast& v)
        {
            m_actor(name, v.cache, v.used, &v);
        }
        PointActor &m_actor;
    };*/
    virtual void foreachPoints(PointActor &actor)
    {
        //vecsym.for_each(boost::bind<void>(vec_adapter(actor), boost::arg<1>(), boost::arg<2>() ));
        m_pnts.for_each([&](NamedPoint* p) { actor(p->getName(), p->getCoord(), p->used, p); });
    }

private:
    Model& operator=(const Model&); // disallow copy
    Model(const Model&);

public:
    SymTable<Evalable<float>*> m_numsym;
    SymTable<NamedPoint*> m_pnts;
    vector<vector<std::string>> polygons;
    vector<std::string> meshs;
    vector<vector<std::string>> invoked;
};


struct NumsymLookup : public Evalable<float>
{
    NumsymLookup(Model* _model, const std::string& s) :model(_model), name(s) 
    {
        id = model->m_numsym.makeid(name);
    }
    virtual float eval() {
        Evalable<float> **e = model->m_numsym.find(id);
        if (e == NULL)
            return 0.0; // should not happen
        return (*e)->eval();
    }
    std::string name;
    int id;
    Model* model;
};


