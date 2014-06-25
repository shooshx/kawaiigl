#include "Shaders.h"

#include <gl/glew.h>

#include "glGlob.h"

     
void mglActiveTexture(uint v) {
    if (glActiveTexture) // WinXP
        glActiveTexture(v);
}
void mglBindFramebuffer(uint a, uint b) {
    glBindFramebuffer(a, b);
}

void UniformParam::getLocation(uint progId)
{
    if (m_name.isEmpty())
        return;
    m_uid = glGetUniformLocation(progId, m_name.toLatin1().data());
    if (m_uid == -1)
        printf("WARNING: uniform '%s' location is -1!\n", m_name.toLatin1().data());
}
void AttribParam::getLocation(uint progId)
{
    if (m_name.isEmpty())
        return;
    m_uid = glGetAttribLocation(progId, m_name.toLatin1().data());
    if (m_uid == -1)
        printf("WARNING: attribute '%s' location is -1!\n", m_name.toLatin1().data());
}

template<> 
void UniformParam::set(const float& v) const
{
    // when calling this, a program should be in use
    if (m_uid != -1)
        glUniform1f(m_uid, v);
}

template<> 
void UniformParam::set(const Vec3& v) const
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
void AttribParam::set(const Vec3& v) const
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


