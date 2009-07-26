
#define Q_ASSERT(a)

#include <iostream>
#include <sstream>
#include <deque>
#include <map>
using namespace std;

#include "ExpParser.h"
#include "ShapeIFS.h"
#include <QString>

#pragma warning (disable: 4996) // function unsafe

class Cell
{
public:
	Cell() { m_cellReg.push_back(this); }
	virtual ~Cell() {}
	virtual string str() = 0;
	virtual char op() { return 0; }
	virtual float val() { return 0.0; }
	virtual string clval() { return string(); }
	virtual Vec* cval() { return NULL; }

	static void cleanup()
	{
		for(Expression::iterator it = m_cellReg.begin(); it != m_cellReg.end(); ++it)
			delete *it;
		m_cellReg.clear();
	}

private:
	static Expression m_cellReg;
};

Expression Cell::m_cellReg;

class OpCell : public Cell
{
public:
	OpCell(char opcode) :oper(opcode) {}
	virtual string str() { return string(&oper, 1); }
	virtual char op() { return oper; }
private:
	char oper;
};

class NumCell : public Cell
{
public:
	NumCell(float n) :num(n) {}
	virtual string str() { stringstream s; s << val(); return s.str(); }
	virtual float val() { return num; }
private:
	float num;
};

static TSymTable *g_symbols;

class SymCell : public Cell
{
public:
	SymCell(const string s) :sym(s), bColorRef(false) {}
	virtual string str() { return cval()->toString().toStdString(); } 
	virtual Vec* cval() { return bColorRef?(color()):(point()); } // change return value to something else
	virtual string clval() { return sym; }
	void setColorRef(bool v) { bColorRef = v; }
	Vec* point() { return &((*g_symbols)[sym].point); }
	Vec* color() { return &((*g_symbols)[sym].color); }
private:
	string sym;
	bool bColorRef;
};

class CoordCell : public Cell
{
public:
	CoordCell(const Vec& c) :coord(c) {}
	CoordCell(float a, float b, float c) :coord(a, b, c) {}
	virtual string str() { return cval()->toString().toStdString(); }
	virtual Vec* cval() { return &coord; }
private:
	Vec coord;
};



bool isOperator(char c)
{
	return (c == '+') || (c == '-') || (c == '*') || (c == '/') || (c == '=');
}
bool isFunc(char c)
{
	return (c >= 'A') && (c <= 'Z');
}
int opPriority(char c)
{
	switch (c)
	{
	case '=': return 1;
	case '+': case '-': return 2;
	case '*': case '/': return 3;
	default: return 0;
	}
}
int funcArgNum(char c)
{
	switch (c)
	{
	case 'C': return 3;
	case 'A': return 4;
	default: return 0;
	}
}
bool isLeftAss(char c)
{
	if (c == '=') return false;
	return true;
}

bool isDigit(char c)
{
	return (c >= '0') && (c <= '9');
}
bool isAlpha(char c)
{
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}
int hAbs(int a) 
{ 
	if (a < 0) return -a; return a; 
}
QString rParen(const Vec& c)
{
	return 	QString("{%1, %2, %3}").arg(c[0], 3, 'f', 2).arg(c[1], 3, 'f', 2).arg(c[2], 3, 'f', 2);
}

Cell *makeValue(string s)
{
	if (isDigit(s[0]) || (s[0] == '-'))
	{ // a number
		for (unsigned int i = 1; i < s.length(); ++i)
			if ((!isDigit(s[i])) && (s[i] != '.'))
				return NULL;
		return new NumCell(atof(s.c_str()));
	}
	if ((s == "add4") || (s == "add"))
		return new OpCell('A'); // add poly
	if (s == "add3")
		return new OpCell('T');
	if (s == "col")
		return new OpCell('U'); // colour
	return new SymCell(s);
}


bool checkMakeValue(bool& inOperand, string& value, Expression &eqn)
{
	if(inOperand)
	{//puts the current value to the equation
		Cell *v = makeValue(value);
		if (v == NULL)
			return false;
		eqn.push_back(v);
		inOperand = false;
		value.clear();
	}
	return true;
}
//start the real code here
//converts the string to an equation to make it easy to work on
char* ExpParser::Char2Eqn(char* text, Expression &eqn)
{
	string value;//value of the current operand

	bool inOperand = false;//flag if i'm in operand (for long numbers)
	bool LastIsOperator = false;//if the last thing is operator
	//char lastOperator = 0;

	deque<char> brackets;//stackfor checking brackets

	while (*text != 0)
	{//while the string hasn't reached its end
		if ((*text == '(') || (*text == '{'))
		{//save it to the stack to check
			if (!checkMakeValue(inOperand, value, eqn))
				return text - 1;
			brackets.push_front(*text);
			if (*text == '{')
				eqn.push_back(new OpCell('C')); // its a function
			eqn.push_back(new OpCell('('));
		}
		else if ((*text == ')') || (*text == '}'))
		{//check for the last bracket at the stack
			if (brackets.empty())//this is close bracket without open
				return text;
			if (hAbs(*text - brackets.front()) > 2)
				return text; // mismatch parens
			brackets.pop_front();
			if (!checkMakeValue(inOperand, value, eqn))
				return text - 1;

			if (LastIsOperator)//this is close bracket immediatly after an operator
				return text;
			eqn.push_back(new OpCell(')'));//at last append it to the quation
		}
		else if ((isOperator(*text) || (*text == ',')) && (!LastIsOperator))
		{//found an operator
			//	return text;
			if (!checkMakeValue(inOperand, value, eqn))
				return text - 1;

			LastIsOperator = true;
			eqn.push_back(new OpCell(*text));//adds it
		}
		else if (isDigit(*text) || (*text == '.') || (isAlpha(*text)) || (*text == '-'))
		{//this is a number
			value += (*text);
			inOperand = true;//if this is the first digit in the number
			LastIsOperator = false;
		}
		else if ((*text == '/') && (!eqn.empty()) && (eqn.back()->op() == '/'))// check for comment start
		{
			eqn.pop_back(); // rid of the first '/'
			break;
		}
		else if ((*text != ' ') && (*text != '\t')) // skip spaces
			return text; //syntax error
		++text;//moves to the next digit
	}

	if (inOperand)//there was a number i hadn't put it at the equation
	{
		Cell *v = makeValue(value);
		if (v == NULL)
			return text - 1;
		eqn.push_back(v);
	}
	else if (eqn.empty())
		return NULL;
	else if ((eqn.back()->op() != ')') && (eqn.back()->op() != '}'))//the last one wasnt' close bracket
		return text;
	if (!brackets.empty())//no open brackets without close
		return text;
	return NULL;//no error found
}


int stackPriority(Expression &s)
{//returns the priority of the element on the top 0 for an empty stack
	return s.empty()?0:opPriority(s.front()->op());
}

//next step is to change it into post fix
bool ExpParser::In2Post(Expression &in, Expression &post)
{
	Expression temp;// a temp stack for operators

	while (!in.empty())
	{//there are more terms
		Cell *cell = in.front();
		in.pop_front();

		char Operator = cell->op();

		if (Operator == 0)//this is number
		{
			post.push_back(cell);//add as is
		}
		else if (isFunc(Operator))
		{
			temp.push_front(cell);
		}
		else if (Operator == ',')
		{
			while (!temp.empty() && (temp.front()->op() != '('))
			{//pops to the open bracket
				post.push_back(temp.front());
				temp.pop_front();
			} 
			if (temp.empty())
				return false; // missing parens
		}
		else if (Operator == ')')
		{
			while (temp.front()->op() != '(')
			{//pops to the open bracket
				post.push_back(temp.front());
				temp.pop_front();
			}
			temp.pop_front(); // pop '('
			if (!temp.empty() && (isFunc(temp.front()->op())))
			{
				post.push_back(temp.front());
				temp.pop_front();
			}
			// we already checked that there are enough parens.
		}
		else if (Operator == '(')
		{
			temp.push_front(cell);
		}
		else
		{//need to pop some operaotrs before putting my one
			while ((!temp.empty()) && 
				( (isLeftAss(Operator) && (opPriority(Operator) <= stackPriority(temp))) ||
				  (!isLeftAss(Operator) && (opPriority(Operator) < stackPriority(temp))) ))
			{
				post.push_back(temp.front());
				temp.pop_front();
			}
			temp.push_front(cell);
		}
	}
	while (!temp.empty())
	{//pops the rest of operators
		post.push_back(temp.front());
		temp.pop_front();
	}
	return true;
}




Cell* ExpParser::Eval2(Cell* n1, Cell* op, Cell* n2)
{//evaluate 2 operands using the operator
	Vec *c1 = n1->cval(), *c2 = n2->cval();
	float v1 = n1->val(), v2 = n2->val();

	switch(op->op())
	{
	case '+': 
		if (c1 && c2) return new CoordCell(*c1 + *c2);
		if (c1 || c2) return NULL;
		return new NumCell(v1 + v2);
	case '-': 
		if (c1 && c2) return new CoordCell(*c1 - *c2);
		if (c1 || c2) return NULL;
		return new NumCell(v1 - v2);
	case '*': 
		if (c1 && c2) return NULL;
		if (c1) return new CoordCell(*c1 * v2);
		if (c2) return new CoordCell(v1 * *c2);
		return new NumCell(v1 * v2);
	case '/':
		if (c2) return NULL; // divisor can't be a coord
		if (v2 == 0) 
		{
			cout << "Can't divide by zero." << endl;
			return NULL;
		}
		if (c1) return new CoordCell(*c1 / v2);
		return new NumCell(v1 / v2);
	case '=': 
		if (n1->clval().size() == 0)
		{
			cout << "Not L-value." << endl;
			return NULL;
		}
		if (c2 == NULL)
			return NULL;
		*n1->cval() = *c2; // default color - white
		return n2;
	default:
		cout << "unknown operator: '" << op->op() << "'" << endl;
		return NULL;
	}

}

/*string ExpParser::getPtrSym(Vec* c)
{
	for(TSymTable::iterator it = m_symbols.begin(); it != m_symbols.end(); ++it)
		if (&it->second == c)
			return it->first;
	return "ERROR";
}*/

//Evaluate an equation in post fix
Cell* ExpParser::PostEval(Expression &post, bool bSymLoad)
{
	static TexAnchor constAncs[4];
	static bool ancsSet;
	if (!ancsSet)
	{
		constAncs[0] = TexAnchor(0,0); constAncs[1] = TexAnchor(1,0);
		constAncs[2] = TexAnchor(1,1); constAncs[3] = TexAnchor(0,1);
	}



	Expression temp;//a stack for operands

	while (!post.empty())
	{//there are more terms to evaluate
		Cell *cell = post.front();
		post.pop_front();
		char Operator = cell->op();

		if (Operator == 0)//if not operator
			temp.push_front(cell);//push it to the stack
		else if (Operator == 'C') 
		{
			Vec c;
			for (int i = 2; i >= 0; --i) // they come in reverse order
			{
				if (temp.empty())
					return NULL; // not enough arguments
				Cell *pc = temp.front();
				if (pc->cval() != NULL)
					return NULL; // Coord in Coord
				c[i] = pc->val();
				temp.pop_front();
			}
			temp.push_front(new CoordCell(c));
		}
		else if ((Operator == 'A') || (Operator == 'T'))
		{
			int n = (Operator == 'A')?4:3;
			SymCell *v[4] = {0}; // init to NULLs
			for (int i = n-1; i >= 0; --i) // they come in reverse order
			{
				if (temp.empty())
					return NULL; // not enough arguments
				Cell *pc = temp.front();
				if (pc->cval() == NULL)
					return NULL; // Coord in Coord
				temp.pop_front();
				v[i] = static_cast<SymCell*>(pc);
			}

			MyPolygon *poly = m_obj->AddPoly(v[0]->point(), v[1]->point(), v[2]->point(), (v[3] != NULL)?(v[3]->point()):NULL, constAncs);
			*m_decompile += (n == 4)?QString("add4("):QString("add3(");
			for(int i = 0; i < n; ++i)
			{
				DefPoint &def = (*g_symbols)[v[i]->clval()];
				def.used = true; // mark symbol as used
				poly->vtx[i]->col = def.color;

				*m_decompile += + v[i]->clval().c_str();
				if (i < n - 1) *m_decompile += ",";
			}
			*m_decompile += ")\n";

			temp.push_front(new NumCell(0));
		}
		else if (Operator == 'U') // color
		{
			// get paramtete
			if (temp.empty())
				return NULL; // not enough arguments
			Cell *pc = temp.front();
			// don't pop it
			if (pc->clval().size() == 0)
				return NULL; // not L-value (color ref)
			static_cast<SymCell*>(pc)->setColorRef(true);			
		}
		else
		{//evaluate the last two operands at the stack
			if (temp.empty())
				return NULL;
			Cell *n2 = temp.front();
			temp.pop_front();
			if (temp.empty())
				return NULL;
			Cell *n1 = temp.front();
			temp.pop_front();

			Cell *res;
			if ((cell->op() == '=') && (!bSymLoad)) // do not load symbol
				res = n2; // short circuit
			else
				res = Eval2(n1, cell, n2);

			if (res == NULL)
				return NULL;
			temp.push_front(res);
		}
	}
	if (temp.size() > 1)
		return NULL; 

	return temp.front();
}

QString ExpParser::defDecomp(const DefPoint &d)
{
	QString ret = QString(d.name.c_str()) + "=" + rParen(d.point) + "\n";
	if (Vec::exactEqual(d.color, Vec(1.0f, 1.0f, 1.0f)))
		ret += QString("col(") + d.name.c_str() + ")=" + rParen(d.color) + "\n";
	return ret;
}

bool ExpParser::parse(char* instr, bool bSymLoad, int &errstart, int &errcnt)
{
	g_symbols = m_symbols;

	errstart = -1; errcnt = -1;
	char *next, *str = strtok(instr, "\n");
	while (str != NULL)
	{
		Expression in, post;
		char *strerr = Char2Eqn(str, in);
		next = strtok(NULL, "\n");
		char *end = (next != NULL)?next:strchr(str, '\0');

		if(strerr)
		{//checks for error
			//gets the character no where there is error
			if (errstart == -1) { errstart = strerr - instr; errcnt = 1; }
		}
		else if (!in.empty())
		{
			if (!In2Post(in, post)) //change to post fix
			{
				if (errstart == -1) { errstart = str - instr; errcnt = end - str; }
			}
			else
			{
				Cell *res = PostEval(post, bSymLoad);
				if (res == NULL)
				{
					if (errstart == -1) { errstart = str - instr; errcnt = end - str; }
				}
			}
		}
		str = next;

	}

	Cell::cleanup();

	QString desym;

	for(TSymTable::iterator it = m_symbols->begin(); it != m_symbols->end(); ++it)
	{
		it->second.name = it->first;
		if (it->second.used)
		{
			desym += defDecomp(it->second);
		}
	}

	*m_decompile = desym + "\n" + *m_decompile;

	if ((errstart > 0) && (instr[errstart] == '\0')) // replaced by strtok
		--errstart;
	return (errstart == -1);
}

/*
void main(void)
{
	char str[100];//for holding the string
	char *strerr;//for syntax error


	gets(str);//reads string from user
	while (strlen(str) > 0)
	{
		Expression in,post;
		strerr = Char2Eqn(str, in);
		if(strerr)
		{//checks for error
			int errn=strerr-str+1;//gets the character no where there is error
			while(--errn)//print spaces till the error character
				cout << " ";
			cout << "\x18\n";//draw an up arrow
			cout << "Syntax error in character #" << strerr-str+1 << endl;//error message
		}
		else
		{
			if (!In2Post(in, post)) //change to post fix
				cout << "Parse error" << endl;
			else
			{
				Cell *res = PostEval(post);
				if (res == NULL)
					cout << "Evaluation Error" << endl;
				else
					cout << "-> " << res->str() << endl;
			}
		}
		gets(str);
	}

	cout << "done. symbols:" << endl;
	for(TSymTable::iterator it = m_symbols.begin(); it != m_symbols.end(); ++it)
		cout << "'" << it->first << "' = " << it->second << endl;
	cout << "polys:" <<endl;
	for(deque<XPoly>::iterator it = m_poly.begin(); it != m_poly.end(); ++it)
		cout << "(" << getPtrSym(it->a) << "," << getPtrSym(it->b) << "," << getPtrSym(it->c) << "," << getPtrSym(it->d) << ")" << endl;

	getch();
}
*/


/*
    * While there are tokens to be read:

        Read a token.

            * If the token is a number, then add it to the output queue.
            * If the token is a function token, then push it onto the stack.
            * If the token is a function argument separator (e.g., a comma):

                    * Until the topmost element of the stack is a left parenthesis, pop the element onto the output queue. If no left parentheses are encountered, either the separator was misplaced or parentheses were mismatched.

            * If the token is an operator, o1, then:

            1) while there is an operator, o2, at the top of the stack, and either

                        o1 is associative or left-associative and its precedence is less than or equal to that of o2, or
                        o1 is right-associative and its precedence is less than that of o2,

                pop o2 off the stack, onto the output queue;

            2) push o1 onto the operator stack.

            * If the token is a left parenthesis, then push it onto the stack.
            * If the token is a right parenthesis, then pop operators off the stack, onto the output queue, until the token at the top of the stack is a left parenthesis, at which point it is popped off the stack but not added to the output queue. At this point, if the token at the top of the stack is a function token, pop it too onto the output queue. If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.

    * When there are no more tokens to read, pop all the operators, if any, off the stack, add each to the output as it is popped out and exit. (These must only be operators; if a left parenthesis is popped, then there are mismatched parentheses.)
*/