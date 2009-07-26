#pragma once
#ifndef SHADERPROGRAM_H_INCLUDED
#define SHADERPROGRAM_H_INCLUDED

#include <qtglobal>
#include <QVector>

#include "Texture.h"

enum EProgType
{
	PTYPE_NOLIMIT,
	PTYPE_POINTS,
	PTYPE_TRIANGLES
};


class ShaderProgram
{
public:
	ShaderProgram() : m_progId(0), m_type(PTYPE_NOLIMIT), m_isOk(false)
	{}
	virtual ~ShaderProgram() 
	{
		clear();
	}

	virtual void clear();
	
	//bool isCurrent() const { return g_current == this; }
	virtual int type() const { return m_type; }

	static const ShaderProgram* current() { return g_current; };
	static bool hasCurrent() { return g_current != NULL; }

	virtual void setPointSize(float size) const {}

	bool init();
	bool isOk() const { return m_isOk; }

	uint progId() const { return m_progId; }

	typedef QList<QString> TCodesList;
	virtual void getCodes() = 0;
	virtual void successLink() = 0;

protected: 
	void use() const;
	void unuse() const;

	static bool printShaderInfoLog(uint obj);
	static bool printProgramInfoLog(uint obj);

	uint m_progId;
	EProgType m_type;
	TCodesList m_vtxprog, m_geomprog, m_fragprog;
	bool m_isOk;
	QList<uint> m_createdShaders; // for later deletion.

	static int g_users; // global. can't use more than one program at a time
	static const ShaderProgram *g_current;

	friend class ProgramUser;
};


class ProgramUser
{
public:
	ProgramUser(const ShaderProgram *prog = NULL) : m_prog(prog) 
	{
		if (m_prog != NULL)
			m_prog->use();
	}
	void use(const ShaderProgram *prog)
	{
		if (prog == NULL)
		{
			dispose();
			return;
		}
		m_prog = prog;
		m_prog->use();
	}

	~ProgramUser()
	{
		if (m_prog != NULL)
			m_prog->unuse();
	}
	void dispose()
	{
		if (m_prog != NULL)
			m_prog->unuse();
		m_prog = NULL;
	}

private:
	const ShaderProgram *m_prog;
};

// add definitions to a shader source (#define)
class ShaderDefines
{
public:
	ShaderDefines()
	{
		m_source = QString("").toAscii();
	}
	void add(const QString& d)
	{
		m_source += QString("#define %1\n").arg(d);
	}
	const char* c_str() const 
	{ 
		return m_source.data(); 
	}
private:
	QByteArray m_source;
};



class AllShader;
typedef QVector<AllShader*> TShaderList;

class ShadersManager
{
public:
	ShadersManager();
	~ShadersManager();
	void init();

	AllShader* operator[](int p)
	{
		if (p >= m_prog.size())
			return NULL;
		return m_prog[p];
	}
	void append(AllShader* s)
	{
		m_prog.append(s);
	}

	void bindTexture(int index, const float* data, int size);

	void bindTexture(int index, const QVector<float>& dat)
	{
		bindTexture(index, dat.data(), dat.size());
	}

	TShaderList& lst() { return m_prog; }
	const GlTexture* tex(int index) const { return m_texEng[index]; }

private:
	TShaderList m_prog;
	QVector<GlTexture*> m_texEng; // map the index of the texture engine to the Texture bound.
};



#endif // SHADERPROGRAM_H_INCLUDED
