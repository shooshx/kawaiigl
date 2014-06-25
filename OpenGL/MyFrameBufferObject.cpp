
#include <gl/glew.h>
#include "Texture.h"
#include "MyFramebufferObject.h"
//#include "FastImage.h"

#include "glGlob.h"

#include <qdebug.h>

//#include <private/qgl_p.h>
//#include <private/qpaintengine_opengl_p.h>
//#include <qglframebufferobject.h>
//#include <qlibrary.h>
#include <qimage.h>

QT_BEGIN_NAMESPACE

// #define DEPTH_BUFFER
#define QGL_FUNC_CONTEXT QGLContext *ctx = d_ptr->ctx;

class MyFramebufferObjectPrivate
{
public:
    MyFramebufferObjectPrivate() : valid(false), ctx(0), fbo(0) {}

    void init(const QSize& sz, MyFramebufferObject::Attachment attachment,
              GLenum texture_target, MyFramebufferObject::ETexFormat format, int numSamp,
              MyFramebufferObject::EFiltering filtering);
    bool checkFramebufferStatus() const;

    GlTexture texture, depth_stencil_buffer;
    GLuint fbo;
    RenderBuffer renderBuf, depth_renderBuf;

    GLenum target;
    QSize size;
    uint valid : 1;
    MyFramebufferObject::Attachment fbo_attachment;
    QGLContext *ctx; // for Windows extension ptrs
};

bool MyFramebufferObjectPrivate::checkFramebufferStatus() const
{
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
    case GL_NO_ERROR:
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        return true;
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        qDebug("QGLFramebufferObject: Unsupported framebuffer format.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete attachment.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, missing attachment.");
        break;
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
    case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, duplicate attachment.");
        break;
#endif
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, attached images must have same dimensions.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, attached images must have same format.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, missing draw buffer.");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        qDebug("QGLFramebufferObject: Framebuffer incomplete, missing read buffer.");
        break;
    default:
        qDebug() <<"QGLFramebufferObject: An undefined error has occurred: "<< status;
        break;
    }
    return false;
}

// glRenderbufferStorageMultisampleEXT

void MyFramebufferObjectPrivate::init(const QSize &sz, MyFramebufferObject::Attachment attachment,
                                      GLenum texture_target, MyFramebufferObject::ETexFormat format, 
                                      int numSamp, MyFramebufferObject::EFiltering filtering)
{
    size = sz;
    target = texture_target;
    // texture dimensions

    //numSamp = 0; // this doesn't work yet.

    mglCheckErrorsC();

    glGenFramebuffersEXT(1, &fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    if (numSamp == 0)
    {
        uint filter = (filtering == MyFramebufferObject::FI_LINEAR)?GL_LINEAR:GL_NEAREST;
        // init texture
        if (format == MyFramebufferObject::FMT_RGBA)
            texture.init(texture_target, sz, 6, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, NULL, filter, filter);
        else if (format == MyFramebufferObject::FMT_FLOAT)
            texture.init(texture_target, sz, 6, GL_ALPHA32F_ARB, GL_ALPHA, GL_FLOAT, NULL, filter, filter);

        glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, texture.handle(), 0);
    }
    else // use a renderbuffer.
    {
        renderBuf.init(sz, GL_RGB, numSamp);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, renderBuf.handle());
    }


    valid = checkFramebufferStatus();

    if (attachment == MyFramebufferObject::CombinedDepthStencil) 
    {
        // depth and stencil buffer needs another extension
        if (texture_target == GL_TEXTURE_2D)
        {
            depth_stencil_buffer.init(GL_TEXTURE_2D, sz, 1, GL_DEPTH24_STENCIL8_EXT, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT);
            glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_stencil_buffer.handle(), 0);
            glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, depth_stencil_buffer.handle(), 0);
        } 
        else if (texture_target == GL_TEXTURE_3D)
        {
            depth_stencil_buffer.init(GL_TEXTURE_2D_ARRAY_EXT, sz, 6, GL_DEPTH24_STENCIL8_EXT, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT);
            glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_stencil_buffer.handle(), 0);
            glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, depth_stencil_buffer.handle(), 0);
        }
        Q_ASSERT(numSamp == 0);

        valid = checkFramebufferStatus();
    } 
    else if (attachment == MyFramebufferObject::Depth)
    {
        if (numSamp == 0)
        {
            if (texture_target == GL_TEXTURE_2D)
            {
                depth_stencil_buffer.init(GL_TEXTURE_2D, sz, 1, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
                glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_stencil_buffer.handle(), 0);
            }
            else if (texture_target == GL_TEXTURE_3D)
            {
                depth_stencil_buffer.init(GL_TEXTURE_2D_ARRAY_EXT, sz, 6, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
                glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_stencil_buffer.handle(), 0);
            }
        }
        else
        {
            Q_ASSERT(texture_target == GL_TEXTURE_2D);
            depth_renderBuf.init(sz, GL_DEPTH_COMPONENT, numSamp);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_renderBuf.handle());
        }
        valid = checkFramebufferStatus();
    } 
    else 
    {
        fbo_attachment = MyFramebufferObject::NoAttachment;
    }

    depth_renderBuf.unbind();
    fbo_attachment = attachment;

    glBindTexture(texture_target, 0);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    if (!valid) 
    {
        glDeleteFramebuffersEXT(1, &fbo);
    }
    mglCheckErrorsC("fbo");
}


MyFramebufferObject::MyFramebufferObject(const QSize &size, GLenum target)
    : d_ptr(new MyFramebufferObjectPrivate)
{
    Q_D(MyFramebufferObject);
    d->init(size, NoAttachment, target, FMT_RGBA, 0, FI_NEAREST);
}


MyFramebufferObject::MyFramebufferObject(int width, int height, GLenum target)
    : d_ptr(new MyFramebufferObjectPrivate)
{
    Q_D(MyFramebufferObject);
    d->init(QSize(width, height), NoAttachment, target, FMT_RGBA, 0, FI_NEAREST);
}


MyFramebufferObject::MyFramebufferObject(int width, int height, Attachment attachment,
                                         GLenum target, MyFramebufferObject::ETexFormat format, 
                                         int numSamp, EFiltering filtering)
    : d_ptr(new MyFramebufferObjectPrivate)
{
    Q_D(MyFramebufferObject);
    d->init(QSize(width, height), attachment, target, format, numSamp, filtering);
}


MyFramebufferObject::MyFramebufferObject(const QSize &size, Attachment attachment,
                                           GLenum target, MyFramebufferObject::ETexFormat format, 
                                           int numSamp, EFiltering filtering)
    : d_ptr(new MyFramebufferObjectPrivate)
{
    Q_D(MyFramebufferObject);
    d->init(size, attachment, target, format, numSamp, filtering);
}


MyFramebufferObject::~MyFramebufferObject()
{
    Q_D(MyFramebufferObject);
//    QGL_FUNC_CONTEXT;

    if (isValid() && (d->ctx == QGLContext::currentContext()
            /*|| qgl_share_reg()->checkSharing(d->ctx, QGLContext::currentContext())*/))
    {
        glDeleteFramebuffersEXT(1, &d->fbo);
    }
    delete d_ptr;
}



bool MyFramebufferObject::isValid() const
{
    Q_D(const MyFramebufferObject);
    return d->valid;
}



bool MyFramebufferObject::bind()
{
    if (!isValid())
        return false;
    Q_D(MyFramebufferObject);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, d->fbo);
    d->valid = d->checkFramebufferStatus();
    return d->valid;
}


bool MyFramebufferObject::release()
{
    if (!isValid())
        return false;
    Q_D(MyFramebufferObject);
    if (glBindFramebufferEXT) // WinXP
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    d->valid = d->checkFramebufferStatus();
    return d->valid;
}

void MyFramebufferObject::doBind(MyFramebufferObject* fbo)
{
    if (fbo != NULL)
        fbo->bind();
    else {
        if (glBindFramebufferEXT) // WinXP
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
}
void MyFramebufferObject::doRelease(MyFramebufferObject* fbo)
{
    if (fbo != NULL)
        fbo->release();
}


const GlTexture* MyFramebufferObject::texture() const
{
    Q_D(const MyFramebufferObject);
    return &d->texture;
}

#if 0
QImage MyFramebufferObject::toImage() const
{
    Q_D(const MyFramebufferObject);
    if (!d->valid)
        return QImage();

    const_cast<MyFramebufferObject *>(this)->bind();
    mglCheckErrors("bind");

    FastImage fi(d->size);
    glReadPixels(0, 0, d->size.width(), d->size.height(), GL_RGBA, GL_UNSIGNED_BYTE, fi.ptr());
    mglCheckErrors("read");

    const_cast<MyFramebufferObject *>(this)->release();
    mglCheckErrors("toImage");

    return fi.toImage();
}
#endif

QSize MyFramebufferObject::size() const
{
    Q_D(const MyFramebufferObject);
    return d->size;
}

/*!
    \fn QImage QGLFramebufferObject::toImage() const

    Returns the contents of this framebuffer object as a QImage.
*/
#if 0
QImage QGLFramebufferObject::toImage() const
{
    Q_D(const QGLFramebufferObject);
    if (!d->valid)
        return QImage();

    const_cast<QGLFramebufferObject *>(this)->bind();
    QImage::Format image_format = QImage::Format_RGB32;
    if (d->ctx->format().alpha())
        image_format = QImage::Format_ARGB32_Premultiplied;
    QImage img(d->size, image_format);
    int w = d->size.width();
    int h = d->size.height();
    // ### fix the read format so that we don't have to do all the byte swapping
    glReadPixels(0, 0, d->size.width(), d->size.height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
    // OpenGL gives RGBA; Qt wants ARGB
    uint *p = (uint*)img.bits();
    uint *end = p + w*h;
    if (1) {
        while (p < end) {
        uint a = *p << 24;
        *p = (*p >> 8) | a;
        p++;
        }
    } else {
        while (p < end) {
        *p = 0xFF000000 | (*p>>8);
        ++p;
        }
    }
    } else {
    // OpenGL gives ABGR (i.e. RGBA backwards); Qt wants ARGB
    img = img.rgbSwapped();
    }
    const_cast<QGLFramebufferObject *>(this)->release();
    return img.mirrored();
}
#endif

//Q_GLOBAL_STATIC(QOpenGLPaintEngine, qt_buffer_paintengine)

/*! \reimp */
QPaintEngine *MyFramebufferObject::paintEngine() const
{
    return NULL;//qt_buffer_paintengine();
}

/*!
    \fn bool QGLFramebufferObject::hasOpenGLFramebufferObjects()

    Returns true if the OpenGL \c{GL_EXT_framebuffer_object} extension
    is present on this system; otherwise returns false.
*/
bool MyFramebufferObject::hasOpenGLFramebufferObjects()
{
    QGLWidget dmy; // needed to detect and init the QGLExtensions object
    return true;//(QGLExtensions::glExtensions & QGLExtensions::FramebufferObject);
}

/*!
    \since 4.4

    Draws the given texture, \a textureId, to the given target rectangle,
    \a target, in OpenGL model space. The \a textureTarget should be a 2D
    texture target.

    The framebuffer object should be bound when calling this function.

    Equivalent to the corresponding QGLContext::drawTexture().
*/
void MyFramebufferObject::drawTexture(const QRectF &target, GLuint textureId, GLenum textureTarget)
{
    Q_D(MyFramebufferObject);
    d->ctx->drawTexture(target, textureId, textureTarget);
}

/*!
    \since 4.4

    Draws the given texture, \a textureId, at the given \a point in OpenGL
    model space. The \a textureTarget should be a 2D texture target.

    The framebuffer object should be bound when calling this function.

    Equivalent to the corresponding QGLContext::drawTexture().
*/
void MyFramebufferObject::drawTexture(const QPointF &point, GLuint textureId, GLenum textureTarget)
{
    Q_D(MyFramebufferObject);
    d->ctx->drawTexture(point, textureId, textureTarget);
}


extern int qt_defaultDpi();

/*! \reimp */
int MyFramebufferObject::metric(PaintDeviceMetric metric) const
{
    Q_D(const MyFramebufferObject);

    float dpmx = qt_defaultDpi()*100./2.54;
    float dpmy = qt_defaultDpi()*100./2.54;
    int w = d->size.width();
    int h = d->size.height();
    switch (metric) {
    case PdmWidth:
        return w;

    case PdmHeight:
        return h;

    case PdmWidthMM:
        return qRound(w * 1000 / dpmx);

    case PdmHeightMM:
        return qRound(h * 1000 / dpmy);

    case PdmNumColors:
        return 0;

    case PdmDepth:
        return 32;//d->depth;

    case PdmDpiX:
        return (int)(dpmx * 0.0254);

    case PdmDpiY:
        return (int)(dpmy * 0.0254);

    case PdmPhysicalDpiX:
        return (int)(dpmx * 0.0254);

    case PdmPhysicalDpiY:
        return (int)(dpmy * 0.0254);

    default:
        qWarning("QGLFramebufferObject::metric(), Unhandled metric type: %d.\n", metric);
        break;
    }
    return 0;
}

/*!
    \fn GLuint QGLFramebufferObject::handle() const

    Returns the GL framebuffer object handle for this framebuffer
    object (returned by the \c{glGenFrameBuffersEXT()} function). This
    handle can be used to attach new images or buffers to the
    framebuffer. The user is responsible for cleaning up and
    destroying these objects.
*/
GLuint MyFramebufferObject::handle() const
{
    Q_D(const MyFramebufferObject);
    return d->fbo;
}

/*! \fn int QGLFramebufferObject::devType() const

    \reimp
*/


/*!
    Returns the status of the depth and stencil buffers attached to
    this framebuffer object.
*/

MyFramebufferObject::Attachment MyFramebufferObject::attachment() const
{
    Q_D(const MyFramebufferObject);
    if (d->valid)
        return d->fbo_attachment;
    return NoAttachment;
}

QT_END_NAMESPACE
