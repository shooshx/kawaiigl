

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
using namespace boost::spirit::qi;
using namespace boost::spirit::ascii;

using namespace boost::spirit::arg_names;
using namespace boost::phoenix;
using namespace std;

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace lambda = boost::lambda;

struct write_action
{
	template<typename T>
	void operator()(T const& i, unused_type, unused_type) const
	{
		std::cout << i << std::endl;
	}
};

struct null_action
{
	template<typename T>
	void operator()(T const& i, unused_type, unused_type) const
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

ostream& operator<<(ostream& out, const Vec& v)
{
	out << v.toString().toStdString();
	return out;
}


BOOST_FUSION_ADAPT_STRUCT(
	IVec,
	(float, x)
	(float, y)
	(float, z)
)




template<typename ValT>
void add_sym(symbols<char, ValT>& table, const string& name, ValT value)
{
	//cout << name << " : " << value << endl;
	table.add(name.c_str(), value);
}

struct addOp {};
struct subOp {};
struct multOp {};
struct divOp {};

//  The AST
template< typename Op> struct binary_op;
struct unary_op;


struct vec_ast : public IPoint
{
	typedef
		boost::variant<
		//nil // can't happen!
		Vec*
		, float
		, boost::recursive_wrapper<vec_ast>
		, boost::recursive_wrapper<binary_op<addOp> >
		, boost::recursive_wrapper<binary_op<subOp> >
		, boost::recursive_wrapper<binary_op<multOp> >
		, boost::recursive_wrapper<binary_op<divOp> >
		, boost::recursive_wrapper<unary_op>
		>
		type;

	vec_ast()
		: expr(new Vec(-1,-1,-1)), used(false) {}

// 	vec_ast(const vec_ast& va)
// 		: expr(vec_ast(expr)), used(false) 
// 	{}

	template <typename Expr>
	vec_ast(const Expr& expr)
		: expr(expr), used(false) 
	{}

	vec_ast(const Vec& v)
		: expr(new Vec(v)), used(false) 
	{}

	vec_ast& operator+=(vec_ast const& rhs);
	vec_ast& operator-=(vec_ast const& rhs);
	vec_ast& operator*=(vec_ast const& rhs);
	vec_ast& operator/=(vec_ast const& rhs);
	vec_ast& operator=(vec_ast const& rhs);

	void doCache();
	void print();
	virtual void setCoord(const Vec& v) 
	{
	//	cout << "setCoord\n";
// 		if (Vec** pvp = boost::get<Vec*>(&expr) )
// 			**pvp = v;
// 		else 
// 			expr = new Vec(v);

		expr = v;
	}
	virtual const Vec& getCoord() const
	{
		return cache;
	}

	type expr;
	Vec cache;
	bool used;
};


template< typename Op>
struct binary_op
{
	binary_op(vec_ast const& left, vec_ast const& right)
		: left(left), right(right) {}

	vec_ast left;
	vec_ast right;
};

struct unary_op
{
	unary_op(vec_ast const& subject)
		: subject(subject) {}

	vec_ast subject;
};

// needs to be after decleration of binary_op 
vec_ast& vec_ast::operator+=(vec_ast const& rhs)
{
	expr = binary_op<addOp>(*this, rhs);
	return *this;
}

vec_ast& vec_ast::operator-=(vec_ast const& rhs)
{
	expr = binary_op<subOp>(*this, rhs);
	return *this;
}

vec_ast& vec_ast::operator*=(vec_ast const& rhs)
{
	expr = binary_op<multOp>(*this, rhs);
	return *this;
}

vec_ast& vec_ast::operator/=(vec_ast const& rhs)
{
	expr = binary_op<divOp>(*this, rhs);
	return *this;
}



// see comment in calc2_ast.cpp of the spirit2 examples
struct negate_expr
{
	template <typename T>
	struct result { typedef T type; };

	vec_ast operator()(vec_ast const& expr) const
	{
		return vec_ast(unary_op(expr));
	}
};

boost::phoenix::function<negate_expr> neg;


struct eval_visitor  :  public boost::static_visitor<Vec>
{
	Vec operator()(Vec*& a) const
	{
		return *a;
	}

	Vec operator()(vec_ast& a) const
	{
		return boost::apply_visitor(eval_visitor(), a.expr);
	}

	Vec operator()(binary_op<addOp>& a) const
	{
		return boost::apply_visitor(eval_visitor(), a.left.expr) +
			   boost::apply_visitor(eval_visitor(), a.right.expr);
	}
	Vec operator()(binary_op<subOp>& a) const
	{
		return boost::apply_visitor(eval_visitor(), a.left.expr) -
			   boost::apply_visitor(eval_visitor(), a.right.expr);
	}
	Vec operator()(binary_op<multOp>& a) const
	{
		return boost::apply_visitor(eval_visitor(), a.left.expr) *
			   boost::apply_visitor(eval_visitor(), a.right.expr);
	}
	Vec operator()(binary_op<divOp>& a) const
	{
		return boost::apply_visitor(eval_visitor(), a.left.expr) /
			   boost::apply_visitor(eval_visitor(), a.right.expr);
	}

	Vec operator()(unary_op& a) const
	{
		return -boost::apply_visitor(eval_visitor(), a.subject.expr);
	}

	Vec operator()(float& a) const
	{
		return Vec(a, a, a); // simulate..
	}

};


void vec_ast::doCache()
{
	cache = boost::apply_visitor(eval_visitor(), expr);
}

ostream& operator<<(ostream& out, vec_ast& v)
{
	out << boost::apply_visitor(eval_visitor(), v.expr);
	//out << v.cache;
	return out;
}


template <typename Iterator>
struct kwprog : public grammar<Iterator, void(), space_type>, public IPolyCreator
{
	kwprog(bool verbose) : kwprog::base_type(program), m_verbose(verbose)
	{

		program = *statement;
			
		statement = assignvec | assignnum  // vector assignment takes precedence
			| addpoly[bind(&kwprog::report_poly, this, _1)]
			; 

		//assignnum = (variable >> '=' >> expression)[bind(numsym.add, _1, _2)] ;
		assignnum = (variable >> '=' >> expression)[bind(&add_sym<float>, ref(numsym), _1, _2)] ;
		assignvec = (variable >> '=' >> vecExpression)[bind(&add_sym<vec_ast>, ref(vecsym), _1, _2)] ;
		addpoly = 
			(lit("add(") >> get_name[push_back(_val, _1)] 
			>> ',' >> get_name[push_back(_val, _1)] 
			>> +(',' >> get_name[push_back(_val, _1)]) 
			>> ')') 
			;
		get_name %= raw[vecsym];

		variable = lexeme[ alpha[_val += _1] >> (*alnum[_val += _1]) ];

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
            ivec                               [_val = construct<Vec>(_1,_2,_3)]
		    |   vecsym                         [_val = _1]
            |   '(' >> vecExpression           [_val = _1] >> ')'
            |   ('-' >> vecFactor              [_val = neg(_1)]) // unary
            |   ('+' >> vecFactor              [_val = _1])  // unary
			| (expression >> '*' >> vecFactor)[_val = _2][_val *= _1] //left multiple
            ;

	}

	void report_poly(const vector<string>& v)
	{
		if (m_verbose)
		{
			std::for_each(v.begin(), v.end(), cout << boost::lambda::_1);
			cout << "\n";
		}
		polygons.push_back(v);
	}

	vector<vector<string> > polygons;
	symbols<char, vec_ast> vecsym;
	symbols<char, float> numsym;

	rule<Iterator, string(), space_type> get_name;
	rule<Iterator, vector<string>(), space_type> addpoly;
	rule<Iterator, vec_ast(), space_type>  vecExpression, vecTerm, vecFactor;
	rule<Iterator, IVec(), space_type> ivec;
	rule<Iterator, void(), space_type> assignnum, assignvec, statement, program;

	rule<Iterator, string(), space_type> variable;
	rule<Iterator, float(), space_type> expression, term, factor;
	bool m_verbose;

	virtual void createPolygons(PolyAdder *adder)
	{
		vector<Vec> pnts;
		for(size_t i = 0; i < polygons.size(); ++i)
		{
			vector<string> &pol = polygons[i];
			pnts.clear();
			for(size_t p = 0; p < pol.size(); ++p)
			{
				string& pname = pol[p];
				vec_ast* va = vecsym.lookup()->find(pname.begin(), pname.end());
				//Vec v = boost::apply_visitor(eval_visitor(), va->expr);
				pnts.push_back(va->cache);
				va->used = true;
			}

			(*adder)(pnts);
		}
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
		void operator()(string& name, vec_ast& v)
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



bool KwParser::kparse(const char* iter, const char* end, bool verbose)
{	
	typedef const char* iterator_type;
	const char* begin = iter;
	m_g.reset(NULL); // detele the old one

	typedef kwprog<iterator_type> kwprog;
	kwprog *g = new kwprog(verbose);
	m_g.reset(g);
	bool ret = phrase_parse(iter, end, *g, space);
	g->cacheVecs();

	if (verbose)
		g->vecsym.for_each(std::cout << boost::lambda::_1 << '=' << boost::lambda::_2 << ' ');


	if (ret)
		if (iter == end)
			printf("OK");
		else
			printf("Error, consumed %d/%d", iter - begin, end - begin);
	else
		printf("Error");
	printf("\n");
	return ret;
}



struct print_visitor  :  public boost::static_visitor<void>
{
	print_visitor(int spaces) :m_spaces(spaces) {}
	int m_spaces;
	
	void printSpaces() const
	{
		for(int i = 0; i < m_spaces; ++i)
			printf(" ");
	}

	void operator()(Vec*& a) const
	{
		printSpaces();
		printf("%s\n", a->toString().toAscii().data());
	}

	void operator()(vec_ast& a) const
	{
		printSpaces();
		printf("[]\n");
		boost::apply_visitor(print_visitor(m_spaces + 2), a.expr);
	}

	void operator()(binary_op<addOp>& a) const
	{
		printSpaces();
		printf("+\n");
		boost::apply_visitor(print_visitor(m_spaces + 2), a.left.expr);
		boost::apply_visitor(print_visitor(m_spaces + 2), a.right.expr);
	}
	void operator()(binary_op<subOp>& a) const
	{
		printSpaces();
		printf("-\n");
		boost::apply_visitor(print_visitor(m_spaces + 2), a.left.expr);
		boost::apply_visitor(print_visitor(m_spaces + 2), a.right.expr);
	}
	void operator()(binary_op<multOp>& a) const
	{
		printSpaces();
		printf("*\n");
		boost::apply_visitor(print_visitor(m_spaces + 2), a.left.expr);
		boost::apply_visitor(print_visitor(m_spaces + 2), a.right.expr);
	}
	void operator()(binary_op<divOp>& a) const
	{
		printSpaces();
		printf("/\n");
		boost::apply_visitor(print_visitor(m_spaces + 2), a.left.expr);
		boost::apply_visitor(print_visitor(m_spaces + 2), a.right.expr);
	}

	void operator()(unary_op& a) const
	{
		printSpaces();
		printf("-\n");
		boost::apply_visitor(print_visitor(m_spaces + 2), a.subject.expr);
	}

	void operator()(float& a) const
	{
		printSpaces();
		printf("%f\n", a);
	}

};

void vec_ast::print()
{
	boost::apply_visitor(print_visitor(0), expr);
}

