
#ifndef _MYMATRIX_H__INCLUDED_
#define _MYMATRIX_H__INCLUDED_


class MyMatrix  
{

public:
	double *v;
	int rows; 	
	int cols; 

	void MultInto(MyMatrix &A, MyMatrix &B);
	void TransposeInto(MyMatrix &A);

	double scalarProdWith(MyMatrix &B);
	void scalarMult(double n);
	void CopyInto(MyMatrix &A);
	void SubInto(MyMatrix &A, MyMatrix &B);
	void AddInto(MyMatrix &A, MyMatrix &B);
	void InterpolateInto(MyMatrix &A, MyMatrix &B, double perc);

	void alloc();

	double *operator[](int row); 

	
	MyMatrix();
	MyMatrix(int inRows, int inCols); // make identity
	virtual ~MyMatrix();

};

#endif // _MYMATRIX_H__INCLUDED_
