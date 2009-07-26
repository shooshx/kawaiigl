#ifndef __EXPPARSER_H_INCLUDED__
#define __EXPPARSER_H_INCLUDED__

#include <string>
#include <deque>
#include "MyLib/Vec.h"

using namespace std;

class DefPoint
{
public:
	DefPoint() :point(0.0f, 0.0f, 0.0f), color(1.0f, 1.0f, 1.0f), used(false) {}
	DefPoint(string n, const Vec& p) :name(n), point(p), color(1.0f, 1.0f, 1.0f), used(false) {}
	DefPoint(const Vec& p, const Vec c) 
		:point(p), color(c), used(false) {}
	string name;
	Vec point;
	Vec color;
	bool used; // is it used in a polygon? (should it be displayed?)
};
typedef map<string, DefPoint> TSymTable;

class MyObject;
class Cell;
class QString;
typedef deque<Cell*> Expression; // TBD-find a better name for this

class ExpParser
{
public:
	ExpParser(TSymTable *sym, MyObject *obj, QString *decompile) : m_symbols(sym), m_obj(obj), m_decompile(decompile) {}

	bool parse(char* in, bool bSymLoad, int &errstrt, int &errcnt);
	static QString defDecomp(const DefPoint &d);

private:
	Cell* PostEval(Expression &post, bool bSymLoad);
	Cell* Eval2(Cell* n1, Cell* op, Cell* n2);
	char* Char2Eqn(char* text, Expression &eqn);
	bool In2Post(Expression &in, Expression &post);

	TSymTable *m_symbols;
	MyObject *m_obj;
	QString *m_decompile;
	
};

#endif __EXPPARSER_H_INCLUDED__