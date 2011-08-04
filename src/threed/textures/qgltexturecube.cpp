/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgltexturecube.h"
#include "qgltexture2d_p.h"
#include "qgltextureutils_p.h"
#include "qglpainter_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLTextureCube
    \brief The QGLTextureCube class represents a cube map texture object for GL painting operations.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::textures

    QGLTextureCube contains six QImage objects for each of the cube
    map faces and settings for texture filters, wrap modes, and mipmap
    generation.  When bind() is called, this information is uploaded to
    the GL server if it has changed since the last time bind() was called.

    Once a QGLTextureCube object is created, it can be bound to multiple
    GL contexts.  Internally, a separate texture identifier is created
    for each context.  This makes QGLTextureCube easier to use than
    raw GL texture identifiers because the application does not need
    to be as concerned with whether the texture identifier is valid
    in the current context.  The application merely calls bind() and
    QGLTextureCube will create a new texture identifier for the context
    if necessary.

    QGLTextureCube internally points to a reference-counted object that
    represents the current texture state.  If the QGLTextureCube is copied,
    the internal pointer is the same.  Modifications to one QGLTextureCube
    copy will affect all of the other copies in the system.

    The texture identifiers will be destroyed when the last QGLTextureCube
    reference is destroyed, or when a context is destroyed that contained a
    texture identifier that was created by QGLTextureCube.

    \sa QGLTexture2D
*/

/*!
    \enum QGLTextureCube::Face
    This enum defines the face of a cube map texture that is affected
    by a texture operation on QGLTextureCube instances.

    \value PositiveX The positive X face of the cube map.
    \value NegativeX The negative X face of the cube map.
    \value PositiveY The positive Y face of the cube map.
    \value NegativeY The negative Y face of the cube map.
    \value PositiveZ The positive Z face of the cube map.
    \value NegativeZ The negative Z face of the cube map.
*/

class QGLTextureCubePrivate : public QGLTexture2DPrivate
{
public:
    QGLTextureCubePrivate();
    ~QGLTextureCubePrivate();

    void bindImages(QGLTexture2DTextureInfo *info);

    QImage otherImages[5];
    uint changedFaces;
};

QGLTextureCubePrivate::QGLTextureCubePrivate()
{
    changedFaces = 0;
}

QGLTextureCubePrivate::~QGLTextureCubePrivate()
{
}

void QGLTextureCubePrivate::bindImages(QGLTexture2DTextureInfo *info)
{
    QSize scaledSize(size);
#if defined(QT_OPENGL_ES_2)
    if ((bindOptions & QGLContext::MipmapBindOption) ||
            horizontalWrap != QGL::ClampToEdge ||
            verticalWrap != QGL::ClampToEdge) {
        // ES 2.0 does not support NPOT textures when mipmaps are in use,
        // or if the wrap mode isn't ClampToEdge.
        scaledSize = QGL::nextPowerOfTwo(scaledSize);
    }
#endif

    // Handle the first face.
    if (!image.isNull())
        info->tex.uploadFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, image, scaledSize);
    else if (size.isValid())
        info->tex.createFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, scaledSize);

    // Handle the other faces.
    for (int face = 1; face < 6; ++face) {
        if (!otherImages[face - 1].isNull()) {
            info->tex.uploadFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                                 otherImages[face - 1], scaledSize);
        } else {
            info->tex.createFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, scaledSize);
        }
    }
}

/*!
    Constructs a null texture object.

    \sa isNull()
*/
QGLTextureCube::QGLTextureCube()
    : d_ptr(new QGLTextureCubePrivate())
{
}

/*!
    Destroys this texture object.  If this object is the last
    reference to the underlying GL texture, then the underlying
    GL texture will also be deleted.
*/
QGLTextureCube::~QGLTextureCube()
{
}

/*!
    Returns true if this texture object is null; that is, all image()
    values are null and textureId() is zero.
*/
bool QGLTextureCube::isNull() const
{
    // TODO
    Q_D(const QGLTextureCube);
    return !d->infos;
}

/*!
    Returns true if this texture has an alpha channel; false if the
    texture is fully opaque.
*/
bool QGLTextureCube::hasAlphaChannel() const
{
    Q_D(const QGLTextureCube);
    if (!d->image.isNull() && d->image.hasAlphaChannel())
        return true;
    for (int face = 0; face < 5; ++face) {
        if (!d->otherImages[face].isNull()) {
            if (d->otherImages[face].hasAlphaChannel())
                return true;
        }
    }
    QGLTexture2DTextureInfo *info = d->infos;
    if (info)
        return info->tex.hasAlpha();
    return false;
}

/*!
    Returns the size of this texture.  If the underlying OpenGL
    implementation requires texture sizes to be a power of two,
    then this function will return the next power of two equal
    to or greater than requestedSize()

    \sa setSize(), requestedSize()
*/
QSize QGLTextureCube::size() const
{
    Q_D(const QGLTextureCube);
    return d->size;
}

/*!
    Sets the size of this texture to \a value.  If the underlying
    OpenGL implementation requires texture sizes to be a power of
    two, then requestedSize() will be set to \a value, and the
    actual size will be set to the next power of two equal
    to or greater than \a value.  Otherwise both size() and
    requestedSize() will be set to \a value.

    \sa size(), requestedSize()
*/
void QGLTextureCube::setSize(const QSize& value)
{
    Q_D(QGLTextureCube);
    if (d->requestedSize == value)
        return;
    if (!(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0) &&
        !(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0))
        d->size = QGL::nextPowerOfTwo(value);
    else
        d->size = value;
    d->requestedSize = value;
    ++(d->imageGeneration);
}

/*!
    Returns the size that was previously set with setSize() before
    it was rounded to a power of two.

    \sa size(), setSize()
*/
QSize QGLTextureCube::requestedSize() const
{
    Q_D(const QGLTextureCube);
    return d->requestedSize;
}

/*!
    Returns the image that is currently associated with the specified
    \a face of this cube map texture.  The image may not have been
    uploaded into the GL server yet.  Uploads occur upon the next
    call to bind().

    \sa setImage()
*/
QImage QGLTextureCube::image(QGLTextureCube::Face face) const
{
    Q_D(const QGLTextureCube);
    if (uint(face) >= 6)
        return QImage();
    if (face == 0)
        return d->image;
    else
        return d->otherImages[face - 1];
}

/*!
    Sets the \a image that is associated with this texture on the
    specified \a face of the cube map.  The image will be uploaded
    into the GL server the next time bind() is called.

    If setSize() or setImage() has been called previously, then \a image
    will be scaled to size() when it is uploaded.

    If \a image is null, then this function is equivalent to clearImage().

    \sa image(), setSize(), copyImage()
*/
void QGLTextureCube::setImage
        (QGLTextureCube::Face face, const QImage& image)
{
    Q_D(QGLTextureCube);
    if (uint(face) >= 6)
        return;
    if (image.isNull()) {
        // Don't change the imageGeneration, because we aren't actually
        // changing the image in the GL server, only the client copy.
        if (face == 0)
            d->image = image;
        else
            d->otherImages[face - 1] = image;
    } else {
        if (!d->size.isValid())
            setSize(image.size());
        if (face == 0)
            d->image = image;
        else
            d->otherImages[face - 1] = image;
        ++(d->imageGeneration);
        d->changedFaces |= (1 << face);
    }
}

/*!
    Clears the image() that is associated with this texture on the
    specified \a face of the cube map.  The GL texture will retain
    its current value.  This can be used to release client-side memory
    that is no longer required once the image has been uploaded into
    the GL server.

    The following code will queue \c image to be uploaded as the
    positive X face of the cube map, immediately force it to
    be uploaded into the current GL context, and then clear the
    client copy:

    \code
    texture.setImage(QGLTextureCube::PositiveX, image);
    texture.bind();
    texture.clearImage(QGLTextureCube::PositiveX);
    \endcode

    \sa image(), setImage()
*/
void QGLTextureCube::clearImage(QGLTextureCube::Face face)
{
    Q_D(QGLTextureCube);
    if (face == 0)
        d->image = QImage();
    else
        d->otherImages[face - 1] = QImage();
}

/*!
    Copies the contents of \a image to \a offset in this texture
    within the current GL context.  The \a face parameter indicates
    which face of the cube map should be altered.

    Unlike setImage(), this function copies the image data to the
    GL server immediately using \c{glTexSubImage2D()}.  This is typically
    used to update the contents of a texture after it has been created.

    It is assumed that the application has already called bind() on
    this texture to bind it to the current GL context.

    If the texture has been created in multiple contexts, only the
    texture identifier for the current context will be updated.

    \sa setImage(), bind()
*/
void QGLTextureCube::copyImage
    (QGLTextureCube::Face face, const QImage& image, const QPoint& offset)
{
    if (uint(face) >= 6)
        return; // Invalid face number.
    QImage img = QGLWidget::convertToGLFormat(image);
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + int(face),
                    0, offset.x(), offset.y(),
                    img.width(), img.height(), GL_RGBA,
                    GL_UNSIGNED_BYTE, img.bits());
#if defined(QT_OPENGL_ES_2)
    Q_D(QGLTextureCube);
    if (d->bindOptions & QGLContext::MipmapBindOption)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
#endif
}

/*!
    Returns the options to use when binding the image() to an OpenGL
    context for the first time.  The default options are
    QGLContext::LinearFilteringBindOption |
    QGLContext::InvertedYBindOption | QGLContext::MipmapBindOption.

    \sa setBindOptions()
*/
QGLContext::BindOptions QGLTextureCube::bindOptions() const
{
    Q_D(const QGLTextureCube);
    return d->bindOptions;
}

/*!
    Sets the \a options to use when binding the image() to an
    OpenGL context.  If the image() has already been bound,
    then changing the options will cause it to be recreated
    from image() the next time bind() is called.

    \sa bindOptions(), bind()
*/
void QGLTextureCube::setBindOptions(QGLContext::BindOptions options)
{
    Q_D(QGLTextureCube);
    if (d->bindOptions != options) {
        d->bindOptions = options;
        ++(d->imageGeneration);
    }
}

/*!
    Returns the wrapping mode for horizontal texture co-ordinates.
    The default value is QGL::Repeat.

    \sa setHorizontalWrap(), verticalWrap()
*/
QGL::TextureWrap QGLTextureCube::horizontalWrap() const
{
    Q_D(const QGLTextureCube);
    return d->horizontalWrap;
}

/*!
    Sets the wrapping mode for horizontal texture co-ordinates to \a value.

    If \a value is not supported by the OpenGL implementation, it will be
    replaced with a value that is supported.  If the application desires a
    very specific \a value, it can call horizontalWrap() to check that
    the specific value was actually set.

    The \a value will not be applied to the texture in the GL
    server until the next call to bind().

    \sa horizontalWrap(), setVerticalWrap()
*/
void QGLTextureCube::setHorizontalWrap(QGL::TextureWrap value)
{
    Q_D(QGLTextureCube);
    value = qt_gl_modify_texture_wrap(value);
    if (d->horizontalWrap != value) {
        d->horizontalWrap = value;
        ++(d->parameterGeneration);
    }
}

/*!
    Returns the wrapping mode for vertical texture co-ordinates.
    The default value is QGL::Repeat.

    \sa setVerticalWrap(), horizontalWrap()
*/
QGL::TextureWrap QGLTextureCube::verticalWrap() const
{
    Q_D(const QGLTextureCube);
    return d->verticalWrap;
}

/*!
    Sets the wrapping mode for vertical texture co-ordinates to \a value.

    If \a value is not supported by the OpenGL implementation, it will be
    replaced with a value that is supported.  If the application desires a
    very specific \a value, it can call verticalWrap() to check that
    the specific value was actually set.

    The \a value will not be applied to the texture in the GL
    server until the next call to bind().

    \sa verticalWrap(), setHorizontalWrap()
*/
void QGLTextureCube::setVerticalWrap(QGL::TextureWrap value)
{
    Q_D(QGLTextureCube);
    value = qt_gl_modify_texture_wrap(value);
    if (d->verticalWrap != value) {
        d->verticalWrap = value;
        ++(d->parameterGeneration);
    }
}

/*!
    Binds this texture to the cube map texture target.

    If this texture object is not associated with an identifier in
    the current context, then a new identifier will be created,
    and the face images will be uploaded into the GL server.

    If setImage() or setSize() was called since the last upload,
    then the face images will be re-uploaded to the GL server.

    Returns false if the texture could not be bound for some reason.

    \sa release(), textureId(), setImage()
*/
bool QGLTextureCube::bind() const
{
    Q_D(const QGLTextureCube);
    return const_cast<QGLTextureCubePrivate *>(d)->bind(GL_TEXTURE_CUBE_MAP);
}

/*!
    Releases the texture associated with the cube map texture target.
    This is equivalent to \c{glBindTexture(GL_TEXTURE_CUBE_MAP, 0)}.

    \sa bind()
*/
void QGLTextureCube::release()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

/*!
    Returns the identifier associated with this texture object in
    the current context.

    Returns zero if the texture has not previously been bound to
    the 2D texture target in the current context with bind().

    \sa bind()
*/
GLuint QGLTextureCube::textureId() const
{
    Q_D(const QGLTextureCube);
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return 0;
    QGLTexture2DTextureInfo *info = d->infos;
    while (info != 0 && info->tex.context() != ctx)
        info = info->next;
    return info ? info->tex.textureId() : 0;
}

/*!
    Constructs a QGLTextureCube object that wraps the supplied literal
    texture identifier \a id, with the dimensions specified by \a size.

    The \a id is assumed to have been created by the application in
    the current GL context, and it will be destroyed by the application
    after the returned QGLTextureCube object is destroyed.

    This function is intended for interfacing to existing code that
    uses raw GL texture identifiers.  The returned QGLTextureCube can
    only be used with the current GL context.

    \sa textureId()
*/
QGLTextureCube *QGLTextureCube::fromTextureId(GLuint id, const QSize& size)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!id || !ctx)
        return 0;

    QGLTextureCube *texture = new QGLTextureCube();
    if (!size.isNull())
        texture->setSize(size);
    QGLTexture2DTextureInfo *info = new QGLTexture2DTextureInfo
        (ctx, id, texture->d_ptr->imageGeneration,
         texture->d_ptr->parameterGeneration, true);
    texture->d_ptr->infos = info;
    return texture;
}

QT_END_NAMESPACE
