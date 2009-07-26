
#ifndef MyFramebufferOBJECT_H
#define MyFramebufferOBJECT_H

#include <QtOpenGL/qgl.h>
#include <QtGui/qpaintdevice.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE


class GlTexture;


class MyFramebufferObjectPrivate;

class  MyFramebufferObject : public QPaintDevice
{
    Q_DECLARE_PRIVATE(MyFramebufferObject)
public:
    enum Attachment {
        NoAttachment,
        CombinedDepthStencil,
        Depth
    };
	enum ETexFormat {
		FMT_RGBA,
		FMT_FLOAT
	};
	enum EFiltering {
		FI_NEAREST,
		FI_LINEAR
	};

    MyFramebufferObject(const QSize &size, GLenum target = GL_TEXTURE_2D);
    MyFramebufferObject(int width, int height, GLenum target = GL_TEXTURE_2D);

    MyFramebufferObject(const QSize &size, Attachment attachment,
                         GLenum target = GL_TEXTURE_2D, ETexFormat format = FMT_RGBA, int numSamp = 0, EFiltering filtering = FI_NEAREST);
    MyFramebufferObject(int width, int height, Attachment attachment,
                         GLenum target = GL_TEXTURE_2D, ETexFormat format = FMT_RGBA, int numSamp = 0, EFiltering filtering = FI_NEAREST);

    virtual ~MyFramebufferObject();

	uint getTextureObj();

    bool isValid() const;
    bool bind();
    bool release();
    const GlTexture* texture() const;
    QSize size() const;
    QImage toImage() const;
    Attachment attachment() const;


    QPaintEngine *paintEngine() const;
    GLuint handle() const;

    static bool hasOpenGLFramebufferObjects();

    void drawTexture(const QRectF &target, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);
    void drawTexture(const QPointF &point, GLuint textureId, GLenum textureTarget = GL_TEXTURE_2D);


protected:
    int metric(PaintDeviceMetric metric) const;
    int devType() const { return QInternal::FramebufferObject; }

private:
    Q_DISABLE_COPY(MyFramebufferObject)
    MyFramebufferObjectPrivate *d_ptr;
    friend class QGLDrawable;
};

QT_END_NAMESPACE

QT_END_HEADER
#endif // MyFramebufferOBJECT_H
