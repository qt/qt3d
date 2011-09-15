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

#include "qgltextureutils_p.h"
#include "qglext_p.h"
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

QGL::TextureWrap qt_gl_modify_texture_wrap(QGL::TextureWrap value)
{
    switch (value) {
#if defined(QT_OPENGL_ES)
    case QGL::Clamp:
        value = QGL::ClampToEdge;
        break;
#endif
#if !defined(QT_OPENGL_ES)
    case QGL::ClampToBorder:
        if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_3)
                == 0)
            value = QGL::Clamp;
        break;
#else
    case QGL::ClampToBorder:
        value = QGL::ClampToEdge;
        break;
#endif
#if !defined(QT_OPENGL_ES)
    case QGL::ClampToEdge:
        if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2)
                == 0)
            value = QGL::Clamp;
        break;
#endif
#if !defined(QT_OPENGL_ES)
    case QGL::MirroredRepeat:
        if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_4)
                == 0)
            value = QGL::Repeat;
        break;
#elif !defined(QT_OPENGL_ES_2)
    case QGL::MirroredRepeat:
        value = QGL::Repeat;
        break;
#endif
    default: break;
    }
    return value;
}

QGLTextureExtensions::QGLTextureExtensions(const QGLContext *ctx)
    : npotTextures(false)
    , generateMipmap(false)
    , bgraTextureFormat(false)
    , ddsTextureCompression(false)
    , etc1TextureCompression(false)
    , pvrtcTextureCompression(false)
    , compressedTexImage2D(0)
{
    Q_UNUSED(ctx);
    QGLExtensionChecker extensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
    if (extensions.match("GL_ARB_texture_non_power_of_two"))
        npotTextures = true;
    if (extensions.match("GL_SGIS_generate_mipmap"))
        generateMipmap = true;
    if (extensions.match("GL_EXT_bgra"))
        bgraTextureFormat = true;
    if (extensions.match("GL_EXT_texture_compression_s3tc"))
        ddsTextureCompression = true;
    if (extensions.match("GL_OES_compressed_ETC1_RGB8_texture"))
        etc1TextureCompression = true;
    if (extensions.match("GL_IMG_texture_compression_pvrtc"))
        pvrtcTextureCompression = true;
#if defined(QT_OPENGL_ES_2)
    npotTextures = true;
    generateMipmap = true;
#endif
#if !defined(QT_OPENGL_ES)
    if (extensions.match("GL_ARB_texture_compression")) {
        compressedTexImage2D = (q_glCompressedTexImage2DARB)
            ctx->getProcAddress(QLatin1String("glCompressedTexImage2DARB"));
    }
#else
    compressedTexImage2D = glCompressedTexImage2D;
#endif
}

QGLTextureExtensions::~QGLTextureExtensions()
{
}

Q_GLOBAL_STATIC(QGLResource<QGLTextureExtensions>, qt_gl_texture_extensions)

QGLTextureExtensions *QGLTextureExtensions::extensions()
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return 0;
    return qt_gl_texture_extensions()->value(ctx);
}

static void qt_gl_destroyTextureId(GLuint id)
{
    glDeleteTextures(1, &id);
}

QGLBoundTexture::QGLBoundTexture()
    : m_resource(qt_gl_destroyTextureId)
    , m_options(QGLContext::DefaultBindOption)
    , m_hasAlpha(false)
{
}

QGLBoundTexture::~QGLBoundTexture()
{
}

// #define QGL_BIND_TEXTURE_DEBUG

void QGLBoundTexture::startUpload(const QGLContext *ctx, GLenum target, const QSize &imageSize)
{
    Q_UNUSED(imageSize);

    QGLTextureExtensions *extensions = QGLTextureExtensions::extensions();
    if (!extensions)
        return;

#ifdef QGL_BIND_TEXTURE_DEBUG
    printf("QGLBoundTexture::startUpload(), imageSize=(%d,%d), options=%x\n",
           imageSize.width(), imageSize.height(), int(m_options));
    time.start();
#endif

#ifndef QT_NO_DEBUG
    // Reset the gl error stack...
    while (glGetError() != GL_NO_ERROR) ;
#endif

    // Create the texture id for the target, which should be one of
    // GL_TEXTURE_2D or GL_TEXTURE_CUBE_MAP.
    GLuint id = m_resource.id();
    if (id) {
        glBindTexture(target, 0);   // Just in case texture is bound.
        m_resource.destroy();
    }
    id = 0;
    glGenTextures(1, &id);
    glBindTexture(target, id);
    m_resource.attach(ctx, id);

    GLuint filtering = m_options & QGLContext::LinearFilteringBindOption ? GL_LINEAR : GL_NEAREST;

#ifdef QGL_BIND_TEXTURE_DEBUG
    printf(" - setting options (%d ms)\n", time.elapsed());
#endif
    q_glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filtering);

    if (QGLContext::currentContext()->format().directRendering()
        && extensions->generateMipmap
        && (m_options & QGLContext::MipmapBindOption))
    {
#if !defined(QT_OPENGL_ES_2)
        glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
        q_glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
#else
        glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
#endif
        q_glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
                m_options & QGLContext::LinearFilteringBindOption
                        ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
    } else {
        q_glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filtering);
        m_options &= ~QGLContext::MipmapBindOption;
    }
}

// map from Qt's ARGB endianness-dependent format to GL's big-endian RGBA layout
static inline void qt_gl_byteSwapImage(QImage &img, GLenum pixel_type)
{
    const int width = img.width();
    const int height = img.height();

    if (pixel_type == GL_UNSIGNED_INT_8_8_8_8_REV
        || (pixel_type == GL_UNSIGNED_BYTE && QSysInfo::ByteOrder == QSysInfo::LittleEndian))
    {
        for (int i = 0; i < height; ++i) {
            uint *p = (uint *) img.scanLine(i);
            for (int x = 0; x < width; ++x)
                p[x] = ((p[x] << 16) & 0xff0000) | ((p[x] >> 16) & 0xff) | (p[x] & 0xff00ff00);
        }
    } else {
        for (int i = 0; i < height; ++i) {
            uint *p = (uint *) img.scanLine(i);
            for (int x = 0; x < width; ++x)
                p[x] = (p[x] << 8) | ((p[x] >> 24) & 0xff);
        }
    }
}

// #define QGL_BIND_TEXTURE_DEBUG

void QGLBoundTexture::uploadFace
    (GLenum target, const QImage &image, const QSize &scaleSize, GLenum format)
{
    GLenum internalFormat(format);

    // Resolve the texture-related extensions for the current context.
    QGLTextureExtensions *extensions = QGLTextureExtensions::extensions();
    if (!extensions)
        return;

    // Adjust the image size for scaling and power of two.
    QSize size = (!scaleSize.isEmpty() ? scaleSize : image.size());
    if (!extensions->npotTextures)
        size = QGL::nextPowerOfTwo(size);
    QImage img(image);
    if (size != image.size()) {
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - scaling up to %dx%d (%d ms) \n", size.width(), size.height(), time.elapsed());
#endif
        img = img.scaled(size);
    }
    m_size = size;

    QImage::Format target_format = img.format();
    bool premul = m_options & QGLContext::PremultipliedAlphaBindOption;
    GLenum externalFormat;
    GLuint pixel_type;
    if (extensions->bgraTextureFormat) {
        externalFormat = GL_BGRA;
        if (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2)
            pixel_type = GL_UNSIGNED_INT_8_8_8_8_REV;
        else
            pixel_type = GL_UNSIGNED_BYTE;
    } else {
        externalFormat = GL_RGBA;
        pixel_type = GL_UNSIGNED_BYTE;
    }

    switch (target_format) {
    case QImage::Format_ARGB32:
        if (premul) {
            img = img.convertToFormat(target_format = QImage::Format_ARGB32_Premultiplied);
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - converting ARGB32 -> ARGB32_Premultiplied (%d ms) \n", time.elapsed());
#endif
        }
        break;
    case QImage::Format_ARGB32_Premultiplied:
        if (!premul) {
            img = img.convertToFormat(target_format = QImage::Format_ARGB32);
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - converting ARGB32_Premultiplied -> ARGB32 (%d ms)\n", time.elapsed());
#endif
        }
        break;
    case QImage::Format_RGB16:
        pixel_type = GL_UNSIGNED_SHORT_5_6_5;
        externalFormat = GL_RGB;
        internalFormat = GL_RGB;
        break;
    case QImage::Format_RGB32:
        break;
    default:
        if (img.hasAlphaChannel()) {
            img = img.convertToFormat(premul
                                      ? QImage::Format_ARGB32_Premultiplied
                                      : QImage::Format_ARGB32);
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - converting to 32-bit alpha format (%d ms)\n", time.elapsed());
#endif
        } else {
            img = img.convertToFormat(QImage::Format_RGB32);
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - converting to 32-bit (%d ms)\n", time.elapsed());
#endif
        }
    }

    if (m_options & QGLContext::InvertedYBindOption) {
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - flipping bits over y (%d ms)\n", time.elapsed());
#endif
        if (img.isDetached()) {
            int ipl = img.bytesPerLine() / 4;
            int h = img.height();
            for (int y=0; y<h/2; ++y) {
                int *a = (int *) img.scanLine(y);
                int *b = (int *) img.scanLine(h - y - 1);
                for (int x=0; x<ipl; ++x)
                    qSwap(a[x], b[x]);
            }
        } else {
            // Create a new image and copy across.  If we use the
            // above in-place code then a full copy of the image is
            // made before the lines are swapped, which processes the
            // data twice.  This version should only do it once.
            img = img.mirrored();
        }
    }

    if (externalFormat == GL_RGBA) {
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - doing byte swapping (%d ms)\n", time.elapsed());
#endif
        // The only case where we end up with a depth different from
        // 32 in the switch above is for the RGB16 case, where we set
        // the format to GL_RGB
        Q_ASSERT(img.depth() == 32);
        qt_gl_byteSwapImage(img, pixel_type);
    }
#ifdef QT_OPENGL_ES
    // OpenGL/ES requires that the internal and external formats be
    // identical.
    internalFormat = externalFormat;
#endif
#ifdef QGL_BIND_TEXTURE_DEBUG
    printf(" - uploading, image.format=%d, externalFormat=0x%x, internalFormat=0x%x, pixel_type=0x%x\n",
           img.format(), externalFormat, internalFormat, pixel_type);
#endif

    const QImage &constRef = img; // to avoid detach in bits()...
    glTexImage2D(target, 0, internalFormat, img.width(), img.height(), 0, externalFormat,
                 pixel_type, constRef.bits());

    m_hasAlpha = (internalFormat != GL_RGB);
}

void QGLBoundTexture::createFace
    (GLenum target, const QSize &size, GLenum format)
{
    glTexImage2D(target, 0, format, size.width(),
                 size.height(), 0, format, GL_UNSIGNED_BYTE, 0);
    m_hasAlpha = (format != GL_RGB);
}

void QGLBoundTexture::finishUpload(GLenum target)
{
    Q_UNUSED(target);

#if defined(QT_OPENGL_ES_2)
    // OpenGL/ES 2.0 needs to generate mipmaps after all cubemap faces
    // have been uploaded.
    if (m_options & QGLContext::MipmapBindOption) {
#ifdef QGL_BIND_TEXTURE_DEBUG
        printf(" - generating mipmaps (%d ms)\n", time.elapsed());
#endif
        glGenerateMipmap(target);
    }
#endif

#ifndef QT_NO_DEBUG
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qWarning(" - texture upload failed, error code 0x%x, enum: %d (%x)\n", error, target, target);
    }
#endif

#ifdef QGL_BIND_TEXTURE_DEBUG
    static int totalUploadTime = 0;
    totalUploadTime += time.elapsed();
    printf(" - upload done in (%d ms) time=%d\n", time.elapsed(), totalUploadTime);
#endif
}

// DDS format structure
struct DDSFormat {
    quint32 dwSize;
    quint32 dwFlags;
    quint32 dwHeight;
    quint32 dwWidth;
    quint32 dwLinearSize;
    quint32 dummy1;
    quint32 dwMipMapCount;
    quint32 dummy2[11];
    struct {
        quint32 dummy3[2];
        quint32 dwFourCC;
        quint32 dummy4[5];
    } ddsPixelFormat;
};

// compressed texture pixel formats
#define FOURCC_DXT1  0x31545844
#define FOURCC_DXT2  0x32545844
#define FOURCC_DXT3  0x33545844
#define FOURCC_DXT4  0x34545844
#define FOURCC_DXT5  0x35545844

#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

// PVR header format for container files that store textures compressed
// with the ETC1, PVRTC2, and PVRTC4 encodings.  Format information from the
// PowerVR SDK at http://www.imgtec.com/powervr/insider/powervr-sdk.asp
// "PVRTexTool Reference Manual, version 1.11f".
struct PvrHeader
{
    quint32 headerSize;
    quint32 height;
    quint32 width;
    quint32 mipMapCount;
    quint32 flags;
    quint32 dataSize;
    quint32 bitsPerPixel;
    quint32 redMask;
    quint32 greenMask;
    quint32 blueMask;
    quint32 alphaMask;
    quint32 magic;
    quint32 surfaceCount;
};

#define PVR_MAGIC               0x21525650      // "PVR!" in little-endian

#define PVR_FORMAT_MASK         0x000000FF
#define PVR_FORMAT_PVRTC2       0x00000018
#define PVR_FORMAT_PVRTC4       0x00000019
#define PVR_FORMAT_ETC1         0x00000036

#define PVR_HAS_MIPMAPS         0x00000100
#define PVR_TWIDDLED            0x00000200
#define PVR_NORMAL_MAP          0x00000400
#define PVR_BORDER_ADDED        0x00000800
#define PVR_CUBE_MAP            0x00001000
#define PVR_FALSE_COLOR_MIPMAPS 0x00002000
#define PVR_VOLUME_TEXTURE      0x00004000
#define PVR_ALPHA_IN_TEXTURE    0x00008000
#define PVR_VERTICAL_FLIP       0x00010000

#ifndef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG      0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG      0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG     0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG     0x8C03
#endif

#ifndef GL_ETC1_RGB8_OES
#define GL_ETC1_RGB8_OES                        0x8D64
#endif

bool QGLBoundTexture::canBindCompressedTexture
    (const char *buf, int len, const char *format, bool *hasAlpha,
     bool *isFlipped)
{
    if (QSysInfo::ByteOrder != QSysInfo::LittleEndian) {
        // Compressed texture loading only supported on little-endian
        // systems such as x86 and ARM at the moment.
        return false;
    }
    if (!format) {
        // Auto-detect the format from the header.
        if (len >= 4 && !qstrncmp(buf, "DDS ", 4)) {
            *hasAlpha = true;
            *isFlipped = true;
            return true;
        } else if (len >= 52 && !qstrncmp(buf + 44, "PVR!", 4)) {
            const PvrHeader *pvrHeader =
                reinterpret_cast<const PvrHeader *>(buf);
            *hasAlpha = (pvrHeader->alphaMask != 0);
            *isFlipped = ((pvrHeader->flags & PVR_VERTICAL_FLIP) != 0);
            return true;
        }
    } else {
        // Validate the format against the header.
        if (!qstricmp(format, "DDS")) {
            if (len >= 4 && !qstrncmp(buf, "DDS ", 4)) {
                *hasAlpha = true;
                *isFlipped = true;
                return true;
            }
        } else if (!qstricmp(format, "PVR") || !qstricmp(format, "ETC1")) {
            if (len >= 52 && !qstrncmp(buf + 44, "PVR!", 4)) {
                const PvrHeader *pvrHeader =
                    reinterpret_cast<const PvrHeader *>(buf);
                *hasAlpha = (pvrHeader->alphaMask != 0);
                *isFlipped = ((pvrHeader->flags & PVR_VERTICAL_FLIP) != 0);
                return true;
            }
        }
    }
    return false;
}

bool QGLBoundTexture::bindCompressedTexture
    (const char *buf, int len, const char *format)
{
    if (QSysInfo::ByteOrder != QSysInfo::LittleEndian) {
        // Compressed texture loading only supported on little-endian
        // systems such as x86 and ARM at the moment.
        return false;
    }
#if !defined(QT_OPENGL_ES)
    QGLTextureExtensions *extensions = QGLTextureExtensions::extensions();
    if (!extensions)
        return false;
    if (!extensions->compressedTexImage2D) {
        qWarning("QGLContext::bindTexture(): The GL implementation does "
                 "not support texture compression extensions.");
        return false;
    }
#endif
    if (!format) {
        // Auto-detect the format from the header.
        if (len >= 4 && !qstrncmp(buf, "DDS ", 4))
            return bindCompressedTextureDDS(buf, len);
        else if (len >= 52 && !qstrncmp(buf + 44, "PVR!", 4))
            return bindCompressedTexturePVR(buf, len);
    } else {
        // Validate the format against the header.
        if (!qstricmp(format, "DDS")) {
            if (len >= 4 && !qstrncmp(buf, "DDS ", 4))
                return bindCompressedTextureDDS(buf, len);
        } else if (!qstricmp(format, "PVR") || !qstricmp(format, "ETC1")) {
            if (len >= 52 && !qstrncmp(buf + 44, "PVR!", 4))
                return bindCompressedTexturePVR(buf, len);
        }
    }
    return false;
}

bool QGLBoundTexture::bindCompressedTexture
    (const QString& fileName, const char *format)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QByteArray contents = file.readAll();
    file.close();
    return bindCompressedTexture
        (contents.constData(), contents.size(), format);
}

bool QGLBoundTexture::bindCompressedTextureDDS(const char *buf, int len)
{
    QGLTextureExtensions *extensions = QGLTextureExtensions::extensions();
    if (!extensions)
        return false;

    // Bail out if the necessary extension is not present.
    if (!extensions->ddsTextureCompression) {
        qWarning("QGLBoundTexture::bindCompressedTextureDDS(): DDS texture compression is not supported.");
        return false;
    }

    const DDSFormat *ddsHeader = reinterpret_cast<const DDSFormat *>(buf + 4);
    if (!ddsHeader->dwLinearSize) {
        qWarning("QGLBoundTexture::bindCompressedTextureDDS(): DDS image size is not valid.");
        return false;
    }

    int blockSize = 16;
    GLenum format;

    switch(ddsHeader->ddsPixelFormat.dwFourCC) {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        blockSize = 8;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        qWarning("QGLBoundTexture::bindCompressedTextureDDS(): DDS image format not supported.");
        return false;
    }

    const GLubyte *pixels =
        reinterpret_cast<const GLubyte *>(buf + ddsHeader->dwSize + 4);

    GLuint id = m_resource.id();
    if (id) {
        glBindTexture(GL_TEXTURE_2D, 0);    // Just in case it is bound.
        m_resource.destroy();
    }
    id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    q_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    q_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_resource.attach(QGLContext::currentContext(), id);

    int size;
    int offset = 0;
    int available = len - int(ddsHeader->dwSize + 4);
    int w = ddsHeader->dwWidth;
    int h = ddsHeader->dwHeight;

    // load mip-maps
    for (int i = 0; i < (int) ddsHeader->dwMipMapCount; ++i) {
        if (w == 0) w = 1;
        if (h == 0) h = 1;

        size = ((w+3)/4) * ((h+3)/4) * blockSize;
        if (size > available)
            break;
        extensions->compressedTexImage2D
            (GL_TEXTURE_2D, i, format, w, h, 0, size, pixels + offset);
        offset += size;
        available -= size;

        // half size for each mip-map level
        w = w/2;
        h = h/2;
    }

    // DDS images are not inverted.
    m_options &= ~QGLContext::InvertedYBindOption;

    m_size = QSize(ddsHeader->dwWidth, ddsHeader->dwHeight);
    m_hasAlpha = false;
    return true;
}

bool QGLBoundTexture::bindCompressedTexturePVR(const char *buf, int len)
{
    QGLTextureExtensions *extensions = QGLTextureExtensions::extensions();
    if (!extensions)
        return false;

    // Determine which texture format we will be loading.
    const PvrHeader *pvrHeader = reinterpret_cast<const PvrHeader *>(buf);
    GLenum textureFormat;
    quint32 minWidth, minHeight;
    switch (pvrHeader->flags & PVR_FORMAT_MASK) {
    case PVR_FORMAT_PVRTC2:
        if (pvrHeader->alphaMask)
            textureFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
        else
            textureFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
        minWidth = 16;
        minHeight = 8;
        break;

    case PVR_FORMAT_PVRTC4:
        if (pvrHeader->alphaMask)
            textureFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        else
            textureFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
        minWidth = 8;
        minHeight = 8;
        break;

    case PVR_FORMAT_ETC1:
        textureFormat = GL_ETC1_RGB8_OES;
        minWidth = 4;
        minHeight = 4;
        break;

    default:
        qWarning("QGLBoundTexture::bindCompressedTexturePVR(): PVR image format 0x%x not supported.", int(pvrHeader->flags & PVR_FORMAT_MASK));
        return false;
    }

    // Bail out if the necessary extension is not present.
    if (textureFormat == GL_ETC1_RGB8_OES) {
        if (!extensions->etc1TextureCompression) {
            qWarning("QGLBoundTexture::bindCompressedTexturePVR(): ETC1 texture compression is not supported.");
            return false;
        }
    } else {
        if (!extensions->pvrtcTextureCompression) {
            qWarning("QGLBoundTexture::bindCompressedTexturePVR(): PVRTC texture compression is not supported.");
            return false;
        }
    }

    // Boundary check on the buffer size.
    quint32 bufferSize = pvrHeader->headerSize + pvrHeader->dataSize;
    if (bufferSize > quint32(len)) {
        qWarning("QGLBoundTexture::bindCompressedTexturePVR(): PVR image size is not valid.");
        return false;
    }

    // Create the texture.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLuint id = m_resource.id();
    if (id) {
        glBindTexture(GL_TEXTURE_2D, 0);    // Just in case it is bound.
        m_resource.destroy();
    }
    id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    m_resource.attach(QGLContext::currentContext(), id);
    if (pvrHeader->mipMapCount) {
        if ((m_options & QGLContext::LinearFilteringBindOption) != 0) {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        }
    } else if ((m_options & QGLContext::LinearFilteringBindOption) != 0) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    // Load the compressed mipmap levels.
    const GLubyte *buffer =
        reinterpret_cast<const GLubyte *>(buf + pvrHeader->headerSize);
    bufferSize = pvrHeader->dataSize;
    quint32 level = 0;
    quint32 width = pvrHeader->width;
    quint32 height = pvrHeader->height;
    while (bufferSize > 0 && level <= pvrHeader->mipMapCount) {
        quint32 size =
            (qMax(width, minWidth) * qMax(height, minHeight) *
             pvrHeader->bitsPerPixel) / 8;
        if (size > bufferSize)
            break;
        extensions->compressedTexImage2D
            (GL_TEXTURE_2D, GLint(level), textureFormat,
             GLsizei(width), GLsizei(height), 0, GLsizei(size), buffer);
        width /= 2;
        height /= 2;
        buffer += size;
        ++level;
    }

    // Restore the default pixel alignment for later texture uploads.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Set the invert flag for the texture.  The "vertical flip"
    // flag in PVR is the opposite sense to our sense of inversion.
    if ((pvrHeader->flags & PVR_VERTICAL_FLIP) != 0)
        m_options &= ~QGLContext::InvertedYBindOption;
    else
        m_options |= QGLContext::InvertedYBindOption;

    m_size = QSize(pvrHeader->width, pvrHeader->height);
    m_hasAlpha = (pvrHeader->alphaMask != 0);
    return true;
}

QT_END_NAMESPACE
