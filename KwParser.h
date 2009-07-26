#pragma once
#ifndef KWPARSET_H_INCLUDED
#define KWPARSET_H_INCLUDED

#include <memory> // auto_ptr
#include <vector>
#include <string>

#include "MyLib/Vec.h"


class QString;


struct IPoint
{
	virtual void setCoord(const Vec& v) = 0;
	virtual const Vec& getCoord() const = 0;
	virtual void setColor(const Vec& c) = 0;
	virtual const Vec& getColor() const = 0;
	virtual const std::string& getName() const = 0;
};

struct PolyAdder
{
	virtual void operator()(std::vector<IPoint*>& pnts) = 0;
};

struct StringAdder
{
	virtual void operator()(const std::string& s) = 0;
};

struct PointActor
{
	virtual void operator()(const std::string& name, const Vec& p, bool used, IPoint* handle) = 0;
};

class IPolyCreator
{
public:
	virtual ~IPolyCreator() {}
	virtual void cacheVecs() = 0;
	virtual void printTree() = 0;
	virtual void createPolygons(PolyAdder *adder) = 0;
	virtual void addMeshes(StringAdder *adder) = 0;
	virtual void callFuncs(StringAdder *adder) = 0;
	virtual void foreachPoints(PointActor &actor) = 0;
};

struct ErrorActor
{
	virtual void clear() = 0;
	virtual void operator()(int start, int end) = 0;
	virtual void finish() = 0;
};

template <typename Iterator>
struct kwprog;

class KwParser
{
public:
	KwParser() : m_g(NULL), m_kg(NULL) {}
	void addFunction(const std::string& s);

	bool kparse(const char* iter, const char* end, bool verbose, ErrorActor* error);

	bool kparseFloat(const char* iter, const char* end, const char* nameend, float& f);
	bool kparseVec(const char* iter, const char* end, const char* nameend, IPoint*& p);
	
	bool kparseVec2(const char* iter, const char* end, Vec2& p);
	bool kparseVec4(const char* iter, const char* end, Vec4& p);


	bool isValid() const { return m_g.get() != NULL; }
	IPolyCreator* creator();

private:
	typedef const char* iterator_type;
	std::auto_ptr<IPolyCreator> m_g;

	kwprog<iterator_type> *m_kg;
	typedef std::vector<std::string> TFuncs;
	TFuncs m_knownFunctions;
};

#endif // KWPARSET_H_INCLUDED
