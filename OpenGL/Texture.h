#pragma once
#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <gl/glew.h>
#include <QtGlobal>
#include <QSize>
#include "..\MyLib\Vec.h"

class QGLContext;
class QImage;

class GlTexture
{
public:
	GlTexture() 
		: m_obj(-1), m_target(-1), m_fromContext(NULL)
	{}
	GlTexture(uint target, const QSize& size) 
		: m_obj(-1), m_target(-1), m_fromContext(NULL)
	{
		init(target, size, 1);
	}
	GlTexture(uint target, const QSize& size, int depth) 
		: m_obj(-1), m_target(-1), m_fromContext(NULL)
	{
		init(target, size, depth);
	}
	GlTexture(const QGLContext* context, const QImage* img, uint target);

	~GlTexture();

	void init(uint target, const QSize& size, int depth, uint internal_format = GL_RGBA8, 
		      uint format = GL_RGBA, uint type = GL_UNSIGNED_BYTE, const void* ptr = NULL,
			  GLenum minFilter = GL_NEAREST, GLenum magFilter = GL_NEAREST, GLenum wrap = GL_CLAMP);
	void destroy();

	uint handle() const { return m_obj; }
	// means we're not going to delete this texture. better take care of it yourself.
	void detach() { m_obj = -1; }

	const Vec3& size() const { return m_size; }
	bool isValid() const { return m_obj != -1; }
	GLenum target() const { return m_target; }

	void bind() const;
	void unbind() const;

private:
	Q_DISABLE_COPY(GlTexture)
	uint m_obj;
	Vec3 m_size;
	uint m_target;
	QGLContext *m_fromContext; // if it came from Qt, need to delete via Qt
};


class RenderBuffer
{
public: 
	RenderBuffer() : m_obj(-1) {}
	RenderBuffer(const QSize& size, uint internal_format, int numSamp)
	{
		init(size, internal_format, numSamp);
	}
	~RenderBuffer();
	void init(const QSize& size, uint internal_format, int numSamp);
	void destroy();

	void bind() const;
	void unbind() const;

	uint handle() const { return m_obj; }
	bool isValid() const { return m_obj != -1; }

private:
	Q_DISABLE_COPY(RenderBuffer);
	uint m_obj;

};

#endif // TEXTURE_H_INCLUDED
