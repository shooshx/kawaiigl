#pragma once
#ifndef TRMATRIX_H_INCLUDED
#define TRMATRIX_H_INCLUDED

#include "Vec.h"

class TrMatrix
{
public:
	TrMatrix();

	void ident();
	void multRight(const TrMatrix &b);
	void multLeft(const TrMatrix &a);
	Vec4 multVec(const Vec4& v) const;
	Vec multVec(const Vec& v) const;
	void fill(float f)
	{
		for(int i = 0; i < 16; ++i)
			m[i] = f;
	}

	void invert();

	//void translate(float x, float y, float z);
	void rotate(EEAxis axis, float degang);

	float& v(int col, int row) { return m[row + col*4]; }
	float v(int col, int row) const { return m[row + col*4]; }

	QString toStringF() const;
	void fromStringF(const QString& str);

	float m[16];
};


template<typename FT>
bool mesagluInvertMatrix(const FT m[16], FT invOut[16]);

template<typename FT>
void mesagluMultMatrixVec(const FT matrix[16], const FT in[4], FT out[4]);

template<typename FT>
void mesagluMultMatrices(const FT a[16], const FT b[16], FT r[16]);


#endif // TRMATRIX_H_INCLUDED
