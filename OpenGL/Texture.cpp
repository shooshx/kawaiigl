#include "Texture.h"

#include <QGLContext>

#include "glGlob.h"


void GlTexture::init(GLenum target, const QSize& size, int depth, GLenum internal_format, 
                   GLenum format, GLenum type, const void* ptr, GLenum minFilter, GLenum magFilter, GLenum wrap)
{
    if (m_obj != -1)
        destroy();

    m_target = target;
    glGenTextures(1, &m_obj);
    //printf(":%d\n", m_obj);
    glBindTexture(target, m_obj);
    mglCheckErrorsC("bind");
    if (target == GL_TEXTURE_1D)
    {
        glTexImage1D(target, 0, internal_format, size.width(), 0, format, type, ptr);
        mglCheckErrorsC("tex1d");
    }
    else if (target == GL_TEXTURE_2D)
    {
        // see http://www.opengl.org/wiki/Common_Mistakes     
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexImage2D(target, 0, internal_format, size.width(), size.height(), 0, format, type, ptr);
        mglCheckErrorsC("tex2d");
    }
    else if ((target == GL_TEXTURE_3D) || (target == GL_TEXTURE_2D_ARRAY_EXT))
    {
        glTexImage3D(target, 0, internal_format, size.width(), size.height(), depth, 0, format, type, ptr);
        mglCheckErrorsC("tex3d");
    }

/*    if (minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        minFilter == GL_LINEAR_MIPMAP_NEAREST || minFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    }*/

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);

    m_size = Vec3(size.width(), size.height(), depth);
}


GlTexture::GlTexture(const QGLContext* context, const QImage* img, uint target, GLenum wrapOpt)
{
    m_fromContext = const_cast<QGLContext*>(context);
    m_obj = m_fromContext->bindTexture(*img, target);
    m_target = target;
    m_size = Vec3(img->width(), img->height(), 1.0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    if (wrapOpt == 0)
        wrapOpt = GL_CLAMP_TO_EDGE;
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapOpt);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapOpt);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapOpt);
}

GlTexture::~GlTexture()
{
    unbind();
    destroy();
}

void GlTexture::destroy()
{
    if (m_fromContext != NULL)
        m_fromContext->deleteTexture(m_obj);
    else if (m_obj != -1)
        glDeleteTextures(1, &m_obj);
    m_obj = -1;
    m_size.clear();
    m_target = -1;
}

void GlTexture::bind() const
{
    if (m_obj == -1)
        return;
    glBindTexture(m_target, m_obj);
}

void GlTexture::unbind() const
{
    if (m_obj == -1)
        return;
    glBindTexture(m_target, 0);
}


/////////////////////////

void RenderBuffer::init(const QSize& size, uint internal_format, int numSamp)
{
    if (m_obj != -1)
        destroy();

    glGenRenderbuffersEXT(1, &m_obj);
    Q_ASSERT(!glIsRenderbufferEXT(m_obj));
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_obj);
    Q_ASSERT(glIsRenderbufferEXT(m_obj));
    
    glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, numSamp, internal_format, size.width(), size.height());
    mglCheckErrors("renderBuf");
}
RenderBuffer::~RenderBuffer()
{
    destroy();
}
void RenderBuffer::destroy()
{
    if (m_obj != -1)
        glDeleteRenderbuffersEXT(1, &m_obj);
    m_obj = -1;
}

void RenderBuffer::bind() const
{
    if (m_obj == -1)
        return;
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_obj);
}
void RenderBuffer::unbind() const
{
    if (m_obj == -1)
        return;
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}
