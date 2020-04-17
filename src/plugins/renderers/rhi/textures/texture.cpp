/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
#include "texture_p.h"

#include <private/qdebug_p.h>
#include <private/qrhi_p.h>
#include <QDebug>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qtexturedata.h>
#include <Qt3DRender/qtextureimagedata.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/qabstracttexture_p.h>
#include <Qt3DRender/private/qtextureimagedata_p.h>
#include <renderbuffer_p.h>
#include <submissioncontext_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {
namespace Rhi {

namespace {

bool issRGBFormat(QAbstractTexture::TextureFormat format) noexcept
{
    switch (format) {
    case QAbstractTexture::SRGB8:
    case QAbstractTexture::SRGB8_ETC2:
    case QAbstractTexture::SRGB8_PunchThrough_Alpha1_ETC2:
        return true;
    default:
        return false;
    }
}

QRhiTexture::Format rhiFormatFromTextureFormat(QAbstractTexture::TextureFormat format) noexcept
{
    switch (format) {
    case QAbstractTexture::RGBA8_UNorm:
    case QAbstractTexture::SRGB8:
        return QRhiTexture::RGBA8;
    case QAbstractTexture::R8_UNorm:
        return QRhiTexture::R8;
    case QAbstractTexture::R16_UNorm:
        return QRhiTexture::R16;
    case QAbstractTexture::RGBA16F:
        return QRhiTexture::RGBA16F;
    case QAbstractTexture::RGBA32F:
        return QRhiTexture::RGBA32F;
    case QAbstractTexture::R16F:
        return QRhiTexture::R16F;
    case QAbstractTexture::R32F:
        return QRhiTexture::R32F;
    case QAbstractTexture::D16:
        return QRhiTexture::D16;
    case QAbstractTexture::D32F:
        return QRhiTexture::D32F;
    case QAbstractTexture::RGB_DXT1:
    case QAbstractTexture::RGBA_DXT1:
        return QRhiTexture::BC1;
    case QAbstractTexture::RGBA_DXT3:
        return QRhiTexture::BC2;
    case QAbstractTexture::RGBA_DXT5:
        return QRhiTexture::BC3;
    case QAbstractTexture::RGB8_ETC2:
    case QAbstractTexture::SRGB8_ETC2:
        return QRhiTexture::ETC2_RGB8;
    case QAbstractTexture::RGB8_PunchThrough_Alpha1_ETC2:
    case QAbstractTexture::SRGB8_PunchThrough_Alpha1_ETC2:
        return QRhiTexture::ETC2_RGB8A1;
    case QAbstractTexture::RGBA8_ETC2_EAC:
        return QRhiTexture::ETC2_RGBA8;
    default:
        Q_UNREACHABLE();
        return QRhiTexture::UnknownFormat;
    }
}

QRhiSampler::Filter rhiFilterFromTextureFilter(QAbstractTexture::Filter filter) noexcept
{
    switch (filter) {
    case QAbstractTexture::Nearest:
    case QAbstractTexture::NearestMipMapNearest:
    case QAbstractTexture::NearestMipMapLinear:
        return QRhiSampler::Nearest;
    case QAbstractTexture::Linear:
    case QAbstractTexture::LinearMipMapNearest:
    case QAbstractTexture::LinearMipMapLinear:
        return QRhiSampler::Linear;
    default:
        Q_UNREACHABLE();
        return QRhiSampler::Nearest;
    }
}

QRhiSampler::Filter rhiMipMapFilterFromTextureFilter(QAbstractTexture::Filter filter) noexcept
{
    switch (filter) {
    case QAbstractTexture::NearestMipMapNearest:
    case QAbstractTexture::LinearMipMapNearest:
        return QRhiSampler::Nearest;
    case QAbstractTexture::Linear:
    case QAbstractTexture::NearestMipMapLinear:
    case QAbstractTexture::LinearMipMapLinear:
        return QRhiSampler::Linear;
    default:
        Q_UNREACHABLE();
        return QRhiSampler::None;
    }
}

std::tuple<QRhiSampler::AddressMode, QRhiSampler::AddressMode, QRhiSampler::AddressMode>
rhiWrapModeFromTextureWrapMode(QTextureWrapMode::WrapMode x, QTextureWrapMode::WrapMode y,
                               QTextureWrapMode::WrapMode z) noexcept
{
    auto toRhiAddress = [](QTextureWrapMode::WrapMode mode) noexcept {
        switch (mode) {
        case Qt3DRender::QTextureWrapMode::Repeat:
            return QRhiSampler::Repeat;
        case Qt3DRender::QTextureWrapMode::ClampToEdge:
        case Qt3DRender::QTextureWrapMode::ClampToBorder:
            return QRhiSampler::ClampToEdge;
        case Qt3DRender::QTextureWrapMode::MirroredRepeat:
            return QRhiSampler::Mirror;
        default:
            Q_UNREACHABLE();
        }
    };

    return { toRhiAddress(x), toRhiAddress(y), toRhiAddress(z) };
}

QRhiSampler::CompareOp
rhiCompareOpFromTextureCompareOp(QAbstractTexture::ComparisonFunction mode) noexcept
{
    switch (mode) {
    case QAbstractTexture::CompareLessEqual:
        return QRhiSampler::LessOrEqual;
    case QAbstractTexture::CompareGreaterEqual:
        return QRhiSampler::GreaterOrEqual;
    case QAbstractTexture::CompareLess:
        return QRhiSampler::Less;
    case QAbstractTexture::CompareGreater:
        return QRhiSampler::Greater;
    case QAbstractTexture::CompareEqual:
        return QRhiSampler::Equal;
    case QAbstractTexture::CommpareNotEqual:
        return QRhiSampler::NotEqual;
    case QAbstractTexture::CompareAlways:
        return QRhiSampler::Always;
    case QAbstractTexture::CompareNever:
        return QRhiSampler::Never;
    default:
        return QRhiSampler::Always;
    }
}

// This uploadGLData where the data is a fullsize subimage
// as QOpenGLTexture doesn't allow partial subimage uploads
QRhiTextureUploadEntry createUploadEntry(int level, int layer, const QByteArray &bytes) noexcept
{
    QRhiTextureSubresourceUploadDescription description;
    description.setData(bytes);
    return QRhiTextureUploadEntry(layer, level, description);
}

template<typename F>
void filterLayersAndFaces(const QTextureImageData &data, F f)
{
    const int layers = data.layers();
    const int faces = data.faces();
    const int miplevels = data.mipLevels();

    if (layers == 1 && faces == 1) {
        for (int level = 0; level < miplevels; level++) {
            f(createUploadEntry(level, 0, data.data(0, 0, level)));
        }
    } else if (layers > 1 && faces == 1) {
        qWarning() << Q_FUNC_INFO << "Unsupported case, see QTBUG-83343";
        /*
        for (int layer = 0; layer < data.layers(); layer++) {
            for (int level = 0; level < mipLevels; level++) {
                f(createUploadEntry(level, layer, data.data(layer, 0, level)));
            }
        }
        */
    } else if (faces > 1 && layers == 1) {
        // Mip levels do not seem to be supported by cubemaps...
        for (int face = 0; face < data.faces(); face++) {
            f(createUploadEntry(0, face, data.data(0, face, 0)));
        }
    } else {
        qWarning() << Q_FUNC_INFO << "Unsupported case";
    }
}

template<typename F>
void filterLayerAndFace(int layer, int face, F f)
{
    if (layer == 0 && face == 0) {
        f(0);
    } else if (layer > 0 && face == 0) {
        qWarning() << Q_FUNC_INFO << "Unsupported case, see QTBUG-83343";
        // f(layer);
    } else if (layer == 0 && face > 0) {
        f(face);
    } else {
        qWarning() << Q_FUNC_INFO << "Unsupported case";
    }
}

// For partial sub image uploads
QRhiTextureUploadEntry createUploadEntry(int mipLevel, int layer, int xOffset, int yOffset,
                                         int zOffset, const QByteArray &bytes,
                                         const QTextureImageDataPtr &data) noexcept
{
    QRhiTextureSubresourceUploadDescription description;
    description.setData(bytes);
    description.setSourceTopLeft(QPoint(xOffset, yOffset));
    return QRhiTextureUploadEntry(layer, mipLevel, description);
}

} // anonymous

RHITexture::RHITexture()
    : m_dirtyFlags(None),
      m_rhi(nullptr),
      m_rhiSampler(nullptr),
      m_renderBuffer(nullptr),
      m_dataFunctor(),
      m_pendingDataFunctor(nullptr),
      m_sharedTextureId(-1),
      m_externalRendering(false),
      m_wasTextureRecreated(false)
{
}

RHITexture::~RHITexture() { }

// Must be called from RenderThread with active GL context
void RHITexture::destroy()
{
    delete m_rhi;
    m_rhi = nullptr;
    delete m_rhiSampler;
    m_rhiSampler = nullptr;
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

bool RHITexture::loadTextureDataFromGenerator()
{
    m_textureData = m_dataFunctor->operator()();
    // if there is a texture generator, most properties will be defined by it
    if (m_textureData) {
        const QAbstractTexture::Target target = m_textureData->target();

        // If both target and functor return Automatic we are still
        // probably loading the texture, return false
        if (m_properties.target == QAbstractTexture::TargetAutomatic
            && target == QAbstractTexture::TargetAutomatic) {
            m_textureData.reset();
            return false;
        }

        if (m_properties.target != QAbstractTexture::TargetAutomatic
            && target != QAbstractTexture::TargetAutomatic && m_properties.target != target) {
            qWarning() << Q_FUNC_INFO
                       << "Generator and Properties not requesting the same texture target";
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

        if (!imageData.empty()) {
            // Set the mips level based on the first image if autoMipMapGeneration is disabled
            if (!m_properties.generateMipMaps)
                m_properties.mipLevels = imageData.first()->mipLevels();
        }
    }
    return !m_textureData.isNull();
}

void RHITexture::loadTextureDataFromImages()
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
        if (!m_textureData && img.layer == 0 && img.mipLevel == 0
            && img.face == QAbstractTexture::CubeMapPositiveX) {
            if (imgData->width() != -1 && imgData->height() != -1 && imgData->depth() != -1) {
                m_properties.width = imgData->width();
                m_properties.height = imgData->height();
                m_properties.depth = imgData->depth();
            }
            // Set the format of the texture if the texture format is set to Automatic
            if (m_properties.format == QAbstractTexture::Automatic) {
                m_properties.format =
                        static_cast<QAbstractTexture::TextureFormat>(imgData->format());
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
RHITexture::TextureUpdateInfo RHITexture::createOrUpdateRhiTexture(SubmissionContext *ctx)
{
    TextureUpdateInfo textureInfo;
    m_wasTextureRecreated = false;

    const bool hasSharedTextureId = m_sharedTextureId > 0;
    // Only load texture data if we are not using a sharedTextureId
    // Check if dataFunctor or images have changed
    if (!hasSharedTextureId) {
        // If dataFunctor exists and we have no data and it hasn´t run yet
        if (m_dataFunctor && !m_textureData && m_dataFunctor.get() != m_pendingDataFunctor) {
            const bool successfullyLoadedTextureData = loadTextureDataFromGenerator();
            // If successful, m_textureData has content
            if (successfullyLoadedTextureData) {
                setDirtyFlag(Properties, true);
                setDirtyFlag(TextureData, true);
            } else {
                if (m_pendingDataFunctor != m_dataFunctor.get()) {
                    qWarning() << "[Qt3DRender::RHITexture] No QTextureData generated from Texture "
                                  "Generator yet. Texture will be invalid for this frame";
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
        if (m_properties.target == QAbstractTexture::TargetAutomatic
            || m_properties.format == QAbstractTexture::Automatic
            || m_properties.format == QAbstractTexture::NoFormat) {
            textureInfo.properties.status = QAbstractTexture::Error;
            return textureInfo;
        }
    }

    // If the properties changed or texture has become a shared texture from a
    // 3rd party engine, we need to destroy and maybe re-allocate the texture
    if (testDirtyFlag(Properties) || testDirtyFlag(SharedTextureId)) {
        delete m_rhi;
        m_rhi = nullptr;
        textureInfo.wasUpdated = true;
        // If we are destroyed because of some property change but still have (some) of
        // our content data make sure we are marked for upload
        // TO DO: We should actually check if the textureData is still correct
        // in regard to the size, target and format of the texture though.
        if (!testDirtyFlag(SharedTextureId)
            && (m_textureData || !m_imageData.empty() || !m_pendingTextureDataUpdates.empty()))
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
        if (!m_rhi) {
            m_rhi = buildRhiTexture(ctx);
            if (!m_rhi) {
                qWarning() << "[Qt3DRender::RHITexture] failed to create texture";
                textureInfo.properties.status = QAbstractTexture::Error;
                return textureInfo;
            }
            m_wasTextureRecreated = true;
        }

        textureInfo.texture = m_rhi;

        // need to (re-)upload texture data?
        const bool needsUpload = testDirtyFlag(TextureData);
        if (needsUpload) {
            uploadRhiTextureData(ctx);
            setDirtyFlag(TextureData, false);
        }

        // need to set texture parameters?
        if (testDirtyFlag(Properties) || testDirtyFlag(Parameters)) {
            updateRhiTextureParameters(ctx);
            setDirtyFlag(Properties, false);
            setDirtyFlag(Parameters, false);
        }
    }

    textureInfo.properties = m_properties;

    return textureInfo;
}

RenderBuffer *RHITexture::getOrCreateRenderBuffer()
{
    if (m_dataFunctor && !m_textureData) {
        m_textureData = m_dataFunctor->operator()();
        if (m_textureData) {
            if (m_properties.target != QAbstractTexture::TargetAutomatic)
                qWarning() << "[Qt3DRender::RHITexture] [renderbuffer] When a texture provides a "
                              "generator, it's target is expected to be TargetAutomatic";

            m_properties.width = m_textureData->width();
            m_properties.height = m_textureData->height();
            m_properties.format = m_textureData->format();

            setDirtyFlag(Properties);
        } else {
            if (m_pendingDataFunctor != m_dataFunctor.get()) {
                qWarning() << "[Qt3DRender::RHITexture] [renderbuffer] No QTextureData generated "
                              "from Texture Generator yet. Texture will be invalid for this frame";
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
        m_renderBuffer =
                new RenderBuffer(m_properties.width, m_properties.height, m_properties.format);

    setDirtyFlag(Properties, false);
    setDirtyFlag(Parameters, false);

    return m_renderBuffer;
}

// This must be called from the RenderThread
// So RHITexture release from the manager can only be done from that thread
void RHITexture::cleanup()
{
    destroy();
}

void RHITexture::setParameters(const TextureParameters &params)
{
    if (m_parameters != params) {
        m_parameters = params;
        setDirtyFlag(Parameters);
    }
}

void RHITexture::setProperties(const TextureProperties &props)
{
    if (m_properties != props) {
        m_properties = props;
        setDirtyFlag(Properties);
    }
}

void RHITexture::setImages(const QVector<Image> &images)
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

void RHITexture::setGenerator(const QTextureGeneratorPtr &generator)
{
    m_textureData.reset();
    m_dataFunctor = generator;
    m_pendingDataFunctor = nullptr;
    requestUpload();
}

void RHITexture::setSharedTextureId(int textureId)
{
    if (m_sharedTextureId != textureId) {
        m_sharedTextureId = textureId;
        setDirtyFlag(SharedTextureId);
    }
}

void RHITexture::addTextureDataUpdates(const QVector<QTextureDataUpdate> &updates)
{
    m_pendingTextureDataUpdates += updates;
    requestUpload();
}

// Return nullptr if
// - context cannot be obtained
// - texture hasn't yet been loaded
QRhiTexture *RHITexture::buildRhiTexture(SubmissionContext *ctx)
{
    const QAbstractTexture::Target actualTarget = m_properties.target;
    if (actualTarget == QAbstractTexture::TargetAutomatic) {
        // If the target is automatic at this point, it means that the texture
        // hasn't been loaded yet (case of remote urls) and that loading failed
        // and that target format couldn't be deduced
        return nullptr;
    }

    const QRhiTexture::Format rhiFormat = rhiFormatFromTextureFormat(m_properties.format);
    const QSize pixelSize(m_properties.width, m_properties.height);
    QRhiTexture::Flags rhiFlags {};
    int sampleCount = 1;

    const bool issRGB8Format = issRGBFormat(m_properties.format);
    if (issRGB8Format)
        rhiFlags |= QRhiTexture::sRGB;

    if (actualTarget == QAbstractTexture::Target2DMultisample
        || actualTarget == QAbstractTexture::Target2DMultisampleArray) {
        // Set samples count if multisampled texture
        // (multisampled textures don't have mipmaps)
        sampleCount = m_properties.samples;
    }

    switch (actualTarget) {
    case QAbstractTexture::TargetCubeMap:
    case QAbstractTexture::TargetCubeMapArray: {
        rhiFlags |= QRhiTexture::CubeMap;
        break;
    }
    default: {
        // Mipmaps don't see to work with cubemaps at the moment
        if (m_properties.generateMipMaps) {
            rhiFlags |= QRhiTexture::UsedWithGenerateMips;
            rhiFlags |= QRhiTexture::MipMapped;
        } else if (m_properties.mipLevels > 1) {
            rhiFlags |= QRhiTexture::MipMapped;
        }
        break;
    }
    }

    QRhiTexture *rhiTexture = ctx->rhi()->newTexture(rhiFormat, pixelSize, sampleCount, rhiFlags);

    if (!rhiTexture->build()) {
        qWarning() << Q_FUNC_INFO << "creating QRhiTexture failed";
        delete rhiTexture;
        return nullptr;
    }
    return rhiTexture;
}

void RHITexture::uploadRhiTextureData(SubmissionContext *ctx)
{
    QVarLengthArray<QRhiTextureUploadEntry> uploadEntries;

    // Upload all QTexImageData set by the QTextureGenerator
    if (m_textureData) {
        const QVector<QTextureImageDataPtr> &imgData = m_textureData->imageData();

        for (const QTextureImageDataPtr &data : imgData) {
            const int mipLevels = data->mipLevels();
            Q_ASSERT(mipLevels <= ctx->rhi()->mipLevelsForSize({ data->width(), data->height() }));

            filterLayersAndFaces(*data, [&](QRhiTextureUploadEntry &&entry) {
                uploadEntries.push_back(std::move(entry));
            });
        }
    }

    // Upload all QTexImageData references by the TextureImages
    for (int i = 0; i < std::min(m_images.size(), m_imageData.size()); i++) {
        const QTextureImageDataPtr &imgData = m_imageData.at(i);
        // Here the bytes in the QTextureImageData contain data for a single
        // layer, face or mip level, unlike the QTextureGenerator case where
        // they are in a single blob. Hence QTextureImageData::data() is not suitable.
        const QByteArray bytes(QTextureImageDataPrivate::get(imgData.get())->m_data);
        const int layer = m_images[i].layer;
        const int face = m_images[i].face;
        filterLayerAndFace(layer, face, [&](int rhiLayer) {
            uploadEntries.push_back(createUploadEntry(m_images[i].mipLevel, rhiLayer, bytes));
        });
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

        // TO DO: There's currently no way to check the depth of an existing QRhiTexture
        const int xOffset = update.x();
        const int yOffset = update.y();
        const int xExtent = xOffset + imgData->width();
        const int yExtent = yOffset + imgData->height();

        // Check update is compatible with our texture
        if (xOffset >= m_rhi->pixelSize().width() || yOffset >= m_rhi->pixelSize().height()
            || xExtent > m_rhi->pixelSize().width() || yExtent > m_rhi->pixelSize().height()) {
            qWarning() << Q_FUNC_INFO << "QTextureDataUpdate incompatible with texture";
            continue;
        }

        const QByteArray bytes = (QTextureImageDataPrivate::get(imgData.get())->m_data);
        // Here the bytes in the QTextureImageData contain data for a single
        // layer, face or mip level, unlike the QTextureGenerator case where
        // they are in a single blob. Hence QTextureImageData::data() is not suitable.

        const int layer = update.layer();
        const int face = update.face();
        filterLayerAndFace(layer, face, [&](int rhiLayer) {
            const QRhiTextureUploadEntry entry = createUploadEntry(
                    update.mipLevel(), rhiLayer, xOffset, yOffset, 0, bytes, imgData);
            uploadEntries.push_back(entry);
        });
    }

    QRhiTextureUploadDescription uploadDescription;
    uploadDescription.setEntries(uploadEntries.begin(), uploadEntries.end());

    ctx->m_currentUpdates->uploadTexture(m_rhi, uploadDescription);
    if (m_properties.generateMipMaps)
        ctx->m_currentUpdates->generateMips(m_rhi);
}

void RHITexture::updateRhiTextureParameters(SubmissionContext *ctx)
{
    const QAbstractTexture::Target actualTarget = m_properties.target;
    const bool isMultisampledTexture =
            (actualTarget == QAbstractTexture::Target2DMultisample
             || actualTarget == QAbstractTexture::Target2DMultisampleArray);
    // Multisampled textures can only be accessed by texelFetch in shaders
    // and don't support wrap modes and mig/mag filtes
    if (isMultisampledTexture)
        return;

    // TO DO:
    if (m_rhiSampler) {
        delete m_rhiSampler;
        m_rhiSampler = nullptr;
    }

    const QRhiSampler::Filter magFilter =
            rhiFilterFromTextureFilter(m_parameters.magnificationFilter);
    const QRhiSampler::Filter minFilter =
            rhiFilterFromTextureFilter(m_parameters.minificationFilter);
    const QRhiSampler::Filter mipMapFilter =
            rhiMipMapFilterFromTextureFilter(m_parameters.magnificationFilter);
    const auto wrapMode = rhiWrapModeFromTextureWrapMode(
            m_parameters.wrapModeX, m_parameters.wrapModeY, m_parameters.wrapModeZ);
    const QRhiSampler::CompareOp compareOp =
            rhiCompareOpFromTextureCompareOp(m_parameters.comparisonFunction);
    m_rhiSampler = ctx->rhi()->newSampler(magFilter, minFilter, mipMapFilter, std::get<0>(wrapMode),
                                          std::get<1>(wrapMode), std::get<2>(wrapMode));

    m_rhiSampler->setTextureCompareOp(compareOp);

    if (!m_rhiSampler->build()) {
        qDebug("Could not build RHI texture sampler");
    }
}

void RHITexture::introspectPropertiesFromSharedTextureId()
{
    //    // We know that the context is active when this function is called
    //    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    //    if (!ctx) {
    //        qWarning() << Q_FUNC_INFO << "requires an OpenGL context";
    //        return;
    //    }
    //    QOpenGLFunctions *gl = ctx->functions();

    //    // If the user has set the target format himself, we won't try to deduce it
    //    if (m_properties.target != QAbstractTexture::TargetAutomatic)
    //        return;

    //    const QAbstractTexture::Target targets[] = {
    //        QAbstractTexture::Target2D,
    //        QAbstractTexture::TargetCubeMap,
    //#ifndef QT_OPENGL_ES_2
    //        QAbstractTexture::Target1D,
    //        QAbstractTexture::Target1DArray,
    //        QAbstractTexture::Target3D,
    //        QAbstractTexture::Target2DArray,
    //        QAbstractTexture::TargetCubeMapArray,
    //        QAbstractTexture::Target2DMultisample,
    //        QAbstractTexture::Target2DMultisampleArray,
    //        QAbstractTexture::TargetRectangle,
    //        QAbstractTexture::TargetBuffer,
    //#endif
    //    };

    //#ifndef QT_OPENGL_ES_2
    //    // Try to find texture target with GL 4.5 functions
    //    const QPair<int, int> ctxGLVersion = ctx->format().version();
    //    if (ctxGLVersion.first > 4 || (ctxGLVersion.first == 4 && ctxGLVersion.second >= 5)) {
    //        // Only for GL 4.5+
    //#ifdef GL_TEXTURE_TARGET
    //        QOpenGLFunctions_4_5_Core *gl5 = ctx->versionFunctions<QOpenGLFunctions_4_5_Core>();
    //        if (gl5 != nullptr)
    //            gl5->glGetTextureParameteriv(m_sharedTextureId, GL_TEXTURE_TARGET,
    //            reinterpret_cast<int *>(&m_properties.target));
    //#endif
    //    }
    //#endif

    //    // If GL 4.5 function unavailable or not working, try a slower way
    //    if (m_properties.target == QAbstractTexture::TargetAutomatic) {
    //        //    // OpenGL offers no proper way of querying for the target of a texture given its
    //        id gl->glActiveTexture(GL_TEXTURE0);

    //        const GLenum targetBindings[] = {
    //            GL_TEXTURE_BINDING_2D,
    //            GL_TEXTURE_BINDING_CUBE_MAP,
    //#ifndef QT_OPENGL_ES_2
    //            GL_TEXTURE_BINDING_1D,
    //            GL_TEXTURE_BINDING_1D_ARRAY,
    //            GL_TEXTURE_BINDING_3D,
    //            GL_TEXTURE_BINDING_2D_ARRAY,
    //            GL_TEXTURE_BINDING_CUBE_MAP_ARRAY,
    //            GL_TEXTURE_BINDING_2D_MULTISAMPLE,
    //            GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,
    //            GL_TEXTURE_BINDING_RECTANGLE,
    //            GL_TEXTURE_BINDING_BUFFER
    //#endif
    //        };

    //        Q_ASSERT(sizeof(targetBindings) / sizeof(targetBindings[0] == sizeof(targets) /
    //        sizeof(targets[0])));

    //        for (uint i = 0; i < sizeof(targetBindings) / sizeof(targetBindings[0]); ++i) {
    //            const int target = targets[i];
    //            gl->glBindTexture(target, m_sharedTextureId);
    //            int boundId = 0;
    //            gl->glGetIntegerv(targetBindings[i], &boundId);
    //            gl->glBindTexture(target, 0);
    //            if (boundId == m_sharedTextureId) {
    //                m_properties.target = static_cast<QAbstractTexture::Target>(target);
    //                break;
    //            }
    //        }
    //    }

    //    // Return early if we weren't able to find texture target
    //    if (std::find(std::begin(targets), std::end(targets), m_properties.target) ==
    //    std::end(targets)) {
    //        qWarning() << "Unable to determine texture target for shared GL texture";
    //        return;
    //    }

    //    // Bind texture once we know its target
    //    gl->glBindTexture(m_properties.target, m_sharedTextureId);

    //    // TO DO: Improve by using glGetTextureParameters when available which
    //    // support direct state access
    //#ifndef GL_TEXTURE_MAX_LEVEL
    //#define GL_TEXTURE_MAX_LEVEL        0x813D
    //#endif

    //#ifndef GL_TEXTURE_WRAP_R
    //#define GL_TEXTURE_WRAP_R           0x8072
    //#endif

    //    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_MAX_LEVEL,
    //    reinterpret_cast<int *>(&m_properties.mipLevels));
    //    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_MIN_FILTER,
    //    reinterpret_cast<int *>(&m_parameters.minificationFilter));
    //    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_MAG_FILTER,
    //    reinterpret_cast<int *>(&m_parameters.magnificationFilter));
    //    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_WRAP_R, reinterpret_cast<int
    //    *>(&m_parameters.wrapModeX)); gl->glGetTexParameteriv(int(m_properties.target),
    //    GL_TEXTURE_WRAP_S, reinterpret_cast<int *>(&m_parameters.wrapModeY));
    //    gl->glGetTexParameteriv(int(m_properties.target), GL_TEXTURE_WRAP_T, reinterpret_cast<int
    //    *>(&m_parameters.wrapModeZ));

    //#ifndef QT_OPENGL_ES_2
    //    // Try to retrieve dimensions (not available on ES 2.0)
    //    if (!ctx->isOpenGLES()) {
    //        QOpenGLFunctions_3_1 *gl3 = ctx->versionFunctions<QOpenGLFunctions_3_1>();
    //        if (!gl3) {
    //            qWarning() << "Failed to retrieve shared texture dimensions";
    //            return;
    //        }

    //        gl3->glGetTexLevelParameteriv(int(m_properties.target), 0, GL_TEXTURE_WIDTH,
    //        reinterpret_cast<int *>(&m_properties.width));
    //        gl3->glGetTexLevelParameteriv(int(m_properties.target), 0, GL_TEXTURE_HEIGHT,
    //        reinterpret_cast<int *>(&m_properties.height));
    //        gl3->glGetTexLevelParameteriv(int(m_properties.target), 0, GL_TEXTURE_DEPTH,
    //        reinterpret_cast<int *>(&m_properties.depth));
    //        gl3->glGetTexLevelParameteriv(int(m_properties.target), 0, GL_TEXTURE_INTERNAL_FORMAT,
    //        reinterpret_cast<int *>(&m_properties.format));
    //    }
    //#endif

    //    gl->glBindTexture(m_properties.target, 0);
}

} // namespace Rhi
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
