#ifndef __VEC_H_INCLUDED__
#define __VEC_H_INCLUDED__

#include <qtglobal>
#include <QString>
#include <QColor>
#include <QTextStream>
#include <QVector>
#include <QList>
#include <math.h>
#include <float.h>

#define PI (3.141592653589793)
#define TWOPI (2.0 * PI)
#define HALFPI (0.5 * PI)
#define SQRT_2 (1.414213562373)

#pragma warning (disable: 4100) // unreferences formal parameter


#define FLOAT_COMPARE_BITMASK (0xFFFF0000)

inline float degFromRad(float rad)
{
	return rad * 180.0 / PI;
}

inline float radFromDeg(float deg)
{
	return deg / 180.0 * PI;
}

inline QString strFloat(float f, int dec)
{
	return QString("%1").arg(f, 1, 'f', dec);
}
inline QString strFloat(float f)
{
	return QString("%1").arg(f, 1, 'f', 2);
}


struct IVec2
{
	float x, y;
	float* ptr() { return &x; }
};

// immediate Vec3 - for initialization using an init list. where is C++0x when you need it?
struct IVec
{
	float x, y, z;
	float* ptr() { return &x; }
};

struct IVec4
{
	float x, y, z, w;
	float* ptr() { return &x; }
};


class VecI;

// this is the ultimate vector class.
class Vec3
{
public:
	Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vec3(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b) {} // alphs is discarded

	Vec3(Qt::GlobalColor qc) 
	{
		initFromColor(qc);
	}
	Vec3(const QColor& c) 
	{
		initFromColor(c);
	}

	Vec3(const IVec& iv) : x(iv.x), y(iv.y), z(iv.z) {}

	Vec3(const Vec3& a, const Vec3& b) // from, to
	{
		*this = Vec3(b);
		*this -= a;
	}

	void initFromColor(const QColor& c); // need MAX_VEC

	static Vec3 fromRgb(uint c)
	{
		return Vec3( (float)(c & 0xFF) / 255.0,
		            (float)((c >> 8) & 0xFF) / 255.0,
			        (float)((c >> 16) & 0xFF) / 255.0);
	}

	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			float r, g, b;
		};
		float v[3];
	};

	Vec3& operator+=(const Vec3& a)
	{
		x += a.x; y += a.y; z += a.z;
		return *this;
	}
	Vec3& operator-=(const Vec3& a)
	{
		x -= a.x; y -= a.y; z -= a.z;
		return *this;
	}
	Vec3& operator+=(float v)
	{
		x += v; y += v; z += v;
		return *this;
	}
	Vec3& operator-=(float v)
	{
		x -= v; y -= v; z -= v;
		return *this;
	}
	Vec3& operator/=(float v)
	{
		x /= v; y /= v; z /= v;
		return *this;
	}
	Vec3& operator*=(float v)
	{
		x *= v; y *= v; z *= v;
		return *this;
	}
	Vec3& operator*=(const Vec3& b)
	{
		x *= b.x; y *= b.y; z *= b.z;
		return *this;
	}
	Vec3& operator/=(const Vec3& b)
	{
		x /= b.x; y /= b.y; z /= b.z;
		return *this;
	}

	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	void clear() 
	{ 
		x = 0.0; y = 0.0; z = 0.0; 
	}

	float length() const
	{
		return sqrt(x*x + y*y + z*z);
	}
	float lenSquare() const
	{
		return x*x + y*y + z*z;
	}

	static float length(const Vec3& from, const Vec3& to)
	{
		return from.to(to).length();
	}

	bool isZero() const
	{
		return ((x == 0.0f) && (y == 0.0f) && (z == 0.0f));
	}

	bool unitize()
	{
		float l = length();
		if (l == 0)
			return false;
		*this /= l;
		return true;
	}
	
	Vec3 unitized() const
	{
		Vec3 c = *this;
		c.unitize();
		return c;
	}
	void setLength(float length)
	{
		unitize();
		*this *= length;
	}

	float dotProd(const Vec3& b) const
	{
		return dotProd(*this, b);
	}

	static float dotProd(const Vec3& a, const Vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	Vec3 crossProd(const Vec3 &b) const
	{
		return crossProd(*this, b);
	}
	static Vec3 crossProd(const Vec3 &a, const Vec3& b)
	{
		return Vec3(a.y * b.z - a.z * b.y, 
				   a.z * b.x - a.x * b.z, 
				   a.x * b.y - a.y * b.x);
	}

	static float distance(const Vec3& a, const Vec3& b)
	{
		Vec3 t = b;
		t -= a;
		return t.length();
	}
	static float distSquare(const Vec3& a, const Vec3& b)
	{
		Vec3 t = b;
		t -= a;
		return t.lenSquare();
	}


	static Vec3 fromTo(const Vec3& from, const Vec3& to);
	Vec3 to(const Vec3& to) const;


	static Vec3 triangleNormal(const Vec3& a, const Vec3& b, const Vec3& c)
	{
		Vec3 ab = a.to(b);
		Vec3 ac = a.to(c);
		return crossProd(ab, ac);
	}

	static Vec3 average(const Vec3& a, const Vec3& b)
	{
		return Vec3((a.x + b.x)/2.0f, (a.y + b.y)/2.0f, (a.z + b.z)/2.0f);
	}

	// returns true if the angle between v0-to-v1 and v1-to-v2 is concave from the point of view of the origin
	static bool isConcave(const Vec3& v0, const Vec3& v1, const Vec3& v2)
	{
		Vec3 v01 = v0.to(v1);
		v01.unitize();
		Vec3 v12 = v1.to(v2);
		v12.unitize();
		Vec3 dir1 = v01.crossProd(v12);	
		float r = dir1.dotProd(v1);
		return (r < 0);
	}
	static bool isConcaveDelta(const Vec3& v01, const Vec3& v1, const Vec3& v12)
	{
		Vec3 dir1 = v01.crossProd(v12);	
		float r = dir1.dotProd(v1);
		return (r < 0);
	}

	float &operator[](int row) 
	{ 
		Q_ASSERT(row < 3); 
		return v[row]; 
	}
	const float &operator[](int row) const 
	{ 
		Q_ASSERT(row < 3); 
		return v[row]; 
	}
	float const* ptr() const { return &v[0]; }

	void pmin(const Vec3 &a)
	{
		x = qMin(x, a.x);
		y = qMin(y, a.y);
		z = qMin(z, a.z);
	}

	void pmax(const Vec3 &a)
	{
		x = qMax(x, a.x);
		y = qMax(y, a.y);
		z = qMax(z, a.z);
	}

	// 2d perpedicular
	Vec3 perp() const
	{
		return Vec3(y, -x, 0);
	}

	// vector must be unitiszed
	float angle() const
	{
		float td = length();
		float rad = acos(x / td);
		if (y > 0) // y axis is inverted of course.
			rad = -rad;
		return degFromRad(rad);
	}
	float angleRad() const
	{
		float td = length();
		float rad = acos(x / td);
		if (y > 0) // y axis is inverted of course.
			rad = -rad;
		return rad;
	}

	static Vec3 unitVec2D(float degAngle)
	{
		float rad = radFromDeg(degAngle);
		return Vec3(cos(rad), -sin(rad), 0.0);
	}

	// geodesic distance. a,b should be unitized
	static float geoDist(const Vec3& a, const Vec3& b)
	{
		return acos(qBound(-1.0f, dotProd(a, b), 1.0f)); 
	}

	// calculate the angle around point a in which point b is at
	// from point a to point b
	// answer is in degrees
	float angleTo(const Vec3& bp) const;

	QString toString() const
	{
		return QString().sprintf("(%1.2f, %1.2f, %1.2f)", x, y, z);
	}
	QString toStringNoParen() const
	{
		return QString().sprintf("%1.2f, %1.2f, %1.2f", x, y, z);
	}
	QString toStringF() const
	{
		return QString().sprintf("(%f, %f, %f)", x, y, z);
	}
	void fromStringF(const QString& str)
	{
		QChar c;
		QTextStream in(const_cast<QString*>(&str), QIODevice::ReadOnly);
		in >> c >> x >> c >> y >> c >> z;
	}
	static Vec3 fromString(const QString& str)
	{
		Vec3 v;
		v.fromStringF(str);
		return v;
	}

	static bool isVeryNear(const Vec3& a, const Vec3& b)
	{
		return Vec3::distance(a,b) < 0.0001;
	}
	bool isCloseTo(const Vec3& b, float epsilon) const
	{
		return qAbs(x - b.x) + qAbs(y - b.y) + qAbs(z - b.z) < epsilon;
	}
	static bool isCloseTo(const Vec3& a, const Vec3& b, float epsilon)
	{
		return qAbs(a.x - b.x) + qAbs(a.y - b.y) + qAbs(a.z - b.z) < epsilon;
	}
	static float manhattenDist(const Vec3& a, const Vec3& b)
	{
		return qAbs(a.x - b.x) + qAbs(a.y - b.y) + qAbs(a.z - b.z);
	}
	// axis parallel distance (to bounding box...)
	static float paraDist(const Vec3& a, const Vec3& b)
	{
		return qMin(qAbs(a.x - b.x), qMin(qAbs(a.y - b.y), qAbs(a.z - b.z)));
	}
	static bool exactEqual(const Vec3& a, const Vec3& b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	}
	static bool maskEqual(const Vec3& a, const Vec3& b)
	{
		return ((*(uint*)&a.x) & FLOAT_COMPARE_BITMASK) == ((*(uint*)&b.x) & FLOAT_COMPARE_BITMASK) &&
			((*(uint*)&a.y) & FLOAT_COMPARE_BITMASK) == ((*(uint*)&b.y) & FLOAT_COMPARE_BITMASK) &&
			((*(uint*)&a.z) & FLOAT_COMPARE_BITMASK) == ((*(uint*)&b.z) & FLOAT_COMPARE_BITMASK);
	}

	// if f==0 we get a, if f==1 we get b
	static Vec3 linearMix(const Vec3& a, const Vec3& b, float f);

	int size() const { return 3; }
	const char* charPtr() const { return (const char*)ptr(); };
	int byteSize() const { return size() * sizeof(float); }

	float red() const { return r; }
	float green() const { return g; }
	float blue() const { return b; }
	float alpha() const { return 1.0f; }

	uint toBgr() const 
	{ 
		return 0xFF000000 | ((int)(r * 255.0f) << 16) | ((int)(g * 255.0f) << 8) | ((int)(b * 255.0f));
	}
	uint toRgb() const 
	{ 
		return 0xFF000000 | ((int)(b * 255.0f) << 16) | ((int)(g * 255.0f) << 8) | ((int)(r * 255.0f));
	}
	QColor toColor() const 
	{
		if (!isValidColor())
			return QColor();
		return QColor(qBound(0.0f, r, 1.0f) * 255.0, 
			          qBound(0.0f, g, 1.0f) * 255.0, 
					  qBound(0.0f, b, 1.0f) * 255.0);
	}
	bool isValidColor() const
	{
		return (r != FLT_MAX);
	}

};

extern const Vec3 MAX_VEC, MIN_VEC, INVALID_COLOR;

inline void Vec3::initFromColor(const QColor& c)
{
	if (!c.isValid())
	{
		*this = MAX_VEC;
		return;
	}
	r = c.red() / 255.0f; g = c.green() / 255.0f; b = c.blue() / 255.0f;
}

inline Vec3 operator+(const Vec3& a, const Vec3& b)
{
	Vec3 res(a);
	res +=b;
	return res;
}
inline Vec3 operator-(const Vec3& a, const Vec3& b)
{
	Vec3 res(a);
	res -=b;
	return res;
}
inline Vec3 operator/(const Vec3& a, float v)
{
	Vec3 res(a);
	res /= v;
	return res;
}
inline Vec3 operator*(const Vec3& a, float v)
{
	Vec3 res(a);
	res *= v;
	return res;
}
inline Vec3 operator*(float v, const Vec3& a)
{
	Vec3 res(a);
	res *= v;
	return res;
}
inline Vec3 operator*(const Vec3& a, const Vec3& b) // member-wise multiplication
{
	Vec3 res(a);
	res *= b;
	return res;
}
inline Vec3 operator/(const Vec3& a, const Vec3& b) // member-wise multiplication
{
	Vec3 res(a);
	res /= b;
	return res;
}

// this doesn't work quite right near 0
// inline bool operator==(const Vec3& a, const Vec3& b)
// {
// 	return ((*(uint*)&a.x) & FLOAT_COMPARE_BITMASK) == ((*(uint*)&b.x) & FLOAT_COMPARE_BITMASK) &&
// 		((*(uint*)&a.y) & FLOAT_COMPARE_BITMASK) == ((*(uint*)&b.y) & FLOAT_COMPARE_BITMASK) &&
// 		((*(uint*)&a.z) & FLOAT_COMPARE_BITMASK) == ((*(uint*)&b.z) & FLOAT_COMPARE_BITMASK);
// 	//return ((a.x == b.x) && (a.y == b.y) && (a.z == b.z));
// }
// inline bool operator!=(const Vec3& a, const Vec3& b)
// {
// 	return !(a == b);
// 	//return ((a.x != b.x) || (a.y != b.y) || (a.z != b.z));
// }

/* not really correct
inline bool operator>(const Vec3& a, const Vec3& b)
{
	return (a.x > b.x) && (a.y > b.y) && (a.z > b.z);
}
inline bool operator>=(const Vec3& a, const Vec3& b)
{
	return (a.x >= b.x) && (a.y >= b.y) && (a.z >= b.z);
}
inline bool operator<(const Vec3& a, const Vec3& b)
{
	return (a.x < b.x) && (a.y < b.y) && (a.z < b.z);
}
inline bool operator<=(const Vec3& a, const Vec3& b)
{
	return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z);
}*/



inline float Vec3::angleTo(const Vec3& bp) const
{
	Vec3 t = bp - *this;
	return t.angle();
}


// this is outside because we need to have operator- defined.
inline Vec3 Vec3::fromTo(const Vec3& from, const Vec3& to)
{
	return to - from;
}

inline Vec3 Vec3::to(const Vec3& to) const
{
	return to - *this;
}

template<class T>
inline T linearMix(const T& a, const T& b, float f) 
{
	return (f*b + (1-f)*a);
}

inline Vec3 Vec3::linearMix(const Vec3& a, const Vec3& b, float f) 
{
	return (f*b + (1-f)*a);
}



typedef QVector<Vec3> TVecList;
typedef QVector<int> TIndexList;
typedef QList<uint> TUIndexList;

class Vec4
{
public:
	Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	explicit Vec4(const Vec3& a) : x(a.x), y(a.y), z(a.z), w(1.0f) {}
	Vec4(const Vec3& a, float w) : x(a.x), y(a.y), z(a.z), w(w) {}
	explicit Vec4(const QColor& c) 
	{
		initFromColor(c);
	}

	void initFromColor(const QColor& c);

	QString toStringNoParen() const
	{
		return QString().sprintf("%1.2f, %1.2f, %1.2f, %1.2f", x, y, z, w);
	}
	void fromStringF(const QString& str)
	{
		QChar c;
		QTextStream in(const_cast<QString*>(&str), QIODevice::ReadOnly);
		in >> c >> x >> c >> y >> c >> z >> c >> w;
	}
	static Vec4 fromString(const QString& str)
	{
		Vec4 v;
		v.fromStringF(str);
		return v;
	}

	union
	{
		struct
		{
			float x, y, z, w;
		};
		struct
		{
			float r, g, b, a;
		};
		float v[4];
	};

	Vec3 toVec() const 
	{ 
		return Vec3(x/w, y/w, z/w);
	}

	QColor toColor() const 
	{
		if (!isValidColor())
			return QColor();
		return QColor(qBound(0.0f, r, 1.0f) * 255.0, 
					  qBound(0.0f, g, 1.0f) * 255.0, 
				  	  qBound(0.0f, b, 1.0f) * 255.0,
					  qBound(0.0f, a, 1.0f) * 255.0);
	}
	bool isValidColor() const
	{
		return (r != FLT_MAX);
	}
};

inline void Vec4::initFromColor(const QColor& c)
{
	if (!c.isValid())
	{
		*this = Vec4(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		return;
	}
	r = c.red() / 255.0f; g = c.green() / 255.0f; b = c.blue() / 255.0f; a = c.alpha() / 255.0f;
}

class Vec2
{
public:
	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float _x, float _y) : x(_x), y(_y) {}
	explicit Vec2(const Vec3& a) : x(a.x), y(a.y) {}
	union
	{
		struct
		{
			float x, y;
		};
		float v[2];
	};

	Vec3 toVec(float z) 
	{ 
		return Vec3(x, y, z);
	}

	QString toStringNoParen() const
	{
		return QString().sprintf("%1.2f, %1.2f", x, y);
	}

	Vec2& operator+=(const Vec2& a)
	{
		x += a.x; y += a.y; 
		return *this;
	}
	Vec2& operator-=(const Vec2& a)
	{
		x -= a.x; y -= a.y;
		return *this;
	}
	Vec2& operator*=(const Vec2& a)
	{
		x *= a.x; y *= a.y;
		return *this;
	}
	Vec2& operator*=(float f)
	{
		x *= f; y *= f;
		return *this;
	}
	Vec2& operator/=(const Vec2& a)
	{
		x /= a.x; y /= a.y;
		return *this;
	}
	Vec2& operator/=(float f)
	{
		x /= f; y /= f;
		return *this;
	}
};

inline Vec2 operator+(const Vec2& a, const Vec2& b)
{
	Vec2 res(a);
	res +=b;
	return res;
}
inline Vec2 operator-(const Vec2& a, const Vec2& b)
{
	Vec2 res(a);
	res -=b;
	return res;
}
inline Vec2 operator/(const Vec2& a, float v)
{
	Vec2 res(a);
	res /= v;
	return res;
}
inline Vec2 operator*(const Vec2& a, float v)
{
	Vec2 res(a);
	res *= v;
	return res;
}
inline Vec2 operator*(float v, const Vec2& a)
{
	Vec2 res(a);
	res *= v;
	return res;
}
inline Vec2 operator*(const Vec2& a, const Vec2& b) // member-wise multiplication
{
	Vec2 res(a);
	res *= b;
	return res;
}
inline Vec2 operator/(const Vec2& a, const Vec2& b) // member-wise multiplication
{
	Vec2 res(a);
	res /= b;
	return res;
}



class Plane
{
public:
	// construct a plane from 3 points in space
	Plane(const Vec3& p0, const Vec3& p1, const Vec3& p2) :point(p0)
	{
		normal = Vec3::crossProd(p0.to(p1), p0.to(p2));
		normal.unitize();
		d = -Vec3::dotProd(normal, point);
	}
	Plane(const Vec3& _normal, const Vec3& _point) : normal(_normal), point(_point)
	{
		normal.unitize();
		d = -Vec3::dotProd(normal, point);
	}

	static bool isEqual(const Plane& a, const Plane& b)
	{
		return Vec3::isCloseTo(a.normal, b.normal, 0.0001f) && (qAbs(a.d - b.d) < 0.0001);
	}

	Vec3 normal;
	Vec3 point;
	float d;
	// the equation of the plane has (a,b,c)=normal, d in ax+by+cz+d=0
};




inline bool operator==(const Plane& a, const Plane& b)
{
	return Plane::isEqual(a, b);
}
uint qHash(const Plane& pl);

class VecUB
{
public:
	VecUB() : r(0), g(0), b(0) {}
	VecUB(uchar _r, uchar _g, uchar _b) : r(_r), g(_g), b(_b) {}
	explicit VecUB(uint _c) : c(_c) {}
	VecUB(Qt::GlobalColor qc) 
	{
		initFromColor(qc);
	}
	VecUB(const QColor& col) 
	{
		initFromColor(col);
	}
	VecUB(const Vec3& v)
	{
		r = qBound(0.0f, v.r * 255.0f, 255.0f);
		g = qBound(0.0f, v.g * 255.0f, 255.0f);
		b = qBound(0.0f, v.b * 255.0f, 255.0f);
	}

	void initFromColor(const QColor& col)
	{
		r = col.red(); g = col.green(); b = col.blue();
	}
	static VecUB fromRgb(uint _c)
	{
		return VecUB(_c);
	}

	union
	{
		struct
		{
			uchar x, y, z;
		};
		struct
		{
			uchar r, g, b;
		};
		uchar v[3];
		uint c;
	};

};

class Vec4UB
{
public:
	Vec4UB() : r(0), g(0), b(0), a(0) {}
	Vec4UB(uchar _r, uchar _g, uchar _b, uchar _a) : r(_r), g(_g), b(_b), a(_a) {}
	Vec4UB(Qt::GlobalColor qc) 
	{
		initFromColor(qc);
	}
	Vec4UB(const QColor& c) 
	{
		initFromColor(c);
	}
	Vec4UB(const Vec3& v) : r(v.r*255.0f), g(v.g*255.0f), b(v.b*255.0f), a(255)
	{
	}

	void initFromColor(const QColor& c)
	{
		r = c.red(); g = c.green(); b = c.blue(); a = c.alpha();
	}

	static Vec4UB fromRgba(uint c)
	{
		return Vec4UB((c & 0xFF), (c >> 8) & 0xFF, (c >> 16) & 0xFF, (c >> 24) & 0xFF);
	}

	QString toString() const
	{
		return QString("(%1, %2, %3)").arg(r).arg(g).arg(b); // no a
	}

	union
	{
		struct
		{
			uchar r, g, b, a;
		};
		uchar v[4];
	};

};

template<typename T>
class VecT
{
public:
	VecT() : p0(0), p1(0), p2(0) {}
	VecT(T _p0, T _p1, T _p2) : p0(_p0), p1(_p1), p2(_p2) {}
	VecT(const Vec3& v) : p0(v.x), p1(v.y), p2(v.z) {}

	union
	{
		struct
		{
			T p0, p1, p2;
		};
		struct 
		{
			T x, y, z;
		};
		T p[3];
	};

	bool isZero() const 
	{
		return (p0 == 0) && (p1 == 0) && (p2 == 0);
	}
};

template<typename T>
bool operator==(const VecT<T>& a, const VecT<T>& b)
{
	return (a.p0 == b.p0) && (a.p1 == b.p1) && (a.p2 == b.p2);
}

typedef VecT<uint> VecUI;

class VecI : public VecT<int>
{
public:
	VecI() : VecT<int>(-1, -1, -1) {}
	VecI(int _p0, int _p1, int _p2) : VecT<int>(qRound(_p0), qRound(_p1), qRound(_p2)) {}
	VecI(const Vec3& v) : VecT<int>(v) {}
};


class VecS2 // a point in a modified spherical surface (Noah Space)
{
public:
	VecS2() : phi(0.0f), phi2(0.0f) {}
	explicit VecS2(const Vec3& p)
	{
		phi = atan2(p.y, p.x); // phi y,x
		phi2 = atan2(p.z, p.x); // other phi z,x
	}

	union
	{
		struct  
		{
			float phi, phi2;
		};
		float v[2];
	};
		
};



enum EEAxis { Xaxis, Yaxis, Zaxis, XYaxis, XZaxis, YZaxis };

class FuncFloatVec
{
public:
	virtual float operator()(const Vec3& v) const = 0;
	virtual bool init() { return true; } // do any initializations. returns false if failed or can't.
	virtual bool isSlow() const { return false; }
};
class FuncToColor
{
public:
	virtual uint operator()(float v) const = 0;
	virtual void setMinMax(float minv, float maxv) {}
	VecUB rgb(float v) const
	{
		return VecUB::fromRgb(operator()(v));
	}
};

class FuncBoolVec
{
public:
	virtual bool operator()(const Vec3& v) const = 0;
	virtual bool init() { return true; } // do any initializations. returns false if failed or can't.
};

class FuncVecToRgb : public FuncFloatVec
{
public:
	float operator()(const Vec3& v) const
	{
		uint c = v.toRgb();
		return *(float*)&c;
	}
	static const FuncVecToRgb f;
};

class FuncConst : public FuncFloatVec
{
public:
	FuncConst(float c) : m_c(c) {}
	float operator()(const Vec3& v) const
	{
		return m_c;
	}
private:
	uint m_c;
};

class CastToColor : public FuncToColor
{
public:
	virtual uint operator()(float v) const
	{
		return *(uint*)&v;
	}

	static const CastToColor f;
};



extern const FuncVecToRgb funcVecToRgb;

class VecFuncs : public Vec3
{
public:
	VecFuncs() {}
	VecFuncs(float _x, float _y, float _z) : Vec3(_x, _y, _z) {}
	VecFuncs(const Vec3& v) : Vec3(v) {}
	operator Vec3&() { return *(Vec3*)this; }


	float x() const { return Vec3::x; }
	float y() const { return Vec3::y; }
	float z() const { return Vec3::z; }

};


#endif
