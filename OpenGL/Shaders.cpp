#include "Shaders.h"

#include <gl/glew.h>

#include "glGlob.h"


template<> 
void UniformParam::set(const float& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glUniform1f(m_uid, v);
}

template<> 
void UniformParam::set(const Vec& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glUniform3fv(m_uid, 1, v.v);
}

template<> 
void UniformParam::set(const Vec2& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glUniform2fv(m_uid, 1, v.v);
}

template<> 
void UniformParam::set(const Vec4& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glUniform4fv(m_uid, 1, v.v);
}

template<> 
void UniformParam::set(const int& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glUniform1i(m_uid, v);
}





template<> 
void AttribParam::set(const float& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glVertexAttrib1f(m_uid, v);
}

template<> 
void AttribParam::set(const Vec& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glVertexAttrib3fv(m_uid, v.v);
}

template<> 
void AttribParam::set(const Vec2& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glVertexAttrib2fv(m_uid, v.v);
}

template<> 
void AttribParam::set(const Vec4& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glVertexAttrib4fv(m_uid, v.v);
}

template<> 
void AttribParam::set(const int& v) const
{
	// when calling this, a program should be in use
	if (m_uid != -1)
		glVertexAttrib1s(m_uid, v); // EH?
}


