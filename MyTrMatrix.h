
#ifndef _MYTRMATRIX_H__INCLUDED_
#define _MYTRMATRIX_H__INCLUDED_


#include "MyMatrix.h"

//#define X_AXIS 1
//#define Y_AXIS 2
//#define Z_AXIS 3

class MyTrMatrix : public MyMatrix  
{
public:
	void indent();
	void InvertInto(MyTrMatrix &A);

	void trans(double x, double y, double z);
	void scale(double sx, double sy, double sz);
	void perspective(double d, double a);

	MyTrMatrix();
	virtual ~MyTrMatrix();

};

#endif // _MYTRMATRIX_H__INCLUDED_
