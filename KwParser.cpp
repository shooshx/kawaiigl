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



#include <string>
#include <map>
#include "MyLib/Vec.h"
#include "ParserModel.h"


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

/*
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
*/

std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
    out << v.toString().toStdString();
    return out;
}

/*
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
*/



//-----------------------------------------------------------------------------------------------------

struct IMVec3;

template<typename T>
struct t_ast 
{
    t_ast() : expr(NULL)
    {}
    t_ast(const t_ast& a) : expr(a.expr)
    {}
    t_ast(const Vec3& expr)  : expr(new VecVal<T>(expr))
    {}
    t_ast(const float& expr) : expr(new FloatVal<T>(expr))
    {}
    t_ast(const IMVec3& v);

    t_ast& operator+=(const t_ast& rhs) {
        expr = new BinaryAdd<T>(this->expr, rhs.expr);
        return *this;
    }
    t_ast& operator-=(const t_ast& rhs) {
        expr = new BinarySub<T>(this->expr, rhs.expr);
        return *this;
    }
    t_ast& operator*=(const t_ast& rhs){
        expr = new BinaryMult<T>(this->expr, rhs.expr);
        return *this;
    }
    t_ast& operator/=(const t_ast& rhs) {
        expr = new BinaryDiv<T>(this->expr, rhs.expr);
        return *this;
    }
    t_ast& operator-() {
        expr = new UnaryNeg<T>(this->expr);
        return *this;
    }

    void print();

    // t_ask arguments need to be const since they are passed by value
    mutable Evalable<T> *expr;

};

typedef t_ast<Vec3> vec_ast;
typedef t_ast<float> float_ast;


template<typename T>
t_ast<T> operator*(const t_ast<T>& lhs, const t_ast<T>& rhs) // for left-hand multiplication
{
    t_ast<T> r;
    r.expr = new BinaryMult<T>(lhs.expr, rhs.expr);
    return r;
}

t_ast<Vec3> operator*(const t_ast<Vec3>& lhs, const t_ast<float>& rhs) 
{
    t_ast<Vec3> r;
    r.expr = new BinaryVFMult(lhs.expr, rhs.expr);
    return r;
}
t_ast<Vec3> operator*=(const t_ast<Vec3>& lhs, const t_ast<float>& rhs) 
{
    lhs.expr = new BinaryVFMult(lhs.expr, rhs.expr);
    return lhs;
}
t_ast<Vec3> operator/=(const t_ast<Vec3>& lhs, const t_ast<float>& rhs) 
{
    lhs.expr = new BinaryVFDiv(lhs.expr, rhs.expr); 
    return lhs;
}
//t_ast<Vec3> operator*(const t_ast<float>& lhs, const t_ast<Vec3>& rhs) 
//{
//    t_ast<Vec3> r;
//    r.expr = new BinaryMult<Vec3>(NULL, rhs.expr); // TBD-fix with correct t_ast
//    return r;
//}



struct IMVec3 {
    float_ast x,y,z;
};
BOOST_FUSION_ADAPT_STRUCT(
    IMVec3,
    (float_ast, x)
    (float_ast, y)
    (float_ast, z)
)

template<> t_ast<Vec3>::t_ast(const IMVec3& v) 
    : expr(new MakeVec3(v.x.expr, v.y.expr, v.z.expr))
{}


struct IMVec2 {
    float_ast x,y;
    operator Vec2() {
        return Vec2(x.expr->eval(), y.expr->eval());
    }
};
BOOST_FUSION_ADAPT_STRUCT(
    IMVec2,
    (float_ast, x)
    (float_ast, y)
)

struct IMVec4 {
    float_ast x,y,z,w;
    operator Vec4() {
        return Vec4(x.expr->eval(), y.expr->eval(), z.expr->eval(), w.expr->eval());
    }
};
BOOST_FUSION_ADAPT_STRUCT(
    IMVec4,
    (float_ast, x)
    (float_ast, y)
    (float_ast, z)
    (float_ast, w)
)



// see comment in calc2_ast.cpp of the spirit2 examples
/*template<typename T>
struct negate_expr
{
    template <typename RT>
    struct result { typedef RT type; };

    t_ast<T> operator()(t_ast<T>& expr) const  {
        t_ast<T> v;
        v.expr = new UnaryNeg<T>(expr.expr);
        return v;
    }
};

boost::phoenix::function<negate_expr<Vec3>> negVec3;
*/


float_ast lookup_float_sym(Model* model, const std::string& name)
{
    float_ast r;
    r.expr = new NumsymLookup(model, name);
    return r;
}


template<typename T>
std::ostream& operator<<(std::ostream& out, t_ast<T>& v)
{
    out << v.expr->eval();
    //out << v.cache;
    return out;
}



void add_sym_v(qi::symbols<char, vec_ast>& table, Model* model, const std::string& name, const vec_ast& value)
{
    //cout << "ADDV " << name << " : " << "..." << endl;
    const vec_ast* v = table.find(name);
    if (v != NULL) 
        table.remove(name.c_str());
    table.add(name.c_str(), value);

    model->add_point(name, value.expr);
}

void add_sym_f(qi::symbols<char, float_ast>& table, Model* model, const std::string& name, const float_ast& value)
{
    //cout << "ADDN " << name << " : " << value << endl;
    const float_ast* v = table.find(name);
    if (v != NULL) 
        table.remove(name.c_str()); 
    table.add(name.c_str(), value);

    model->m_numsym.set(name, value.expr);
}




template <typename Iterator>
struct kwprog : public qi::grammar<Iterator, void(), ascii::space_type>
{
    kwprog(bool verbose, Model* model) : kwprog::base_type(program), m_verbose(verbose), m_model(model)
    {

        program = *statement;
            
        statement = (assignvec | assignnum  // vector assignment takes precedence
            | addpoly[phoenix::bind(&kwprog::report_poly, this, _1)] 
            | loadmesh[push_back(phoenix::ref(m_model->meshs), _1)]
            | miscfunc[push_back(phoenix::ref(m_model->invoked), _1)]
            )
            >> *comment
            ; 

        comment = lexeme[ (lit('#') | lit("//")) >> *(ascii::char_ - eol) >> eol];

        //assignnum = (variable >> '=' >> expression)[bind(numsym.add, _1, _2)] ;
        assignnum = (variable >> '=' >> expression)[phoenix::bind(&add_sym_f, ref(numsym), m_model, _1, _2)] ;
        assignvec = (variable >> '=' >> vecExpression)[phoenix::bind(&add_sym_v, ref(vecsym), m_model, _1, _2)] ;

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
        get_numname %= raw[numsym];
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
            //|   numsym                     [_val = _1]
            |   get_numname                 [_val = phoenix::bind(&lookup_float_sym, m_model, _1) ]
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
            ivec                               [_val = construct<vec_ast>(_1)]
            |   vecsym                         [_val = _1]
            |   '(' >> vecExpression           [_val = _1] >> ')'
            |   ('-' >> vecFactor              [_val = -_1]) // unary
            |   ('+' >> vecFactor              [_val = _1])  // unary
           // |   get_numname             [_val = phoenix::bind(&lookup_float_sym, ref(numsym), _1) ] 
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
        m_model->polygons.push_back(v);
    }

// 	void report_mesh(const std::string& v)
// 	{
// 		meshs.push_back(v);
// 	}

    Model* m_model;


    qi::symbols<char, vec_ast> vecsym;
    qi::symbols<char, float_ast> numsym;
    
    qi::symbols<char, std::string> functions; // possible general purpose no-arguments (yet) functions. added by the user

    qi::rule<Iterator, std::string(), ascii::space_type> no_close;
    qi::rule<Iterator, std::string(), ascii::space_type> get_name;
    qi::rule<Iterator, std::string(), ascii::space_type> get_numname;
    qi::rule<Iterator, vector<std::string>(), ascii::space_type> addpoly;
    qi::rule<Iterator, vec_ast(), ascii::space_type>  vecExpression, vecTerm, vecFactor;
    qi::rule<Iterator, IMVec3(), ascii::space_type> ivec; // IVec3 - immediate Vec3
    qi::rule<Iterator, void(), ascii::space_type> assignnum, assignvec, statement, program, comment;

    qi::rule<Iterator, std::string(), ascii::space_type> variable, loadmesh;
    qi::rule<Iterator, vector<std::string>(), ascii::space_type>  miscfunc;
    qi::rule<Iterator, float_ast(), ascii::space_type> expression, term, factor; //<--

    qi::rule<Iterator, IMVec2(), ascii::space_type> ivec2;
    qi::rule<Iterator, Vec2(), ascii::space_type> vec2Expression;

    qi::rule<Iterator, IMVec4(), ascii::space_type> ivec4;
    qi::rule<Iterator, Vec4(), ascii::space_type> vec4Expression;


    bool m_verbose;



};


void KwParser::addFunction(const string& s)
{
    m_knownFunctions.push_back(s);
}

void KwParser::defineFloatSym(const std::string& s) 
{
    m_kg->numsym.add(s.c_str(), float_ast());
}

KwParser::KwParser() 
{
}

KwParser::~KwParser()
{}

bool KwParser::kparse(const char* iter, const char* end, bool verbose, ErrorActor* errorAct)
{	
    const char* begin = iter;

    m_modelOwn.reset(new Model);
    m_model = m_modelOwn.get();

    typedef kwprog<iterator_type> kwprog;
    m_kg.reset(new kwprog(verbose, m_model));


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

    m_model->cacheVecs();

    if (verbose)
    {
        std::cout << "vecs:\n";
        //m_kg->vecsym.for_each(std::cout << "  " << boost::lambda::_1 << '=' << boost::lambda::_2 << '\n');
        std::cout << "nums:\n";
        //m_kg->numsym.for_each(std::cout << "  " << boost::lambda::_1 << '=' << boost::lambda::_2 << '\n');
    
        std::cout << "\n";
    }

    return true;
}

 IPolyCreator* KwParser::creator() { 
     return m_model; 
 }

// it's useful for this to be an assignment if we want to use the symbol created in later rules
bool KwParser::kparseFloat(const char* iter, const char* end, const char* nameend, float& f)
{
    Evalable<float>* fa = kparseFloat(iter, end, nameend);
    if (fa == NULL)
        return false;
    f = fa->eval();
    return true;
}

Evalable<float>* KwParser::kparseFloat(const char* iter, const char* end, const char* nameend)
{
    if (!isValid())
        return NULL; 

    const char* nameiter = iter;
    bool ok = phrase_parse(iter, end, m_kg->assignnum, ascii::space);
    if (ok && iter == end)
    {
        Evalable<float> **fa = m_model->m_numsym.find(std::string(nameiter, nameend) );
        if (fa != NULL)
            return *fa;
    }
    return NULL;
}



bool KwParser::kparseVec(const char* iter, const char* end, const char* nameend, IPoint*& p)
{
    if (!isValid())
        return false;

    const char* nameiter = iter;
    bool ok = phrase_parse(iter, end, m_kg->assignvec, ascii::space);
    if (ok && iter == end)
    {
        p = m_model->lookupSymbol( std::string(nameiter, nameend));
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



