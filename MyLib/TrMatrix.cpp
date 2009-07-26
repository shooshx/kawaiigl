
#include "TrMatrix.h"


TrMatrix::TrMatrix()
{
	for(int i = 0; i < 16; ++i)
		m[i] = 0.0f;

}


void TrMatrix::ident()
{
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			if (i == j) 
				v(i,j) = 1.0f;
			else 
				v(i,j) = 0.0f;
		}
	}
}

void TrMatrix::multRight(const TrMatrix &b)
{
	TrMatrix r;
	mesagluMultMatrices(m, b.m, r.m);
	*this = r;
}

void TrMatrix::multLeft(const TrMatrix &a)
{
	TrMatrix r;
	mesagluMultMatrices(a.m, m, r.m);
	*this = r;
}

Vec4 TrMatrix::multVec(const Vec4& v) const
{
	Vec4 r;
	mesagluMultMatrixVec(m, v.v, r.v);
	return r;
}

Vec TrMatrix::multVec(const Vec& v) const
{
	Vec4 v4(v);
	Vec4 r;
	mesagluMultMatrixVec(m, v4.v, r.v);
	return r.toVec();
}

void TrMatrix::invert()
{
	mesagluInvertMatrix(m, m);
}




void TrMatrix::rotate(EEAxis axis, float fi)
{
	fi = radFromDeg(fi);
	TrMatrix r;
	switch (axis)
	{
	case Xaxis:
		r.v(0,0) = 1;		r.v(0,1) = 0;			r.v(0,2) = 0;			r.v(0,3) = 0;  
		r.v(1,0) = 0;		r.v(1,1) = cos(fi);		r.v(1,2) = sin(fi);		r.v(1,3) = 0;  
		r.v(2,0) = 0;		r.v(2,1) = -sin(fi);	r.v(2,2) = cos(fi);		r.v(2,3) = 0;  
		r.v(3,0) = 0;		r.v(3,1) = 0;			r.v(3,2) = 0;			r.v(3,3) = 1;  
		break;
	case Yaxis:
		r.v(0,0) = cos(fi);	r.v(0,1) = 0;		r.v(0,2) = -sin(fi);	r.v(0,3) = 0;  
		r.v(1,0) = 0;		r.v(1,1) = 1;		r.v(1,2) = 0;			r.v(1,3) = 0;  
		r.v(2,0) = sin(fi);	r.v(2,1) = 0;		r.v(2,2) = cos(fi);		r.v(2,3) = 0;  
		r.v(3,0) = 0;		r.v(3,1) = 0;		r.v(3,2) = 0;			r.v(3,3) = 1;  
		break;
	case Zaxis:
		r.v(0,0) = cos(fi);		r.v(0,1) = sin(fi);		r.v(0,2) = 0;		r.v(0,3) = 0;  
		r.v(1,0) = -sin(fi);	r.v(1,1) = cos(fi);		r.v(1,2) = 0;		r.v(1,3) = 0;  
		r.v(2,0) = 0;			r.v(2,1) = 0;			r.v(2,2) = 1;		r.v(2,3) = 0;  
		r.v(3,0) = 0;			r.v(3,1) = 0;			r.v(3,2) = 0;		r.v(3,3) = 1;  
		break;
	}
	this->multRight(r);
}


QString TrMatrix::toStringF() const
{
	return QString().sprintf("(%f %f %f %f; %f %f %f %f; %f %f %f %f; %f %f %f %f",
		m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15]);

}
void TrMatrix::fromStringF(const QString& str)
{
	if (str.isEmpty())
	{
		ident();
		return;
	}
	QChar c;
	QTextStream in(const_cast<QString*>(&str), QIODevice::ReadOnly);
	in >> c >> m[0] >> m[1] >> m[2] >> m[3];
	in >> c >> m[4] >> m[5] >> m[6] >> m[7];
	in >> c >> m[8] >> m[9] >> m[10] >> m[11];
	in >> c >> m[12] >> m[13] >> m[14] >> m[15];
}

/*
** from mesa-glu code.
** Invert 4x4 matrix.
** Contributed by David Moore (See Mesa bug #6748)
*/
template<typename FT>
bool mesagluInvertMatrix(const FT m[16], FT invOut[16])
{
	double inv[16], det;
	int i;

	inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15]
	+ m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15]
	- m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15]
	+ m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14]
	- m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
	inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15]
	- m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15]
	+ m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15]
	- m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14]
	+ m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15]
	+ m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15]
	- m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15]
	+ m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14]
	- m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11]
	- m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11]
	+ m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11]
	- m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10]
	+ m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

	det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}

template bool mesagluInvertMatrix(const double m[16], double invOut[16]);
template bool mesagluInvertMatrix(const float m[16], float invOut[16]);

template<typename FT>
void mesagluMultMatrixVec(const FT matrix[16], const FT in[4], FT out[4])
{
	int i;

	for (i=0; i<4; i++) {
		out[i] = 
			in[0] * matrix[0*4+i] +
			in[1] * matrix[1*4+i] +
			in[2] * matrix[2*4+i] +
			in[3] * matrix[3*4+i];
	}
}

template void mesagluMultMatrixVec(const double matrix[16], const double in[4], double out[4]);
template void mesagluMultMatrixVec(const float matrix[16], const float in[4], float out[4]);

template<typename FT>
void mesagluMultMatrices(const FT a[16], const FT b[16], FT r[16])
{
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			r[i*4+j] = 
				a[i*4+0]*b[0*4+j] +
				a[i*4+1]*b[1*4+j] +
				a[i*4+2]*b[2*4+j] +
				a[i*4+3]*b[3*4+j];
		}
	}
}

template void mesagluMultMatrices(const double a[16], const double b[16], double r[16]);
template void mesagluMultMatrices(const float a[16], const float b[16], float r[16]);






#if 0
void MyTrMatrix::trans(double x, double y, double z)
{
	(*this)[0][0] = 1;		(*this)[0][1] = 0;		(*this)[0][2] = 0;		(*this)[0][3] = 0;  
	(*this)[1][0] = 0;		(*this)[1][1] = 1;		(*this)[1][2] = 0;		(*this)[1][3] = 0;  
	(*this)[2][0] = 0;		(*this)[2][1] = 0;		(*this)[2][2] = 1;		(*this)[2][3] = 0;  
	(*this)[3][0] = x;		(*this)[3][1] = y;		(*this)[3][2] = z;		(*this)[3][3] = 1;  
}

void MyTrMatrix::scale(double sx, double sy, double sz)
{
	(*this)[0][0] = sx;		(*this)[0][1] = 0;		(*this)[0][2] = 0;		(*this)[0][3] = 0;  
	(*this)[1][0] = 0;		(*this)[1][1] = sy;		(*this)[1][2] = 0;		(*this)[1][3] = 0;  
	(*this)[2][0] = 0;		(*this)[2][1] = 0;		(*this)[2][2] = sz;		(*this)[2][3] = 0;  
	(*this)[3][0] = 0;		(*this)[3][1] = 0;		(*this)[3][2] = 0;		(*this)[3][3] = 1;  
}

void MyTrMatrix::perspective(double d, double a)
{
	(*this)[0][0] = 1;		(*this)[0][1] = 0;		(*this)[0][2] = 0;			(*this)[0][3] = 0;  
	(*this)[1][0] = 0;		(*this)[1][1] = 1;		(*this)[1][2] = 0;			(*this)[1][3] = 0;  
	(*this)[2][0] = 0;		(*this)[2][1] = 0;		(*this)[2][2] = d/(d-a);	(*this)[2][3] = 1/d;  
	(*this)[3][0] = 0;		(*this)[3][1] = 0;		(*this)[3][2] = -a*d/(d-a);	(*this)[3][3] = 0;  
}
#endif

