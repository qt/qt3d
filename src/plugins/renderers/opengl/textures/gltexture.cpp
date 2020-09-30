/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qhash.h>
#include "gltexture_p.h"

#include <private/qdebug_p.h>
#include <private/qopengltexture_p.h>
#include <private/qopengltexturehelper_p.h>
#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qtexturedata.h>
#include <Qt3DRender/qtextureimagedata.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/qabstracttexture_p.h>
#include <Qt3DRender/private/qtextureimagedata_p.h>
#include <renderbuffer_p.h>

#if !defined(QT_OPENGL_ES_2)
#include <QOpenGLFunctions_3_1>
#include <QOpenGLFunctions_4_5_Core>
#endif

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

namespace {

// This uploadGLData where the data is a fullsize subimage
// as QOpenGLTexture doesn't allow partial subimage uploads
void uploadGLData(QOpenGLTexture *glTex,
                  int level, int layer, QOpenGLTexture::CubeMapFace face,
                  const QByteArray &bytes, const QTextureImageDataPtr &data)
{
    const auto alignment = QTextureImageDataPrivate::get(data.get())->m_alignment;
    QOpenGLPixelTransferOptions uploadOptions;
    uploadOptions.setAlignment(alignment);
    if (data->isCompressed())
        glTex->setCompressedData(level, layer, face, bytes.size(), bytes.constData(), &uploadOptions);
    else
        glTex->setData(level, layer, face, data->pixelFormat(), data->pixelType(), bytes.constData(), &uploadOptions);
}

// For partial sub image uploads
void uploadGLData(QOpenGLTexture *glTex,
                  int mipLevel, int layer, QOpenGLTexture::CubeMapFace cubeFace,
                  int xOffset, int yOffset, int zOffset,
                  const QByteArray &bytes, const QTextureImageDataPtr &data)
{
    if (data->isCompressed()) {
        qWarning() << Q_FUNC_INFO << "Uploading non full sized Compressed Data not supported yet";
    } else {
        const auto alignment = QTextureImageDataPrivate::get(data.get())->m_alignment;
        QOpenGLPixelTransferOptions uploadOptions;
        uploadOptions.setAlignment(alignment);
        glTex->setData(xOffset, yOffset, zOffset,
                       data->width(), data->height(), data->depth(),
                       mipLevel, layer, cubeFace, data->layers(),
                       data->pixelFormat(), data->pixelType(),
                       bytes.constData(), &uploadOptions);
    }
}

} // anonymous


GLTexture::GLTexture()
    : m_dirtyFlags(None)
    , m_gl(nullptr)
    , m_renderBuffer(nullptr)
    , m_dataFunctor()
    , m_pendingDataFunctor(nullptr)
    , m_sharedTextureId(-1)
    , m_externalRendering(false)
    , m_wasTextureRecreated(false)
{
}

GLTexture::~GLTexture()
{
}

// Must be called from RenderThread with active GL context
void GLTexture::destroy()
{
    delete m_gl;
    m_gl = nullptr;
    delete m_renderBuffer;
    m_renderBuffer = nullptr;

    m_dirtyFlags = None;
    m_sharedTextureId = -1;
    m_externalRendering = false;
    m_wasTextureRecreated = false;
    m_dataFunctor.reset();
    m_pendingDataFunctor = nullptr;

    m_properties = {};
    m_parameters = {};
    m_textureData.reset();
    m_images.clear();
    m_imageData.clear();
    m_pendingTextureDataUpdates.clear();
}

bool GLTexture::loadTextureDataFromGenerator()
{
    m_textureData = m_dataFunctor->operator()();
    // if there is a texture generator, most properties will be defined by it
    if (m_textureData) {
        const QAbstractTexture::Target target = m_textureData->target();

        // If both target and functor return Automatic we are still
        // probably loading the texture, return false
        if (m_properties.target == QAbstractTexture::TargetAutomatic &&
            target == QAbstractTexture::TargetAutomatic) {
            m_textureData.reset();
            return false;
        }

        if (m_properties.target != QAbstractTexture::TargetAutomatic &&
            target != QAbstractTexture::TargetAutomatic &&
            m_properties.target != target) {
            qWarning() << Q_FUNC_INFO << "Generator and Properties not requesting the same texture target";
            m_textureData.reset();
            return false;
        }

        // We take target type from generator if it wasn't explicitly set by the user
        if (m_properties.target == QAbstractTexture::TargetAutomatic)
            m_properties.target = target;
        m_properties.width = m_textureData->width();
        m_properties.height = m_textureData->height();
        m_properties.depth = m_textureData->depth();
        m_properties.layers = m_textureData->layers();
        m_properties.format = m_textureData->format();

        const QVector<QTextureImageDataPtr> imageData = m_textureData->imageData();

        if (imageData.size() > 0) {
            // Set the mips level based on the first image if autoMipMapGeneration is disabled
            if (!m_properties.generateMipMaps)
                m_properties.mipLevels = imageData.first()->mipLevels();
        }
    }
    return !m_textureData.isNull();
}

void GLTexture::loadTextureDataFromImages()
{
    int maxMipLevel = 0;
    for (const Image &img : qAsConst(m_images)) {
        const QTextureImageDataPtr imgData = img.generator->operator()();
        // imgData may be null in the following cases:
        // - Texture is created with TextureImages which have yet to be
        // loaded (skybox where you don't yet know the path, source set by
        // a property binding, queued connection ...)
        // - TextureImage whose generator failed to return a valid data
        // (invalid url, error opening file...)
        if (imgData.isNull())
            continue;

        m_imageData.push_back(imgData);
        maxMipLevel = qMax(maxMipLevel, img.mipLevel);

        // If the texture doesn't have a texture generator, we will
        // derive some properties from the first TextureImage (layer=0, miplvl=0, face=0)
        if (!m_textureData && img.layer == 0 && img.mipLevel == 0 && img.face == QAbstractTexture::CubeMapPositiveX) {
            if (imgData->width() != -1 && imgData->height() != -1 && imgData->depth() != -1) {
                m_properties.width = imgData->width();
                m_properties.height = imgData->height();
                m_properties.depth = imgData->depth();
            }
            // Set the format of the texture if the texture format is set to Automatic
            if (m_properties.format == QAbstractTexture::Automatic) {
                m_properties.format = static_cast<QAbstractTexture::TextureFormat>(imgData->format());
            }
            setDirtyFlag(Properties, true);
        }
    }

    // make sure the number of mip levels is set when there is no texture data generator
    if (!m_dataFunctor) {
        m_properties.mipLevels = maxMipLevel + 1;
        setDirtyFlag(Properties, true);
    }
}

// Called from RenderThread
GLTexture::TextureUpdateInfo GLTexture::createOrUpdateGLTexture()
{
    TextureUpdateInfo textureInfo;
    m_wasTextureRecreated = false;

    const bool hasSharedTextureId = m_sharedTextureId > 0;
    // Only load texture data if we are not using a sharedTextureId
    // Check if dataFunctor or images have changed
    if (!hasSharedTextureId) {
        // If dataFunctor exists and we have no data and it hasn´t run yet
        if (m_dataFunctor && !m_textureData && m_dataFunctor.get() != m_pendingDataFunctor ) {
            const bool successfullyLoadedTextureData = loadTextureDataFromGenerator();
            // If successful, m_textureData has content
            if (successfullyLoadedTextureData) {
                setDirtyFlag(Properties, true);
                setDirtyFlag(TextureData, true);
            } else {
                if (m_pendingDataFunctor != m_dataFunctor.get()) {
                    qWarning() << "[Qt3DRender::GLTexture] No QTextureData generated from Texture Generator yet. Texture will be invalid for this frame";
                    m_pendingDataFunctor = m_dataFunctor.get();
                }
                textureInfo.properties.status = QAbstractTexture::Loading;
                return textureInfo;
            }
        }

        // If images have changed, clear previous images data
        // and regenerate m_imageData for the images
        if (testDirtyFlag(TextureImageData)) {
            m_imageData.clear();
            loadTextureDataFromImages();
            // Mark for upload if we actually have something to upload
            if (!m_imageData.empty()) {
                setDirtyFlag(TextureData, true);
            }
            // Reset image flag
            setDirtyFlag(TextureImageData, false);
        }

        // Don't try to create the texture if the target or format was still not set
        // Format should either be set by user or if Automatic
        // by either the dataGenerator of the texture or the first Image
        // Target should explicitly be set by the user or the dataGenerator
        if (m_properties.target == QAbstractTexture::TargetAutomatic ||
            m_properties.format == QAbstractTexture::Automatic ||
            m_properties.format == QAbstractTexture::NoFormat) {
            textureInfo.properties.status = QAbstractTexture::Error;
            return textureInfo;
        }
    }

    // If the properties changed or texture has become a shared texture from a
    // 3rd party engine, we need to destroy and maybe re-allocate the texture
    if (testDirtyFlag(Properties) || testDirtyFlag(SharedTextureId)) {
        delete m_gl;
        m_gl = nullptr;
        textureInfo.wasUpdated = true;
        // If we are destroyed because of some property change but still have (some) of
        // our content data make sure we are marked for upload
        // TO DO: We should actually check if the textureData is still correct
        // in regard to the size, target and format of the texture though.
        if (!testDirtyFlag(SharedTextureId) &&
            (m_textureData || !m_imageData.empty() || !m_pendingTextureDataUpdates.empty()))
            setDirtyFlag(TextureData, true);
    }

    m_properties.status = QAbstractTexture::Ready;

    if (testDirtyFlag(SharedTextureId) || hasSharedTextureId) {
        // Update m_properties by doing introspection on the texture
        if (hasSharedTextureId)
            introspectPropertiesFromSharedTextureId();
        setDirtyFlag(SharedTextureId, false);
    } else {
        // We only build a QOpenGLTexture if we have no shared textureId set
        if (!m_gl) {
            m_gl = buildGLTexture();
            if (!m_gl) {
                qWarning() << "[Qt3DRender::GLTexture] failed to create texture";
                textureInfo.properties.status = QAbstractTexture::Error;
                return textureInfo;
            }

            m_gl->allocateStorage();
            if (!m_gl->isStorageAllocated()) {
                qWarning() << "[Qt3DRender::GLTexture] failed to allocate texture";
                textureInfo.properties.status = QAbstractTexture::Error;
                return textureInfo;
            }
            m_wasTextureRecreated = true;
        }

        textureInfo.texture = m_gl;

        // need to (re-)upload texture data?
        const bool needsUpload = testDirtyFlag(TextureData);
        if (needsUpload) {
            uploadGLTextureData();
            setDirtyFlag(TextureData, false);
        }

        // need to set texture parameters?
        if (testDirtyFlag(Properties) || testDirtyFlag(Parameters)) {
            updateGLTextureParameters();
            setDirtyFlag(Properties, false);
            setDirtyFlag(Parameters, false);
        }
    }

    textureInfo.properties = m_properties;

    return textureInfo;
}

RenderBuffer *GLTexture::getOrCreateRenderBuffer()
{
    if (m_dataFunctor && !m_textureData) {
        m_textureData = m_dataFunctor->operator()();
        if (m_textureData) {
            if (m_properties.target != QAbstractTexture::TargetAutomatic)
                qWarning() << "[Qt3DRender::GLTexture] [renderbuffer] When a texture provides a generator, it's target is expected to be TargetAutomatic";

            m_properties.width = m_textureData->width();
            m_properties.height = m_textureData->height();
            m_properties.format = m_textureData->format();

            setDirtyFlag(Properties);
        } else {
            if (m_pendingDataFunctor != m_dataFunctor.get()) {
                qWarning() << "[Qt3DRender::GLTexture] [renderbuffer] No QTextureData generated from Texture Generator yet. Texture will be invalid for this frame";
                m_pendingDataFunctor = m_dataFunctor.get();
            }
            return nullptr;
        }
    }

    if (testDirtyFlag(Properties)) {
        delete m_renderBuffer;
        m_renderBuffer = nullptr;
    }

    if (!m_renderBuffer)
        m_renderBuffer = new RenderBuffer(m_properties.width, m_properties.height, m_properties.format);

    setDirtyFlag(Properties, false);
    setDirtyFlag(Parameters, false);

    return m_renderBuffer;
}

// This must be called from the RenderThread
// So GLTexture release from the manager can only be done from that thread
void GLTexture::cleanup()
{
    destroy();
}

void GLTexture::setParameters(const TextureParameters &params)
{
    if (m_parameters != params) {
        m_parameters = params;
        setDirtyFlag(Parameters);
    }
}

void GLTexture::setProperties(const TextureProperties &props)
{
    if (m_properties != props) {
        m_properties = props;
        setDirtyFlag(Properties);
    }
}

void GLTexture::setImages(const QVector<Image> &images)
{
    // check if something has changed at all
    bool same = (images.size() == m_images.size());
    if (same) {
        for (int i = 0; i < images.size(); i++) {
            if (images[i] != m_images[i]) {
                same = false;
                break;
            }
        }
    }


    if (!same) {
        m_images = images;
        requestImageUpload();
    }
}

void GLTexture::setGenerator(const QTextureGeneratorPtr &generator)
{
    m_textureData.reset();
    m_dataFunctor = generator;
    m_pendingDataFunctor = nullptr;
    requestUpload();
}

void GLTexture::setSharedTextureId(int textureId)
{
    if (m_sharedTextureId != textureId) {
        m_sharedTextureId = textureId;
        setDirtyFlag(SharedTextureId);
    }
}

void GLTexture::addTextureDataUpdates(const QVector<QTextureDataUpdate> &updates)
{
    m_pendingTextureDataUpdates += updates;
    requestUpload();
}

// Return nullptr if
// - context cannot be obtained
// - texture hasn't yet been loaded
QOpenGLTexture *GLTexture::buildGLTexture()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qWarning() << Q_FUNC_INFO << "requires an OpenGL context";
        return nullptr;
    }

    const QAbstractTexture::Target actualTarget = m_properties.target;
    if (actualTarget == QAbstractTexture::TargetAutomatic) {
        // If the target is automatic at this point, it means that the texture
        // hasn't been loaded yet (case of remote urls) and that loading failed
        // and that target format couldn't be deduced
        return nullptr;
    }

    QOpenGLTexture* glTex = new QOpenGLTexture(static_cast<QOpenGLTexture::Target>(actualTarget));

    // m_format may not be ES2 compatible. Now it's time to convert it, if necessary.
    QAbstractTexture::TextureFormat format = m_properties.format;
    if (ctx->isOpenGLES() && ctx->format().majorVersion() < 3) {
        switch (m_properties.format) {
        case QAbstractTexture::RGBA8_UNorm:
        case QAbstractTexture::RGBAFormat:
            format = QAbstractTexture::RGBAFormat;
            break;
        case QAbstractTexture::RGB8_UNorm:
        case QAbstractTexture::RGBFormat:
            format = QAbstractTexture::RGBFormat;
            break;
        case QAbstractTexture::DepthFormat:
            format = QAbstractTexture::DepthFormat;
            break;
        default:
            auto warning = qWarning();
            warning << "Could not find a matching OpenGL ES 2.0 texture format:";
            QtDebugUtils::formatQEnum(warning, m_properties.format);
            break;
        }
    }

    // Map ETC1 to ETC2 when supported. This allows using features like
    // immutable storage as ETC2 is standard in GLES 3.0, while the ETC1 extension
    // is written against GLES 1.0.
    if (m_properties.format == QAbstractTexture::RGB8_ETC1) {
        if ((ctx->isOpenGLES() && ctx->format().majorVersion() >= 3)
            || ctx->hasExtension(QByteArrayLiteral("GL_OES_compressed_ETC2_RGB8_texture"))
            || ctx->hasExtension(QByteArrayLiteral("GL_ARB_ES3_compatibility")))
            format = m_properties.format = QAbstractTexture::RGB8_ETC2;
    }

    glTex->setFormat(m_properties.format == QAbstractTexture::Automatic ?
                     QOpenGLTexture::NoFormat :
                     static_cast<QOpenGLTexture::TextureFormat>(format));
    glTex->setSize(m_properties.width, m_properties.height, m_properties.depth);
    // Set layers count if texture array
    if (actualTarget == QAbstractTexture::Target1DArray ||
        actualTarget == QAbstractTexture::Target2DArray ||
        actualTarget == QAbstractTexture::Target2DMultisampleArray ||
        actualTarget == QAbstractTexture::TargetCubeMapArray) {
        glTex->setLayers(m_properties.layers);
    }

    if (actualTarget == QAbstractTexture::Target2DMultisample ||
        actualTarget == QAbstractTexture::Target2DMultisampleArray) {
        // Set samples count if multisampled texture
        // (multisampled textures don't have mipmaps)
        glTex->setSamples(m_properties.samples);
    } else if (m_properties.generateMipMaps) {
        glTex->setMipLevels(glTex->maximumMipLevels());
    } else {
        glTex->setAutoMipMapGenerationEnabled(false);
        if (glTex->hasFeature(QOpenGLTexture::TextureMipMapLevel)) {
            glTex->setMipBaseLevel(0);
            glTex->setMipMaxLevel(m_properties.mipLevels - 1);
        }
        glTex->setMipLevels(m_properties.mipLevels);
    }

    if (!glTex->create()) {
        qWarning() << Q_FUNC_INFO << "creating QOpenGLTexture failed";
        return nullptr;
    }

    return glTex;
}

void GLTexture::uploadGLTextureData()
{
    // Upload all QTexImageData set by the QTextureGenerator
    if (m_textureData) {
        const QVector<QTextureImageDataPtr> imgData = m_textureData->imageData();

        for (const QTextureImageDataPtr &data : imgData) {
            const int mipLevels = m_properties.generateMipMaps ? 1 : data->mipLevels();

            for (int layer = 0; layer < data->layers(); layer++) {
                for (int face = 0; face < data->faces(); face++) {
                    for (int level = 0; level < mipLevels; level++) {
                        // ensure we don't accidentally cause a detach / copy of the raw bytes
                        const QByteArray bytes(data->data(layer, face, level));
                        uploadGLData(m_gl, level, layer,
                                     static_cast<QOpenGLTexture::CubeMapFace>(QOpenGLTexture::CubeMapPositiveX + face),
                                     bytes, data);
                    }
                }
            }
        }
    }

    // Upload all QTexImageData references by the TextureImages
    for (int i = 0; i < std::min(m_images.size(), m_imageData.size()); i++) {
        const QTextureImageDataPtr &imgData = m_imageData.at(i);
        // Here the bytes in the QTextureImageData contain data for a single
        // layer, face or mip level, unlike the QTextureGenerator case where
        // they are in a single blob. Hence QTextureImageData::data() is not suitable.
        const QByteArray bytes(QTextureImageDataPrivate::get(imgData.get())->m_data);
        uploadGLData(m_gl, m_images[i].mipLevel, m_images[i].layer,
                     static_cast<QOpenGLTexture::CubeMapFace>(m_images[i].face),
                     bytes, imgData);
    }
    // Free up image data once content has been uploaded
    // Note: if data functor stores the data, this won't really free anything though
    m_imageData.clear();

    // Update data from TextureUpdates
    const QVector<QTextureDataUpdate> textureDataUpdates = std::move(m_pendingTextureDataUpdates);
    for (const QTextureDataUpdate &update : textureDataUpdates) {
        const QTextureImageDataPtr imgData = update.data();

        if (!imgData) {
            qWarning() << Q_FUNC_INFO << "QTextureDataUpdate no QTextureImageData set";
            continue;
        }

        const int xOffset = update.x();
        const int yOffset = update.y();
        const int zOffset = update.z();
        const int xExtent = xOffset + imgData->width();
        const int yExtent = yOffset + imgData->height();
        const int zExtent = zOffset + imgData->depth();

        // Check update is compatible with our texture
        if (xOffset >= m_gl->width() ||
            yOffset >= m_gl->height() ||
            zOffset >= m_gl->depth() ||
            xExtent > m_gl->width() ||
            yExtent > m_gl->height() ||
            zExtent > m_gl->depth() ||
            update.mipLevel() >= m_gl->mipLevels() ||
            update.layer() >= m_gl->layers()) {
            qWarning() << Q_FUNC_INFO << "QTextureDataUpdate incompatible with texture";
            continue;
        }

        const QByteArray bytes = (QTextureImageDataPrivate::get(imgData.get())->m_data);
        // Here the bytes in the QTextureImageData contain data for a single
        // layer, face or mip level, unlike the QTextureGenerator case where
        // they are in a single blob. Hence QTextureImageData::data() is not suitable.

        uploadGLData(m_gl,
                     update.mipLevel(), update.layer(),
                     static_cast<QOpenGLTexture::CubeMapFace>(update.face()),
                     xOffset, yOffset, zOffset,
                     bytes, imgData);
    }
}

void GLTexture::updateGLTextureParameters()
{
    const QAbstractTexture::Target actualTarget = m_properties.target;
    const bool isMultisampledTexture = (actualTarget == QAbstractTexture::Target2DMultisample ||
                                        actualTarget == QAbstractTexture::Target2DMultisampleArray);
    // Multisampled textures can only be accessed by texelFetch in shaders
    // and don't support wrap modes and mig/mag filtes
    if (isMultisampledTexture)
        return;

    m_gl->setWrapMode(QOpenGLTexture::DirectionS, static_cast<QOpenGLTexture::WrapMode>(m_parameters.wrapModeX));
    if (actualTarget != QAbstractTexture::Target1D &&
        actualTarget != QAbstractTexture::Target1DArray &&
        actualTarget != QAbstractTexture::TargetBuffer)
        m_gl->setWrapMode(QOpenGLTexture::DirectionT, static_cast<QOpenGLTexture::WrapMode>(m_parameters.wrapModeY));
    if (actualTarget == QAbstractTexture::Target3D)
        m_gl->setWrapMode(QOpenGLTexture::DirectionR, static_cast<QOpenGLTexture::WrapMode>(m_parameters.wrapModeZ));
    m_gl->setMinMagFilters(static_cast<QOpenGLTexture::Filter>(m_parameters.minificationFilter),
                           static_cast<QOpenGLTexture::Filter>(m_parameters.magnificationFilter));
    if (m_gl->hasFeature(QOpenGLTexture::AnisotropicFiltering))
        m_gl->setMaximumAnisotropy(m_parameters.maximumAnisotropy);
    if (m_gl->hasFeature(QOpenGLTexture::TextureComparisonOperators)) {
        m_gl->setComparisonFunction(static_cast<QOpenGLTexture::ComparisonFunction>(m_parameters.comparisonFunction));
        m_gl->setComparisonMode(static_cast<QOpenGLTexture::ComparisonMode>(m_parameters.comparisonMode));
    }
}

void GLTexture::introspectPropertiesFromSharedTextureId()
{
    // We know that the context is active when this function is called
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qWarning() << Q_FUNC_INFO << "requires an OpenGL context";
        return;
    }
    QOpenGLFunctions *gl = ctx->functions();

    // If the user has set the target format himself, we won't try to deduce it
    if (m_properties.target != QAbstractTexture::TargetAutomatic)
        return;

    const QAbstractTexture::Target targets[] = {
        QAbstractTexture::Target2D,
        QAbstractTexture::TargetCubeMap,
#ifndef QT_OPENGL_ES_2
        QAbstractTexture::Target1D,
        QAbstractTexture::Target1DArray,
        QAbstractTexture::Target3D,
        QAbstractTexture::Target2DArray,
        QAbstractTexture::TargetCubeMapArray,
        QAbstractTexture::Target2DMultisample,
        QAbstractTexture::Target2DMultisampleArray,
        QAbstractTexture::TargetRectangle,
        QAbstractTexture::TargetBuffer,
#endif
    };

#ifndef QT_OPENGL_ES_2
    // Try to find texture target with GL 4.5 functions
    const QPair<int, int> ctxGLVersion = ctx->format().version();
    if (ctxGLVersion.first > 4 || (ctxGLVersion.first == 4 && ctxGLVersion.second >= 5)) {
        // Only for GL 4.5+
#ifdef GL_TEXTURE_TARGET
        QOpenGLFunctions_4_5_Core *gl5 = ctx->versionFunctions<QOpenGLFunctions_4_5_Core>();
        if (gl5 != nullptr)
            gl5->glGetTextureParameteriv(m_sharedTextureId, GL_TEXTURE_TARGET, reinterpret_cast<int *>(&m_properties.target));
#endif
    }
#endif

    // If GL 4.5 function unavailable or not working, try a slower way
    if (m_properties.target == QAbstractTexture::TargetAutomatic) {
        //    // OpenGL offers no proper way of querying for the target of a texture given its id
        gl->glActiveTexture(GL_TEXTURE0);

        const GLenum targetBindings[] = {
            GL_TEXTURE_BINDING_2D,
            GL_TEXTURE_BINDING_CUBE_MAP,
#ifndef QT_OPENGL_ES_2
            GL_TEXTURE_BINDING_1D,
            GL_TEXTURE_BINDING_1D_ARRAY,
            GL_TEXTURE_BINDING_3D,
            GL_TEXTURE_BINDING_2D_ARRAY,
            GL_TEXTURE_BINDING_CUBE_MAP_ARRAY,
            GL_TEXTURE_BINDING_2D_MULTISAMPLE,
            GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,
            GL_TEXTURE_BINDING_RECTANGLE,
            GL_TEXTURE_BINDING_BUFFER
#endif
        };

        Q_STATIC_ASSERT(sizeof(targetBindings) / sizeof(targetBindings[0]) == sizeof(targets) / sizeof(targets[0]));

        for (uint i = 0; i < sizeof(targetBindings) / sizeof(targetBindings[0]); ++i) {
            const int target = targets[i];
            gl->glBindTexture(target, m_sharedTextureId);
            int boundId = 0;
            gl->glGetIntegerv(targetBindings[i], &boundId);
            gl->glBindTexture(target, 0);
            if (boundId == m_sharedTextureId) {
                m_properties.target = static_cast<QAbstractTexture::Target>(target);
                break;
            }
        }
    }

    // Return early if we weren't able to find texture target
    if (std::find(std::begin(targets), std::end(targets), m_properties.target) == std::end(targets)) {
        qWarning() << "Unable to determine texture target for shared GL texture";
        return;
    }

    // Bind texture once we know its target
    gl->glBindTexture(m_properties.target, m_sharedTextureId);

    // TO DO: Improve by using glGetTextureParameters when available which
    // support direct state access
#ifndef GL_TEXTURE_MAX_LEVEL
#define GL_TEXTURE_MAX_LEVEL        0x813D
#endif

#ifndef GL_TEXTURE_WRAP_R
#define GL_TEXTURE_WRAP_R           0x8072
#endif

    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_MAX_LEVEL, reinterpret_cast<int *>(&m_properties.mipLevels));
    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_MIN_FILTER, reinterpret_cast<int *>(&m_parameters.minificationFilter));
    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_MAG_FILTER, reinterpret_cast<int *>(&m_parameters.magnificationFilter));
    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_WRAP_R, reinterpret_cast<int *>(&m_parameters.wrapModeX));
    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_WRAP_S, reinterpret_cast<int *>(&m_parameters.wrapModeY));
    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_WRAP_T, reinterpret_cast<int *>(&m_parameters.wrapModeZ));

#ifndef QT_OPENGL_ES_2
    // Try to retrieve dimensions (not available on ES 2.0)
    if (!ctx->isOpenGLES()) {
        QOpenGLFunctions_3_1 *gl3 = ctx->versionFunctions<QOpenGLFunctions_3_1>();
        if (!gl3) {
            qWarning() << "Failed to retrieve shared texture dimensions";
            return;
        }

        gl3->glGetTexLevelParameteriv(int(m_properties.target), 0, GL_TEXTURE_WIDTH, reinterpret_cast<int *>(&m_properties.width));
        gl3->glGetTexLevelParameteriv(int(m_properties.target), 0, GL_TEXTURE_HEIGHT, reinterpret_cast<int *>(&m_properties.height));
        gl3->glGetTexLevelParameteriv(int(m_properties.target), 0, GL_TEXTURE_DEPTH, reinterpret_cast<int *>(&m_properties.depth));
        gl3->glGetTexLevelParameteriv(int(m_properties.target), 0, GL_TEXTURE_INTERNAL_FORMAT, reinterpret_cast<int *>(&m_properties.format));
    }
#endif

    gl->glBindTexture(m_properties.target, 0);
}

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
