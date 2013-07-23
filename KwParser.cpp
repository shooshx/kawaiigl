#include "KwParser.h"

#pragma warning (disable: 4819) // character in current code page
#pragma warning (disable: 4003) // strange foreach  - not enough actual parameters.
#pragma warning (disable: 4172) // strange float_ - return address of local 
#pragma warning (disable: 4100) // unreferenced formal parameter

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <boost/spirit/include/phoenix_object.hpp>

#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/bind.hpp>

#include <string>
#include "MyLib/Vec.h"


//using namespace boost;
using namespace boost::spirit;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

//using namespace boost::spirit::arg_names;
using namespace boost::phoenix;
using std::string;
using std::vector;

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace lambda = boost::lambda;

struct write_action
{
	template<typename T>
	void operator()(T const& i, qi::unused_type, qi::unused_type) const
	{
		std::cout << i << std::endl;
	}
};

struct null_action
{
	template<typename T>
	void operator()(T const& i, qi::unused_type, qi::unused_type) const
	{

	}
};

struct writer
{
	void print(int const& i) const
	{
		std::cout << i << std::endl;
	}
};

std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
	out << v.toString().toStdString();
	return out;
}


BOOST_FUSION_ADAPT_STRUCT(
	IVec3,
	(float, x)
	(float, y)
	(float, z)
)

BOOST_FUSION_ADAPT_STRUCT(
	IVec2,
	(float, x)
	(float, y)
)

BOOST_FUSION_ADAPT_STRUCT(
	IVec4,
	(float, x)
	(float, y)
	(float, z)
	(float, w)
)



struct addOp {};
struct subOp {};
struct multOp {};
struct divOp {};

struct vec_ast;

struct Evalable
{
	virtual ~Evalable() {}
	virtual Vec3 eval() = 0;

// 	void printSpaces() const
// 	{
// 		for(int i = 0; i < m_spaces; ++i)
// 			printf(" ");
// 	}
	//virtual void print() = 0;
};

typedef Evalable* EvalablePtr;


struct vec_ast 
{
	vec_ast()
		: expr(new EvalablePtr(NULL)), used(false), isCached(false) {}

	vec_ast(const vec_ast& a) : expr(a.expr), isCached(false), used(false) {}
	vec_ast(const Vec3& expr);
	vec_ast(const float& expr);


	vec_ast& operator+=(vec_ast const& rhs);
	vec_ast& operator-=(vec_ast const& rhs);
	vec_ast& operator*=(vec_ast const& rhs);
	vec_ast& operator/=(vec_ast const& rhs);

	void doCache() const
	{
		cache = (*expr)->eval();
		isCached = true;
	}

	void print();


	EvalablePtr *expr;
	mutable Vec3 cache;
	mutable bool isCached;
	bool used;
};

struct symbol_ast : public vec_ast, public IPoint
{
	symbol_ast() : color(INVALID_COLOR) {}
	symbol_ast(const vec_ast& v) : vec_ast(v), color(INVALID_COLOR)
	{}
	symbol_ast(const vec_ast& v, const std::string& name) : vec_ast(v), color(INVALID_COLOR), myname(name)
	{}

	virtual void setCoord(const Vec3& v);
	virtual const Vec3& getCoord() const
	{
		if (!isCached)
			doCache();
		return cache;
	}
	virtual void setColor(const Vec3& c)
	{
		color = c;
	}
	virtual const Vec3& getColor() const
	{
		return color;
	}
	virtual const std::string& getName() const
	{
		return myname;
	}

	std::string myname;
	Vec3 color;
};

template< typename Op>
struct binary_op : public Evalable
{
	binary_op(vec_ast const& _left, vec_ast const& _right)
		: left(const_cast<EvalablePtr*>(_left.expr) ), 
		  right(const_cast<EvalablePtr*>(_right.expr) ) {}

	virtual Vec3 eval();

	EvalablePtr *left;
	EvalablePtr *right;
};


Vec3 binary_op<addOp>::eval()
{
	return (*left)->eval() + (*right)->eval();
}
Vec3 binary_op<subOp>::eval()
{
	return (*left)->eval() - (*right)->eval();
}
Vec3 binary_op<multOp>::eval()
{
	return (*left)->eval() * (*right)->eval();
}
Vec3 binary_op<divOp>::eval()
{
	return (*left)->eval() / (*right)->eval();
}


struct unary_op : public Evalable
{
	unary_op(vec_ast const& _subject)
		: subject(const_cast<EvalablePtr*>(_subject.expr) ) {}

	virtual Vec3 eval()
	{
		return -(*subject)->eval();
	}

	EvalablePtr *subject;
};


struct vec_val : public Evalable
{
	vec_val(const Vec3& v) : val(v) {}
	virtual Vec3 eval()
	{
		return val;
	}
	Vec3 val;
};

struct float_val : public Evalable
{
	float_val(float v) : val(v) {}
	virtual Vec3 eval()
	{
		return Vec3(val, val, val);
	}
	float val;
};

void symbol_ast::setCoord(const Vec3& v) 
{
// 	vec_val *e = dynamic_cast<vec_val*>(expr);
// 	if (e != NULL)
// 		e->val = v;
//	cout << "setCoord\n";
	*expr = new vec_val(v); //mem leak
}


vec_ast::vec_ast(const Vec3& expr)
: expr(new EvalablePtr(new vec_val(expr) )), used(false), isCached(false)
{}
vec_ast::vec_ast(const float& expr)
: expr(new EvalablePtr(new float_val(expr) )), used(false), isCached(false)
{}



vec_ast& vec_ast::operator+=(vec_ast const& rhs)
{
	expr = new EvalablePtr(new binary_op<addOp>(*this, rhs));
	return *this;
}

vec_ast& vec_ast::operator-=(vec_ast const& rhs)
{
	expr = new EvalablePtr(new binary_op<subOp>(*this, rhs));
	return *this;
}

vec_ast& vec_ast::operator*=(vec_ast const& rhs)
{
	expr = new EvalablePtr(new binary_op<multOp>(*this, rhs));
	return *this;
}

vec_ast& vec_ast::operator/=(vec_ast const& rhs)
{
	expr = new EvalablePtr(new binary_op<divOp>(*this, rhs));
	return *this;
}

vec_ast operator*(vec_ast const& lhs, vec_ast const& rhs) // for left-hand multiplication
{
    vec_ast r;
	r.expr = new EvalablePtr(new binary_op<multOp>(lhs, rhs));
	return r;
}




// see comment in calc2_ast.cpp of the spirit2 examples
struct negate_expr
{
	template <typename T>
	struct result { typedef T type; };

	vec_ast operator()(vec_ast const& expr) const
	{
		vec_ast v;
		v.expr = new EvalablePtr(new unary_op(expr));
		return v;
	}
};

boost::phoenix::function<negate_expr> neg;



std::ostream& operator<<(std::ostream& out, vec_ast& v)
{
	out << (*v.expr)->eval();
	//out << v.cache;
	return out;
}


template<typename ValT>
void add_sym(qi::symbols<char, ValT>& table, const std::string& name, const ValT& value);
/*
{
	//cout << name << " : " << value << endl;
	table.add(name.c_str(), value);
}
*/

template<>
void add_sym(qi::symbols<char, symbol_ast>& table, const std::string& name, const symbol_ast& value)
{
	//cout << "ADDV " << name << " : " << "..." << endl;
	const symbol_ast* v = table.find(name);
    if (v != NULL) 
		table.remove(name.c_str());
	table.add(name.c_str(), symbol_ast(value, name));
}
template<>
void add_sym(qi::symbols<char, float>& table, const std::string& name, const float& value)
{
	//cout << "ADDN " << name << " : " << value << endl;
	const float* v = table.find(name);
	if (v != NULL) 
		table.remove(name.c_str()); 
	table.add(name.c_str(), value);
}


template <typename Iterator>
struct kwprog : public qi::grammar<Iterator, void(), ascii::space_type>, public IPolyCreator
{
	kwprog(bool verbose) : kwprog::base_type(program), m_verbose(verbose)
	{

		program = *statement;
			
		statement = (assignvec | assignnum  // vector assignment takes precedence
			| addpoly[phoenix::bind(&kwprog::report_poly, this, _1)] 
			| loadmesh[push_back(phoenix::ref(meshs), _1)]
			| miscfunc[push_back(phoenix::ref(invoked), _1)]
			)
			>> *comment
			; 

		comment = lexeme[ (lit('#') | lit("//")) >> *(ascii::char_ - eol) >> eol];

		//assignnum = (variable >> '=' >> expression)[bind(numsym.add, _1, _2)] ;
		assignnum = (variable >> '=' >> expression)[phoenix::bind(&add_sym<float>, ref(numsym), _1, _2)] ;
		assignvec = (variable >> '=' >> vecExpression)[phoenix::bind(&add_sym<symbol_ast>, ref(vecsym), _1, _2)] ;

		addpoly = 
			(lit("add") >> '(' >> get_name[push_back(_val, _1)] 
			>> ',' >> get_name[push_back(_val, _1)] 
			>> +(',' >> get_name[push_back(_val, _1)]) 
			>> ')')
			;
		loadmesh %= (lit("load") >> '(' >> no_close >> ')');
		loadmesh %= (lit("load") >> '(' >> '\"' >> lexeme[*(ascii::char_ - '\"')] >> '\"' >> ')');
		miscfunc = (functions[push_back(_val, _1)] >> '(' >> no_close[push_back(_val, _1)] >> ')');

		get_name %= raw[vecsym];
		no_close %= lexeme[*(ascii::char_ - ')')];

		variable = lexeme[ ascii::alpha[_val += _1] >> (*ascii::alnum[_val += _1]) ];

		expression = 
			//float_                 [_val = _1]; shortcut for faster compilation
			term                            [_val = _1]
			>> *(   ('+' >> term            [_val += _1])
				|   ('-' >> term            [_val -= _1])
				)
			;

		term =
			factor                          [_val = _1]
			>> *(   ('*' >> factor          [_val *= _1])
				|   ('/' >> factor          [_val /= _1])
				)
			;

		factor =
			float_                         [_val = _1]
			|   numsym                      [_val = _1]
			|   '(' >> expression           [_val = _1] >> ')'
			|   ('-' >> factor              [_val = -_1])
			|   ('+' >> factor              [_val = _1])
			;


		ivec %= '{' >> expression >> ',' >> expression >> ',' >> expression >> '}';
	
		vecExpression =
			vecTerm                            [_val = _1]
			>> *(   ('+' >> vecTerm            [_val += _1])
				  | ('-' >> vecTerm            [_val -= _1])
				)
			;

		vecTerm =
			vecFactor                          [_val = _1]
			>> *(   ('*' >> vecFactor          [_val *= _1])
				  | ('*' >> expression         [_val *= _1])
				  | ('/' >> vecFactor          [_val /= _1])
				  | ('/' >> expression         [_val /= _1])
				)
			;

		vecFactor =
			ivec                               [_val = construct<Vec3>(_1)]
			|   vecsym                         [_val = _1]
			|   '(' >> vecExpression           [_val = _1] >> ')'
			|   ('-' >> vecFactor              [_val = neg(_1)]) // unary
			|   ('+' >> vecFactor              [_val = _1])  // unary
			| (expression >> '*' >> vecFactor)[_val = _2 * _1] //left multiple
			;

		ivec2 %= '{' >> expression >> ',' >> expression >> '}';

		vec2Expression = 
			ivec2			[_val = construct<Vec2>(_1)]
			;

		ivec4 %= '{' >> expression >> ',' >> expression >> ',' >> expression >> ',' >> expression >> '}';

		vec4Expression = 
			ivec4			[_val = construct<Vec4>(_1)]
			;

	}

	void report_poly(const std::vector<std::string>& v)
	{
		if (m_verbose)
		{
			std::for_each(v.begin(), v.end(), std::cout << boost::lambda::_1 << ", ");
			std::cout << "\n";
		}
		polygons.push_back(v);
	}

// 	void report_mesh(const std::string& v)
// 	{
// 		meshs.push_back(v);
// 	}

	vector<vector<std::string> > polygons;
	vector<std::string> meshs;
	vector<vector<std::string> > invoked, invokedArgs;
	qi::symbols<char, symbol_ast> vecsym;
	qi::symbols<char, float> numsym;
	
	qi::symbols<char, std::string> functions; // possible general purpose no-arguments (yet) functions. added by the user

	qi::rule<Iterator, std::string(), ascii::space_type> no_close;
	qi::rule<Iterator, std::string(), ascii::space_type> get_name;
	qi::rule<Iterator, vector<std::string>(), ascii::space_type> addpoly;
	qi::rule<Iterator, vec_ast(), ascii::space_type>  vecExpression, vecTerm, vecFactor;
	qi::rule<Iterator, IVec3(), ascii::space_type> ivec;
	qi::rule<Iterator, void(), ascii::space_type> assignnum, assignvec, statement, program, comment;

	qi::rule<Iterator, std::string(), ascii::space_type> variable, loadmesh;
	qi::rule<Iterator, vector<std::string>(), ascii::space_type>  miscfunc;
	qi::rule<Iterator, float(), ascii::space_type> expression, term, factor;

	qi::rule<Iterator, IVec2(), ascii::space_type> ivec2;
	qi::rule<Iterator, Vec2(), ascii::space_type> vec2Expression;

	qi::rule<Iterator, IVec4(), ascii::space_type> ivec4;
	qi::rule<Iterator, Vec4(), ascii::space_type> vec4Expression;


	bool m_verbose;

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
				if (va != NULL)
				{
					pnts.push_back(va);
				}
			}

			(*adder)(pnts);
		}
	}

	virtual IPoint* lookupSymbol(const string& pname) 
	{
		symbol_ast* va = vecsym.find(pname);
		if (va != NULL) {
			va->used = true;
			return va;
		}
		return va;
	}

	virtual void addMeshes(StringAdder *adder)
	{
		for(vector<string>::iterator it = meshs.begin(); it != meshs.end(); ++it)
		{
			(*adder)(*it);
		}
		//std::for_each(meshs.begin(), meshs.end(), *adder);
	}

	virtual void callFuncs(MultiStringAdder *adder)
	{
		for(vector<vector<string> >::iterator it = invoked.begin(); it != invoked.end(); ++it)
		{
			(*adder)(*it);
		}
		//std::for_each(meshs.begin(), meshs.end(), *adder);
	}

	virtual void cacheVecs()
	{
		vecsym.for_each(boost::bind(&vec_ast::doCache, boost::arg<2>() ));
		//vecsym.for_each(bind(&vec_ast::doCache, _2 ));
	}

	virtual void printTree()
	{
		vecsym.for_each(boost::bind(&vec_ast::print, boost::arg<2>() ));
	}

	struct vec_adapter
	{
		vec_adapter(PointActor &actor) :m_actor(actor) {}
		void operator()(string& name, symbol_ast& v)
		{
			m_actor(name, v.cache, v.used, &v);
		}
		PointActor &m_actor;
	};
	virtual void foreachPoints(PointActor &actor)
	{
		vecsym.for_each(boost::bind<void>(vec_adapter(actor), boost::arg<1>(), boost::arg<2>() ));
	}



};


IPolyCreator* KwParser::creator() 
{ 
	return m_g.get();
}
void KwParser::addFunction(const string& s)
{
	m_knownFunctions.push_back(s);
}



bool KwParser::kparse(const char* iter, const char* end, bool verbose, ErrorActor* errorAct)
{	
	const char* begin = iter;
	m_g.reset(NULL); // detele the old one

	typedef kwprog<iterator_type> kwprog;
	m_kg = new kwprog(verbose);
	m_g.reset(m_kg);

	for(TFuncs::iterator it = m_knownFunctions.begin(); it != m_knownFunctions.end(); ++it)
		m_kg->functions.add(it->c_str(), *it);

	while(iter != end)
	{
		bool ok = phrase_parse(iter, end, *m_kg, ascii::space);
		if (!ok || iter != end)
		{ // consume till end of line and try again
			const char* estart = iter;
			if (verbose)
				std::cout << "Error" << ok << " " << (int)(iter - begin) << endl;
			while (iter != end && *iter != '\n')
				++iter;
			if (errorAct)
				(*errorAct)(estart - begin, iter - begin);
		}
	}

	m_kg->cacheVecs();

	if (verbose)
	{
		std::cout << "vecs:\n";
		m_kg->vecsym.for_each(std::cout << "  " << boost::lambda::_1 << '=' << boost::lambda::_2 << '\n');
		std::cout << "nums:\n";
		m_kg->numsym.for_each(std::cout << "  " << boost::lambda::_1 << '=' << boost::lambda::_2 << '\n');
	
		std::cout << "\n";
	}

	return true;
}

// it's useful for this to be an assignment if we want to use the symbol created in later rules
bool KwParser::kparseFloat(const char* iter, const char* end, const char* nameend, float& f)
{
	if (!isValid())
		return false; 

	const char* nameiter = iter;
	bool ok = phrase_parse(iter, end, m_kg->assignnum, ascii::space);
	if (ok && iter == end)
	{
		f = *(m_kg->numsym.find(std::string(nameiter, nameend) ));
		return true;
	}
	return false;
}

bool KwParser::kparseVec(const char* iter, const char* end, const char* nameend, IPoint*& p)
{
	if (!isValid())
		return false;

	const char* nameiter = iter;
	bool ok = phrase_parse(iter, end, m_kg->assignvec, ascii::space);
	if (ok && iter == end)
	{
		p = m_kg->vecsym.find( std::string(nameiter, nameend));
		return true;
	}
	return false;
}

// parses only the expression, not the assignment
bool KwParser::kparseVec2(const char* iter, const char* end, Vec2& p)
{
	if (!isValid())
		return false;

	bool ok = phrase_parse(iter, end, m_kg->vec2Expression, ascii::space, p);
	return (ok && iter == end);
}


// parses only the expression, not the assignment
bool KwParser::kparseVec4(const char* iter, const char* end, Vec4& p)
{
	if (!isValid())
		return false;

	bool ok = phrase_parse(iter, end, m_kg->vec4Expression, ascii::space, p);
	return (ok && iter == end);
}


void vec_ast::print()
{
	
}

