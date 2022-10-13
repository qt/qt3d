// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "imagesubmissioncontext_p.h"
#include <Qt3DRender/private/shaderimage_p.h>
#include <Qt3DRender/qshaderimage.h>
#include <graphicscontext_p.h>
#include <gltexture_p.h>
#include <logging_p.h>

QT_BEGIN_NAMESPACE

// ES 3.1+ or GL 4.2+
#ifndef GL_READ_ONLY
#define GL_READ_ONLY                      0x88B8
#endif
#ifndef GL_WRITE_ONLY
#define GL_WRITE_ONLY                     0x88B9
#endif
#ifndef GL_READ_WRITE
#define GL_READ_WRITE                     0x88BA
#endif

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

class GraphicsContext;
class GLTexture;

namespace  {

GLenum glAccessEnumForShaderImageAccess(QShaderImage::Access access)
{
    switch (access) {
    case QShaderImage::ReadOnly:
        return GL_READ_ONLY;
    case QShaderImage::WriteOnly:
        return GL_WRITE_ONLY;
    case QShaderImage::ReadWrite:
    default:
        break;
    }
    return GL_READ_WRITE;
}

GLenum glImageFormatToGL(QShaderImage::ImageFormat format)
{
    // Right now we can abuse from the fact that the ImageFormat enum values
    // have been assigned the same value as the GL enum
    return GLenum(format);
}

GLenum glImageFormatForShaderImageFormat(QShaderImage::ImageFormat format,
                                         QAbstractTexture::TextureFormat textureFormat)
{
    Q_ASSERT_X(format != QShaderImage::NoFormat, Q_FUNC_INFO, "Valid image format or Automatic expected");

    if (format != QShaderImage::Automatic)
        return glImageFormatToGL(format);

    // Otherwise try to mind to best texture format
    switch (textureFormat) {
    case QAbstractTexture::R8_UNorm:
        return glImageFormatToGL(QShaderImage::R8_UNorm);
    case QAbstractTexture::RG8_UNorm:
        return glImageFormatToGL(QShaderImage::RG8_UNorm);
    case QAbstractTexture::RGBA8_UNorm:
        return glImageFormatToGL(QShaderImage::RGBA8_UNorm);

    case QAbstractTexture::R16_UNorm:
        return glImageFormatToGL(QShaderImage::R16_UNorm);
    case QAbstractTexture::RG16_UNorm:
        return glImageFormatToGL(QShaderImage::RG16_UNorm);
    case QAbstractTexture::RGBA16_UNorm:
        return glImageFormatToGL(QShaderImage::RGBA16_UNorm);

    case QAbstractTexture::R8_SNorm:
        return glImageFormatToGL(QShaderImage::R8_SNorm);
    case QAbstractTexture::RG8_SNorm:
        return glImageFormatToGL(QShaderImage::RG8_SNorm);
    case QAbstractTexture::RGBA8_SNorm:
        return glImageFormatToGL(QShaderImage::RGBA8_SNorm);

    case QAbstractTexture::R16_SNorm:
        return glImageFormatToGL(QShaderImage::R16_SNorm);
    case QAbstractTexture::RG16_SNorm:
        return glImageFormatToGL(QShaderImage::RG16_SNorm);
    case QAbstractTexture::RGBA16_SNorm:
        return glImageFormatToGL(QShaderImage::RGBA16_SNorm);

    case QAbstractTexture::R8U:
        return glImageFormatToGL(QShaderImage::R8U);
    case QAbstractTexture::RG8U:
        return glImageFormatToGL(QShaderImage::RG8U);
    case QAbstractTexture::RGBA8U:
        return glImageFormatToGL(QShaderImage::RGBA8U);

    case QAbstractTexture::R16U:
        return glImageFormatToGL(QShaderImage::R16U);
    case QAbstractTexture::RG16U:
        return glImageFormatToGL(QShaderImage::RG16U);
    case QAbstractTexture::RGBA16U:
        return glImageFormatToGL(QShaderImage::RGBA16U);

    case QAbstractTexture::R32U:
        return glImageFormatToGL(QShaderImage::R32U);
    case QAbstractTexture::RG32U:
        return glImageFormatToGL(QShaderImage::RG32U);
    case QAbstractTexture::RGBA32U:
        return glImageFormatToGL(QShaderImage::RGBA32U);

    case QAbstractTexture::R8I:
        return glImageFormatToGL(QShaderImage::R8I);
    case QAbstractTexture::RG8I:
        return glImageFormatToGL(QShaderImage::RG8I);
    case QAbstractTexture::RGBA8I:
        return glImageFormatToGL(QShaderImage::RGBA8I);

    case QAbstractTexture::R16I:
        return glImageFormatToGL(QShaderImage::R16I);
    case QAbstractTexture::RG16I:
        return glImageFormatToGL(QShaderImage::RG16I);
    case QAbstractTexture::RGBA16I:
        return glImageFormatToGL(QShaderImage::RGBA16I);

    case QAbstractTexture::R32I:
        return glImageFormatToGL(QShaderImage::R32I);
    case QAbstractTexture::RG32I:
        return glImageFormatToGL(QShaderImage::RG32I);
    case QAbstractTexture::RGBA32I:
        return glImageFormatToGL(QShaderImage::RGBA32I);

    case QAbstractTexture::R16F:
        return glImageFormatToGL(QShaderImage::R16F);
    case QAbstractTexture::RG16F:
        return glImageFormatToGL(QShaderImage::RG16F);
    case QAbstractTexture::RGBA16F:
        return glImageFormatToGL(QShaderImage::RGBA16F);

    case QAbstractTexture::R32F:
        return glImageFormatToGL(QShaderImage::R32F);
    case QAbstractTexture::RG32F:
        return glImageFormatToGL(QShaderImage::RG32F);
    case QAbstractTexture::RGBA32F:
        return glImageFormatToGL(QShaderImage::RGBA32F);

    case QAbstractTexture::RG11B10F:
        return glImageFormatToGL(QShaderImage::RG11B10F);
    case QAbstractTexture::RGB10A2:
        return glImageFormatToGL(QShaderImage::RGB10A2);
    case QAbstractTexture::RGB10A2U:
        return glImageFormatToGL(QShaderImage::RGB10A2U);

    default:
        qWarning() << "Cannot map Texture format" << textureFormat << "to a valid Image Format";
        Q_UNREACHABLE_RETURN(GL_NONE);
    }
}

} // anonymous

ImageSubmissionContext::ImageSubmissionContext()
    : m_ctx(nullptr)
{
}

void ImageSubmissionContext::initialize(GraphicsContext *context)
{
    m_ctx = context;
    m_activeImages.resize(m_ctx->maxImageUnitsCount());
}

void ImageSubmissionContext::endDrawing()
{
    // Reduce score of all active Images
    decayImageScores();
}

// Return Image Unit for Image
// If Image was used previously and recently, it will return the last used unit
// for that image. Otherwise it will try to return the image unit the least used.
int ImageSubmissionContext::activateImage(ShaderImage *image, GLTexture *tex)
{
    const int onUnit = assignUnitForImage(image->peerId());

    if (onUnit < 0) {
        qWarning() << "Unable to find available image unit";
        return -1;
    }

    QOpenGLTexture *glTex = tex->getGLTexture();
    if (glTex == nullptr) {
        qWarning() << "Unable to retrieve valid texture for Image";
        return -1;
    }

    // Bind Image against Texture and resolve Image Format
    m_ctx->bindImageTexture(onUnit,
                            glTex->textureId(),
                            image->mipLevel(),
                            image->layered(),
                            image->layer(),
                            glAccessEnumForShaderImageAccess(image->access()),
                            glImageFormatForShaderImageFormat(image->format(),
                                                              tex->properties().format));

    // Store information about the Texture/Image on ActiveImage for given
    // image unit
    m_activeImages[onUnit].shaderImageId = image->peerId();
    m_activeImages[onUnit].texture = tex;
    m_activeImages[onUnit].score = 200;
    m_activeImages[onUnit].pinned = true;

    return onUnit;
}

// Unset pinned Active Image and reduce their score
void ImageSubmissionContext::deactivateImages()
{
    for (size_t u = 0, m = m_activeImages.size();  u < m; ++u) {
        if (m_activeImages[u].pinned) {
            m_activeImages[u].pinned = false;
            m_activeImages[u].score = qMax(m_activeImages[u].score - 1, 0);
            return;
        }
    }
}

// Reduce score of all active images (pinned or not)
void ImageSubmissionContext::decayImageScores()
{
    for (size_t u = 0, m = m_activeImages.size();  u < m; ++u)
        m_activeImages[u].score = qMax(m_activeImages[u].score - 1, 0);
}

int ImageSubmissionContext::assignUnitForImage(Qt3DCore::QNodeId shaderImageId)
{
    int lowestScoredUnit = -1;
    int lowestScore = 0xfffffff;

    const size_t m = m_activeImages.size();
    for (size_t u = 0; u < m; ++u) {
        if (m_activeImages[u].shaderImageId == shaderImageId)
            return int(u);
    }

    for (size_t u = 0; u < m; ++u) {
        // No image is currently active on the image unit
        // we save the image unit with the texture that has been on there
        // the longest time while not being used
        if (!m_activeImages[u].pinned) {
            const int score = m_activeImages[u].score;
            if (score < lowestScore) {
                lowestScore = score;
                lowestScoredUnit = int(u);
            }
        }
    }

    if (lowestScoredUnit == -1)
        qCWarning(Backend) << Q_FUNC_INFO << "No free image units!";

    return lowestScoredUnit;
}

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
