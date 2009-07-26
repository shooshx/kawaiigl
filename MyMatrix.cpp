
#include "GlobDefs.h"
#include "MyMatrix.h"
#include <assert.h>


MyMatrix::MyMatrix(): cols(-1), rows(-1), v(NULL)
{
}

void MyMatrix::alloc()
{
	v = (double*)malloc(rows * cols * sizeof(double));
	
	for(int i=0; i < rows * cols; ++i)
	{
		v[i] = 0;
	}
}

MyMatrix::MyMatrix(int inRows, int inCols): cols(inCols), rows(inRows) 
{
	alloc();
}

MyMatrix::~MyMatrix()
{
	if (v == NULL) return;
	free(v);
}

double *MyMatrix::operator[](int nrow)
{
	return &(v[nrow * cols]);
}


void MyMatrix::MultInto(MyMatrix &A, MyMatrix &B) // matrix multiply overload
{
	 _ASSERTE(A.cols == B.rows);
	 _ASSERTE((rows == A.rows)&&(cols == B.cols));
	int i,j,k;

	static MyMatrix Res(30, 30); 	// create temporary matrix of correct size for result !

	double sum = 0;
	for(i = 0; i < A.rows; ++i)
	{
		for(j = 0; j < B.cols; ++j) 
		{
			sum = 0; 
			for(k = 0; k < A.cols; ++k) 
			{
				sum += A[i][k] * B[k][j];		// use Overloaded Subscript ()
			}
			Res[i][j] = sum;
		}
	}
	for(i = 0; i < A.rows; ++i) 
	{
		for(j = 0; j < B.cols; ++j)
		{
			(*this)[i][j] = Res[i][j];
		}
	}
}

void MyMatrix::TransposeInto(MyMatrix &A)
{
	_ASSERTE((rows == A.cols) && (cols == A.rows));
	int i,j;

	static MyMatrix res(30,30);
	for(i = 0; i < A.rows; ++i)
	{
		for(j = 0; j < A.cols; ++j)
		{
			res[i][j] = A[j][i];
		}
	}

	for(i = 0; i < rows; ++i) 
	{
		for(j = 0; j < cols; ++j)
		{
			(*this)[i][j] = res[i][j];
		}
	}
}


void MyMatrix::scalarMult(double n)
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			(*this)[i][j] = n * (*this)[i][j];
		}
	}
}

void MyMatrix::CopyInto(MyMatrix &A)
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			(*this)[i][j] = A[i][j];
		}
	}
}

double MyMatrix::scalarProdWith(MyMatrix &B)
{
	double sum = 0.0;

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			sum += (*this)[i][j] * B[i][j];
		}
	}
	return sum;
}

void MyMatrix::SubInto(MyMatrix &A, MyMatrix &B)
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			(*this)[i][j] = A[i][j] - B[i][j];
		}
	}
}

void MyMatrix::AddInto(MyMatrix &A, MyMatrix &B)
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			(*this)[i][j] = A[i][j] + B[i][j];
		}
	}
}

void MyMatrix::InterpolateInto(MyMatrix &A, MyMatrix &B, double perc)
{
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			(*this)[i][j] = ((perc) * A[i][j]) + ((1 - perc) * B[i][j]);
		}
	}
}