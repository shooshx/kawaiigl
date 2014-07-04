#ifndef POINTSIZERENDERE_H__INCLUDEDE
#define POINTSIZERENDERE_H__INCLUDEDE

#include <qtglobal>
#include <QVector>

#include "Texture.h"
#include "ShaderProgram.h"
//#include "defs.h"
#include "MyLib/Vec.h"



class Control3D;



class ShaderParam 
{
public:
	ShaderParam(const QString& name) :m_uid(-1), m_name(name)
	{}
	virtual void getLocation(uint progId) = 0;
	QString& name() { return m_name; }

protected:
	uint m_uid;
	QString m_name;
};


class UniformParam : public ShaderParam
{
public:
	UniformParam(const QString& name) : ShaderParam(name) {}
	virtual void getLocation(uint progId);

	template<typename T>
	void set(const T& v) const;
};


class AttribParam : public ShaderParam
{
public:
	AttribParam(const QString& name) : ShaderParam(name) {}
	virtual void getLocation(uint progId);
	
	template<typename T>
	void set(const T& v) const;
};





class GenericShader : public ShaderProgram
{
public:
	GenericShader() {}

	TCodesList& vtxprogs() { return m_vtxprog; }
	TCodesList& fragprogs() { return m_fragprog; }
	TCodesList& geomprogs() { return m_geomprog; }

	virtual void clear() 
	{
		ShaderProgram::clear();
		foreach(ShaderParam* p, m_params)
			delete p;
		m_params.clear();
	}

	void addParam(ShaderParam* p, int index)
	{
		if (m_params.size() <= index)
			m_params.resize(index + 1);
		m_params[index] = p;
	}

	template<typename T>
	bool setUniform(const T& val, int index)
	{
		if (!isOk())
			return false;
		if (index >= m_params.size() || index < 0)
			return false;
		UniformParam* p = dynamic_cast<UniformParam*>(m_params[index]);
		if (p == NULL)
		{
			printf("Param %s is not a uniform\n", m_params[index]->name().toLatin1().data());
			return false;
		}
		p->set(val);
		return true;
	}

	template<typename T>
	bool setAttrib(const T& val, int index)
	{
		if (!isOk())
			return false;
		if (index >= m_params.size())
			return false;
		AttribParam* p = dynamic_cast<AttribParam*>(m_params[index]);
		if (p == NULL)
		{
			printf("Param %s is not an attribute\n", m_params[index]->name().toLatin1().data());
			return false;
		}
		p->set(val);
		return true;
	}


protected:
	virtual void getCodes() {}
	virtual void successLink() 
	{
		foreach(ShaderParam* p, m_params)
			p->getLocation(m_progId);
	}

private: 
	QVector<ShaderParam*> m_params; // memory leaks

};



#if 0
class FloatAttrib : public AttribParam
{
public:
	FloatAttrib(const char* name) : AttribParam(name) {}
	void set(float v) const
	{
		if (m_uid != -1)
			glVertexAttrib1f(m_uid, v);
	}
};


class Vec3Uniform : public UniformParam
{
public:
	Vec3Uniform(const char* name) : UniformParam(name) {}
	void set(const Vec3& p) const
	{
		if (m_uid != -1)
			glUniform3fv(m_uid, 1, p.v);
	}
};

class Vec2Uniform : public UniformParam
{
public:
	Vec2Uniform(const char* name) : UniformParam(name) {}
	void set(const VecS2& s) const
	{
		if (m_uid != -1)
			glUniform2fv(m_uid, 1, s.v);
	}
};

class IntUniform : public UniformParam
{
public:
	IntUniform(const char* name) : UniformParam(name) {}
	void set(int v) const
	{
		if (m_uid != -1)
			glUniform1i(m_uid, v);
	}
};

class Mat4ArrUniform : public UniformParam
{
public:
	Mat4ArrUniform(const char* name) : UniformParam(name) {}
	void set(float mats[6 * 16]) const
	{
		if (m_uid != -1)
			glUniformMatrix4fv(m_uid, 6, false, mats);
	}
};


// a 1d array of data sent to the shader
class Shader1DData
{
public:
	Shader1DData(ShadersManager* mgr, const char* datName, const char* sizeName) 
		: m_dat(datName), m_datSize(sizeName), m_mgr(mgr)
	{}

	void getLocations(int progId);
	// get an index in the ShaderManages which is also the index of the texture engine
	void setData(int index) const;

	IntUniform m_dat;
	IntUniform m_datSize;

	ShadersManager *m_mgr;
};
#endif

class SLNotifiable;


#if 0
class AllShader : public ShaderProgram
{
public:
	AllShader(ShadersManager* mgr = NULL) :
		m_mats("cubeRot"),
		m_psize("psize"),
		m_polyData(mgr, "polyData", "polySize"),
		m_sphPolyData(mgr, "sphPolyData", NULL),
		m_externP("external_p"),
		m_externS("external_s"),
		m_coneOrig("coneOrigin"),
		m_sheetData("sheet")
	{}


	void setProjMatrices(float mats[6 * 16]) const // an array with the 6 4x4 matrics
	{
		m_mats.set(mats);
	}
	void setPointSize(float size) const
	{
		m_psize.set(size);
	}
	void setExternalPoint(const Vec3& p) const
	{
		m_externP.set(p);
		VecS2 s(p);
		m_externS.set(s);
	}
	void setConeOrigin(const Vec3& p) const
	{
		m_coneOrig.set(p);
	}
	void setSheetData(int texind) const
	{
		m_sheetData.set(texind);
	}

	Shader1DData m_polyData;
	Shader1DData m_sphPolyData;

public:

	Mat4ArrUniform m_mats;

	FloatAttrib m_psize;

	Vec3Uniform m_externP;
	Vec2Uniform m_externS;

	Vec3Uniform m_coneOrig;
	IntUniform m_sheetData;

private:
	QList<SLNotifiable*> m_params;
public:
	void registerNotify(SLNotifiable* prm)
	{
		m_params.append(prm);
	}
	virtual void successLink();

	static void instantiate(ShadersManager* mgr);
};



class SLNotifiable // Successful Link Notifiable
{
public:
	SLNotifiable() {}
	SLNotifiable(AllShader* prog)
	{
		prog->registerNotify(this);
	}

	virtual void notifySL(AllShader* prog) = 0;
};


#endif

#endif // POINTSIZERENDERE_H__INCLUDEDE
