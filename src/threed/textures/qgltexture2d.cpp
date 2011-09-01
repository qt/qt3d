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

#include "qgltexture2d.h"
#include "qgltexture2d_p.h"
#include "qgltextureutils_p.h"
#include "qglpainter_p.h"
#include "qglext_p.h"
#include "qabstractdownloadmanager.h"
#include "qdownloadmanager.h"
#include "qthreadeddownloadmanager.h"

#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLTexture2D
    \brief The QGLTexture2D class represents a 2D texture object for GL painting operations.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::textures

    QGLTexture2D contains a QImage and settings for texture filters,
    wrap modes, and mipmap generation.  When bind() is called, this
    information is uploaded to the GL server if it has changed since
    the last time bind() was called.

    Once a QGLTexture2D object is created, it can be bound to multiple
    GL contexts.  Internally, a separate texture identifier is created
    for each context.  This makes QGLTexture2D easier to use than
    raw GL texture identifiers because the application does not need
    to be as concerned with whether the texture identifier is valid
    in the current context.  The application merely calls bind() and
    QGLTexture2D will create a new texture identifier for the context
    if necessary.

    QGLTexture2D internally points to a reference-counted object that
    represents the current texture state.  If the QGLTexture2D is copied,
    the internal pointer is the same.  Modifications to one QGLTexture2D
    copy will affect all of the other copies in the system.

    The texture identifiers will be destroyed when the last QGLTexture2D
    reference is destroyed, or when a context is destroyed that contained a
    texture identifier that was created by QGLTexture2D.

    QGLTexture2D can also be used for uploading 1D textures into the
    GL server by specifying an image() with a height of 1.

    \sa QGLTextureCube
*/

QGLTexture2DPrivate::QGLTexture2DPrivate()
{
    horizontalWrap = QGL::Repeat;
    verticalWrap = QGL::Repeat;
    bindOptions = QGLContext::DefaultBindOption;
#if !defined(QT_OPENGL_ES)
    mipmapSupported = false;
    mipmapSupportedKnown = false;
#endif
    imageGeneration = 0;
    parameterGeneration = 0;
    infos = 0;
    downloadManager = 0;
}

QGLTexture2DPrivate::~QGLTexture2DPrivate()
{
    // Destroy the texture id's in the GL server in their original contexts.
    QGLTexture2DTextureInfo *current = infos;
    QGLTexture2DTextureInfo *next;
    const QGLContext *currentContext =
        const_cast<QGLContext *>(QGLContext::currentContext());
    const QGLContext *firstContext = currentContext;
    while (current != 0) {
        next = current->next;
        if (current->isLiteral)
            current->tex.clearId(); // Don't delete literal id's.
        delete current;
        current = next;
    }
    if (firstContext != currentContext) {
        if (firstContext)
            const_cast<QGLContext *>(firstContext)->makeCurrent();
        else if (currentContext)
            const_cast<QGLContext *>(currentContext)->doneCurrent();
    }
}

/*!
    Constructs a null texture object and attaches it to \a parent.

    \sa isNull()
*/
QGLTexture2D::QGLTexture2D(QObject *parent)
    : QObject(parent), d_ptr(new QGLTexture2DPrivate())
{
}

/*!
    Destroys this texture object.  If this object is the last
    reference to the underlying GL texture, then the underlying
    GL texture will also be deleted.
*/
QGLTexture2D::~QGLTexture2D()
{
}

/*!
    Returns true if this texture object is null; that is, image()
    is null and textureId() is zero.
*/
bool QGLTexture2D::isNull() const
{
    Q_D(const QGLTexture2D);
    return d->image.isNull() && !d->infos;
}

/*!
    Returns true if this texture has an alpha channel; false if the
    texture is fully opaque.
*/
bool QGLTexture2D::hasAlphaChannel() const
{
    Q_D(const QGLTexture2D);
    if (!d->image.isNull())
        return d->image.hasAlphaChannel();
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
QSize QGLTexture2D::size() const
{
    Q_D(const QGLTexture2D);
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
void QGLTexture2D::setSize(const QSize& value)
{
    Q_D(QGLTexture2D);
    if (d->requestedSize == value)
        return;
    if (!(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0)
        && !(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0))
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
QSize QGLTexture2D::requestedSize() const
{
    Q_D(const QGLTexture2D);
    return d->requestedSize;
}

/*!
    Returns the image that is currently associated with this texture.
    The image may not have been uploaded into the GL server yet.
    Uploads occur upon the next call to bind().

    \sa setImage()
*/
QImage QGLTexture2D::image() const
{
    Q_D(const QGLTexture2D);
    return d->image;
}

/*!
    Sets the \a image that is associated with this texture.  The image
    will be uploaded into the GL server the next time bind() is called.

    If setSize() or setImage() has been called previously, then \a image
    will be scaled to size() when it is uploaded.

    If \a image is null, then this function is equivalent to clearImage().

    \sa image(), setSize(), copyImage(), setPixmap()
*/
void QGLTexture2D::setImage(const QImage& image)
{
    Q_D(QGLTexture2D);
    d->compressedData = QByteArray(); // Clear compressed file data.
    if (image.isNull()) {
        // Don't change the imageGeneration, because we aren't actually
        // changing the image in the GL server, only the client copy.
        d->image = image;
    } else {
        if (image.size().isValid())
            setSize(image.size());
        d->image = image;
        ++(d->imageGeneration);
    }
}

/*!
    Sets the image that is associated with this texture to \a pixmap.

    This is a convenience that calls setImage() after converting
    \a pixmap into a QImage.  It may be more efficient on some
    platforms than the application calling QPixmap::toImage().

    \sa setImage()
*/
void QGLTexture2D::setPixmap(const QPixmap& pixmap)
{
    QImage image = pixmap.toImage();
    if (pixmap.depth() == 16 && !image.hasAlphaChannel()) {
        // If the system depth is 16 and the pixmap doesn't have an alpha channel
        // then we convert it to RGB16 in the hope that it gets uploaded as a 16
        // bit texture which is much faster to access than a 32-bit one.
        image = image.convertToFormat(QImage::Format_RGB16);
    }
    setImage(image);
}

/*!
    Clears the image() that is associated with this texture, but the
    GL texture will retain its current value.  This can be used to
    release client-side memory that is no longer required once the
    image has been uploaded into the GL server.

    The following code will queue \c image to be uploaded, immediately
    force it to be uploaded into the current GL context, and then
    clear the client copy:

    \code
    texture.setImage(image);
    texture.bind();
    texture.clearImage()
    \endcode

    \sa image(), setImage()
*/
void QGLTexture2D::clearImage()
{
    Q_D(QGLTexture2D);
    d->image = QImage();
}

#ifndef GL_GENERATE_MIPMAP_SGIS
#define GL_GENERATE_MIPMAP_SGIS       0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS  0x8192
#endif

/*!
    Sets this texture to the contents of a compressed image file
    at \a path.  Returns true if the file exists and has a supported
    compressed format; false otherwise.

    The DDS, ETC1, PVRTC2, and PVRTC4 compression formats are
    supported, assuming that the GL implementation has the
    appropriate extension.

    \sa setImage(), setSize()
*/
bool QGLTexture2D::setCompressedFile(const QString &path)
{
    Q_D(QGLTexture2D);
    d->image = QImage();
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
        qWarning("QGLTexture2D::setCompressedFile(%s): File could not be read",
                 qPrintable(path));
        return false;
    }
    QByteArray data = f.readAll();
    f.close();

    bool hasAlpha, isFlipped;
    if (!QGLBoundTexture::canBindCompressedTexture
            (data.constData(), data.size(), 0, &hasAlpha, &isFlipped)) {
        qWarning("QGLTexture2D::setCompressedFile(%s): Format is not supported",
                 path.toLocal8Bit().constData());
        return false;
    }

    QFileInfo fi(path);
    d->url = QUrl::fromLocalFile(fi.absoluteFilePath());

    // The 3DS loader expects the flip state to be set before bind().
    if (isFlipped)
        d->bindOptions &= ~QGLContext::InvertedYBindOption;
    else
        d->bindOptions |= QGLContext::InvertedYBindOption;

    d->compressedData = data;
    ++(d->imageGeneration);
    return true;
}

/*!
    Returns the url that was last set with setUrl.
*/
QUrl QGLTexture2D::url() const
{
    Q_D(const QGLTexture2D);
    return d->url;
}

/*!
    Sets this texture to have the contents of the image stored at \a url.

    If the environment variable QT3D_MULTITHREAD is defined, network urls
    (http, etc) are downloaded in a separate download thread, otherwise they are
    downloaded asynchronously in the current thread.
*/
void QGLTexture2D::setUrl(const QUrl &url)
{
    Q_D(QGLTexture2D);
    if (d->url == url)
        return;
    d->url = url;

    if (url.isEmpty())
    {
        d->image = QImage();
    }
    else
    {
        if (url.scheme() == QLatin1String("file") || url.scheme().toLower() == QLatin1String("qrc"))
        {
            QString fileName = url.toLocalFile();

            // slight hack since there doesn't appear to be a QUrl::toResourcePath() function
            // to convert qrc:///foo into :/foo
            if (url.scheme().toLower() == QLatin1String("qrc")) {
                // strips off any qrc: prefix and any excess slashes and replaces it with :/
                QUrl tempUrl(url);
                tempUrl.setScheme("");
                fileName = QLatin1String(":")+tempUrl.toString();
            }

            if (fileName.endsWith(QLatin1String(".dds"), Qt::CaseInsensitive))
            {
                setCompressedFile(fileName);
            }
            else
            {
                QImage im(fileName);
                if (im.isNull())
                    qWarning("Could not load texture: %s", qPrintable(fileName));
                setImage(im);
            }
        }
        else
        {
             if (!d->downloadManager) {
                if (getenv(QT3D_MULTITHREAD)) {
                    //Download in a multithreaded environment
                    d->downloadManager = new QThreadedDownloadManager();
                } else {
                    //Download in a single threaded environment
                    d->downloadManager = new QDownloadManager();
                }
                connect (d->downloadManager,SIGNAL(downloadComplete(QByteArray)),this, SLOT(textureRequestFinished(QByteArray)));
            }

            //Create a temporary image that will be used until the Url is loaded.
            static QImage tempImg(128,128, QImage::Format_RGB32);
            QColor fillcolor(Qt::gray);
            tempImg.fill(fillcolor.rgba());
            setImage(tempImg);

            if (!d->downloadManager->beginDownload(QUrl(url.toString()))) {
                qWarning("Unable to issue texture download request.");
            }
        }
    }
}

/*!
    Copies the contents of \a image to \a offset in this texture
    within the current GL context.

    Unlike setImage(), this function copies the image data to the
    GL server immediately using \c{glTexSubImage2D()}.  This is typically
    used to update the contents of a texture after it has been created.

    It is assumed that the application has already called bind() on
    this texture to bind it to the current GL context.

    If the texture has been created in multiple contexts, only the
    texture identifier for the current context will be updated.

    \sa setImage(), bind()
*/
void QGLTexture2D::copyImage(const QImage& image, const QPoint& offset)
{
    QImage img = QGLWidget::convertToGLFormat(image);
    glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x(), offset.y(),
                    img.width(), img.height(), GL_RGBA,
                    GL_UNSIGNED_BYTE, img.bits());
#if defined(QT_OPENGL_ES_2)
    Q_D(QGLTexture2D);
    if (d->bindOptions & QGLContext::MipmapBindOption)
        glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

/*!
    Returns the options to use when binding the image() to an OpenGL
    context for the first time.  The default options are
    QGLContext::LinearFilteringBindOption |
    QGLContext::InvertedYBindOption | QGLContext::MipmapBindOption.

    \sa setBindOptions()
*/
QGLContext::BindOptions QGLTexture2D::bindOptions() const
{
    Q_D(const QGLTexture2D);
    return d->bindOptions;
}

/*!
    Sets the \a options to use when binding the image() to an
    OpenGL context.  If the image() has already been bound,
    then changing the options will cause it to be recreated
    from image() the next time bind() is called.

    \sa bindOptions(), bind()
*/
void QGLTexture2D::setBindOptions(QGLContext::BindOptions options)
{
    Q_D(QGLTexture2D);
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
QGL::TextureWrap QGLTexture2D::horizontalWrap() const
{
    Q_D(const QGLTexture2D);
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
void QGLTexture2D::setHorizontalWrap(QGL::TextureWrap value)
{
    Q_D(QGLTexture2D);
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
QGL::TextureWrap QGLTexture2D::verticalWrap() const
{
    Q_D(const QGLTexture2D);
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
void QGLTexture2D::setVerticalWrap(QGL::TextureWrap value)
{
    Q_D(QGLTexture2D);
    value = qt_gl_modify_texture_wrap(value);
    if (d->verticalWrap != value) {
        d->verticalWrap = value;
        ++(d->parameterGeneration);
    }
}

/*!
    Binds this texture to the 2D texture target.

    If this texture object is not associated with an identifier in
    the current context, then a new identifier will be created,
    and image() uploaded into the GL server.

    If setImage() or setSize() was called since the last upload,
    then image() will be re-uploaded to the GL server.

    Returns false if the texture could not be bound for some reason.

    \sa release(), textureId(), setImage()
*/
bool QGLTexture2D::bind() const
{
    Q_D(const QGLTexture2D);
    return const_cast<QGLTexture2DPrivate *>(d)->bind(GL_TEXTURE_2D);
}

bool QGLTexture2DPrivate::bind(GLenum target)
{
    // Get the current context.  If we don't have one, then we
    // cannot bind the texture.
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return false;

    // Find the information block for the context, or create one.
    QGLTexture2DTextureInfo *info = infos;
    QGLTexture2DTextureInfo *prev = 0;
    while (info != 0 && !QGLContext::areSharing(info->tex.context(), ctx)) {
        if (info->isLiteral)
            return false; // Cannot create extra texture id's for literals.
        prev = info;
        info = info->next;
    }
    if (!info) {
        info = new QGLTexture2DTextureInfo
            (ctx, 0, imageGeneration - 1, parameterGeneration - 1);
        if (prev)
            prev->next = info;
        else
            infos = info;
    }

    if (!info->tex.textureId() || imageGeneration != info->imageGeneration) {
        // Create the texture contents and upload a new image.
        info->tex.setOptions(bindOptions);
        if (!compressedData.isEmpty()) {
            info->tex.bindCompressedTexture
                (compressedData.constData(), compressedData.size());
        } else {
            info->tex.startUpload(ctx, target, image.size());
            bindImages(info);
            info->tex.finishUpload(target);
        }
        info->imageGeneration = imageGeneration;
    } else {
        // Bind the existing texture to the texture target.
        glBindTexture(target, info->tex.textureId());
    }

    // If the parameter generation has changed, then alter the parameters.
    if (parameterGeneration != info->parameterGeneration) {
        info->parameterGeneration = parameterGeneration;
        q_glTexParameteri(target, GL_TEXTURE_WRAP_S, horizontalWrap);
        q_glTexParameteri(target, GL_TEXTURE_WRAP_T, verticalWrap);
    }

    // Texture is ready to be used.
    return true;
}

void QGLTexture2DPrivate::bindImages(QGLTexture2DTextureInfo *info)
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
    if (!image.isNull())
        info->tex.uploadFace(GL_TEXTURE_2D, image, scaledSize);
    else if (size.isValid())
        info->tex.createFace(GL_TEXTURE_2D, scaledSize);
}

/*!
    Releases the texture associated with the 2D texture target.
    This is equivalent to \c{glBindTexture(GL_TEXTURE_2D, 0)}.

    \sa bind()
*/
void QGLTexture2D::release() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*!
    Returns the identifier associated with this texture object in
    the current context.

    Returns zero if the texture has not previously been bound to
    the 2D texture target in the current context with bind().

    \sa bind()
*/
GLuint QGLTexture2D::textureId() const
{
    Q_D(const QGLTexture2D);
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return 0;
    QGLTexture2DTextureInfo *info = d->infos;
    while (info != 0 && !QGLContext::areSharing(info->tex.context(), ctx))
        info = info->next;
    return info ? info->tex.textureId() : 0;
}

/*!
    Constructs a QGLTexture2D object that wraps the supplied literal
    texture identifier \a id, with the dimensions specified by \a size.

    The \a id is assumed to have been created by the application in
    the current GL context, and it will be destroyed by the application
    after the returned QGLTexture2D object is destroyed.

    This function is intended for interfacing to existing code that
    uses raw GL texture identifiers.  The returned QGLTexture2D can
    only be used with the current GL context.

    \sa textureId()
*/
QGLTexture2D *QGLTexture2D::fromTextureId(GLuint id, const QSize& size)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!id || !ctx)
        return 0;

    QGLTexture2D *texture = new QGLTexture2D();
    if (!size.isNull())
        texture->setSize(size);
    QGLTexture2DTextureInfo *info = new QGLTexture2DTextureInfo
        (ctx, id, texture->d_ptr->imageGeneration,
         texture->d_ptr->parameterGeneration, true);
    texture->d_ptr->infos = info;
    return texture;
}

/*!
    This slot is used to recieve signals from the QDownloadManager class
    which provides Url download capability across the network.

    A successful download will have a valid QByteArray stored in \a assetData,
    while a failed download (due to network error, etc), will result in
    a NULL value.

    Successful downloads use the image data downloaded into \a assetData,
    which is converted to an image/texture, and emit a textureUpdated()
    signal.

    \sa QDownloadManager
*/
void QGLTexture2D::textureRequestFinished(QByteArray assetData)
{
    //Ensure valid asset data exists.
    if (assetData.isEmpty()) {
        qWarning("Network request failed. Texture not loaded.");
    } else {
        //Convert asset data to an image.
        QImage texImage;
        texImage.loadFromData(assetData);
        setImage(texImage.mirrored());

        emit textureUpdated();
    }
}

/*!
    \fn QGLTexture2D::textureUpdated()
    Signals that some property of this texture has changed in a manner
    that may require that the parent material class be updated.
*/

QT_END_NAMESPACE
