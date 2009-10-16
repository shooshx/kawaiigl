#include <QByteArray>
#include <stdio.h>
#include <gl/glew.h>

#include "ShaderProgram.h"
#include "Shaders.h"
#include "glGlob.h"



int ShaderProgram::g_users = 0;
const ShaderProgram *ShaderProgram::g_current = NULL;


bool ShaderProgram::printShaderInfoLog(uint obj)
{
	int infologLength = 0, charsWritten  = 0;
	QByteArray infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 1)
	{
		infoLog.resize(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog.data());
		printf("Shader Info Log:\n%s\n", infoLog.data());
	}
	int ret;
	glGetShaderiv(obj, GL_COMPILE_STATUS, &ret);
	if (!ret)
		printf("Shader compile failed\n");
	return ret;
}

bool ShaderProgram::printProgramInfoLog(uint obj)
{
	int infologLength = 0, charsWritten  = 0;
	QByteArray infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	if (infologLength > 1)
	{
		infoLog.resize(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog.data());
		printf("Program Info Log:\n%s\n", infoLog.data());
	}
	int ret;
	glGetProgramiv(obj, GL_LINK_STATUS, &ret);
	if (!ret)
		printf("Program compile failed\n");
	return ret;
}

void ShaderProgram::use() const
{
	if (g_users++ != 0)
		return;
	if (!isOk())
		return;
	glUseProgram(m_progId);
	g_current = this;
}
void ShaderProgram::unuse() const 
{
	if (--g_users != 0)
		return;
	glUseProgram(0);
	g_current = NULL;
}

void ShaderProgram::clear() 
{
	m_vtxprog.clear();
	m_fragprog.clear();
	m_geomprog.clear();

	glDeleteProgram(m_progId);
	foreach(uint so, m_createdShaders)
	{
		glDeleteShader(so);
	}
	m_createdShaders.clear();
	m_isOk = false; 
} 

bool ShaderProgram::init(const ProgCompileConf& conf)
{
	mglCheckErrorsC("clear errors");
	const char* name = typeid(this).name();

	m_progId = glCreateProgram();

	ShaderDefines defines;

	getCodes(); // populate the lists, set m_type.
	m_isOk = false;


	if (!m_geomprog.isEmpty())
	{ // has geometry shaders
		glProgramParameteriEXT(m_progId, GL_GEOMETRY_INPUT_TYPE_EXT, conf.geomInput);
		glProgramParameteriEXT(m_progId, GL_GEOMETRY_OUTPUT_TYPE_EXT, conf.geomOutput);
		glProgramParameteriEXT(m_progId, GL_GEOMETRY_VERTICES_OUT_EXT, conf.geomVtxCount);	
	}

	mglCheckErrorsC(QString("codes %1").arg(name));


	for (int i = 0; i < m_vtxprog.size(); ++i)
	{
		uint vso = glCreateShader(GL_VERTEX_SHADER);
		QByteArray ba = m_vtxprog[i].toAscii();
		const char *srcs[2] = { defines.c_str(), ba.data() };
		glShaderSource(vso, 2, srcs, NULL);
		glCompileShader(vso);
		glAttachShader(m_progId, vso);
		m_createdShaders.append(vso);
	}
	mglCheckErrorsC(QString("vtx %1").arg(name));


	for (int i = 0; i < m_geomprog.size(); ++i)
	{
		uint gso = glCreateShader(GL_GEOMETRY_SHADER_EXT);
		QByteArray ba = m_geomprog[i].toAscii();
		const char *srcs[2] = { defines.c_str(), ba.data() };
		glShaderSource(gso, 2, srcs, NULL);
		glCompileShader(gso);
		glAttachShader(m_progId, gso);
		m_createdShaders.append(gso);
	}
	mglCheckErrorsC(QString("geom %1").arg(name));


	for (int i = 0; i < m_fragprog.size(); ++i)
	{
		uint fso = glCreateShader(GL_FRAGMENT_SHADER);
		QByteArray ba = m_fragprog[i].toAscii();
		const char *srcs[2] = { defines.c_str(), ba.data() };
		glShaderSource(fso, 2, srcs, NULL);
		glCompileShader(fso);
		glAttachShader(m_progId, fso);
		m_createdShaders.append(fso);
	}
	mglCheckErrorsC(QString("frag %1").arg(name));


	if (m_createdShaders.size() == 0)
		return m_isOk;

	glLinkProgram(m_progId);
	mglCheckErrorsC(QString("link %1").arg(name));

	m_isOk = printProgramInfoLog(m_progId);
	if (m_isOk)
	{
		printf("Compiled OK %d\n", m_progId);
	}
	mglCheckErrorsC(QString("proginfo %1").arg(name));


	successLink();

	mglCheckErrorsC(QString("vars %1").arg(name));
	return m_isOk;

}



ShadersManager::ShadersManager()
{
	m_texEng.resize(1 + 2); // 0 slot is unoccupied.
	m_texEng.fill(NULL);
}

void ShadersManager::init()
{
//	AllShader::instantiate(this);
}

ShadersManager::~ShadersManager()
{
	foreach(GlTexture* tex, m_texEng)
	{
		delete tex;
	}
}

void ShadersManager::bindTexture(int index, const float* data, int size)
{
	if ((index <= 0) || (index > m_texEng.size()))
	{
		printf("ERROR! can't bind to 0\n");
		return;
	}
	glActiveTexture(GL_TEXTURE0 + index);
	delete m_texEng[index];

	int ds = size / 3;
	GlTexture *tex = new GlTexture;
	tex->init(GL_TEXTURE_1D, QSize(ds, 1), 1, GL_RGB32F_ARB, GL_RGB, GL_FLOAT, data);
	m_texEng[index] = tex;

	glActiveTexture(GL_TEXTURE0);
}

