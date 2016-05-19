/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qtextureimagedata.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>
#include <Qt3DRender/private/qabstracttexture_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

/* A Texture can get its data in two complementary ways
 * - Usually when a texture is created it is associated with a various number of
 *   QTextureImages <-> TextureImage which will internally contain a set of QTexImageData
 * - A QTexture can also provide a QTextureGenerator functor which might also
 *   return a vector of QTexImageData
 * So internally a Texture has a vector of HTextureImage which allow to retrieve a TextureImage and HTextureData
 * but also a vector of HTextureData filled by the QTextureGenerator if present.
 * From a memory management point of view, the texture needs to make sure it releases the HTextureData
 * that were generated from the QTextureGenerator as these are not shared and belong to the Texture object.
 * The HTextureData associated to a HTextureImage are managed by the TextureImage.
 */

Texture::Texture()
    : BackendNode()
    , m_gl(nullptr)
    , m_width(1)
    , m_height(1)
    , m_depth(1)
    , m_layers(1)
    , m_mipLevels(1)
    , m_generateMipMaps(false)
    , m_target(QAbstractTexture::Target2D)
    , m_format(QAbstractTexture::RGBA8_UNorm)
    , m_magnificationFilter(QAbstractTexture::Nearest)
    , m_minificationFilter(QAbstractTexture::Nearest)
    , m_wrapModeX(QTextureWrapMode::ClampToEdge)
    , m_wrapModeY(QTextureWrapMode::ClampToEdge)
    , m_wrapModeZ(QTextureWrapMode::ClampToEdge)
    , m_maximumAnisotropy(1.0f)
    , m_comparisonFunction(QAbstractTexture::CompareLessEqual)
    , m_comparisonMode(QAbstractTexture::CompareNone)
    , m_isDirty(false)
    , m_filtersAndWrapUpdated(false)
    , m_dataUploadRequired(false)
    , m_textureDNA(0)
    , m_textureManager(nullptr)
    , m_textureImageManager(nullptr)
    , m_textureDataManager(nullptr)
{
    // We need backend -> frontend notifications to update the status of the texture
}

Texture::~Texture()
{
    // Release the texture data handles that may have been loaded
    // by a QTextureGenerator functor
    releaseTextureDataHandles();
}

void Texture::cleanup()
{
    // Release the texture data handles that may have been loaded
    // by a QTextureGenerator functor
    releaseTextureDataHandles();

    QBackendNode::setEnabled(false);
    m_gl = nullptr;
    m_width = 1;
    m_height = 1;
    m_depth = 1;
    m_layers = 1;
    m_mipLevels = 1;
    m_generateMipMaps = false;
    m_target = QAbstractTexture::Target2D;
    m_format = QAbstractTexture::RGBA8_UNorm;
    m_magnificationFilter = QAbstractTexture::Nearest;
    m_minificationFilter = QAbstractTexture::Nearest;
    m_wrapModeX = QTextureWrapMode::ClampToEdge;
    m_wrapModeY = QTextureWrapMode::ClampToEdge;
    m_wrapModeZ = QTextureWrapMode::ClampToEdge;
    m_maximumAnisotropy = 1.0f;
    m_comparisonFunction = QAbstractTexture::CompareLessEqual;
    m_comparisonMode = QAbstractTexture::CompareNone;
    m_isDirty = false;
    m_filtersAndWrapUpdated = false;
    m_dataUploadRequired = false;
    m_textureDNA = 0;
    m_textureImages.clear();
    m_textureManager = nullptr;
    m_textureImageManager = nullptr;
    m_textureDataManager = nullptr;
    m_dataFunctor.clear();
}

// AspectThread
void Texture::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QAbstractTextureData>>(change);
    const auto &data = typedChange->data;

    QMutexLocker lock(&m_lock);
    m_target = data.target;
    m_format = data.format;
    m_width = data.width;
    m_height = data.height;
    m_depth = data.depth;
    m_generateMipMaps = data.autoMipMap;
    m_minificationFilter = data.minFilter;
    m_magnificationFilter = data.magFilter;
    m_wrapModeX = data.wrapModeX;
    m_wrapModeY = data.wrapModeY;
    m_wrapModeZ = data.wrapModeZ;
    m_maximumAnisotropy = data.maximumAnisotropy;
    m_comparisonFunction = data.comparisonFunction;
    m_comparisonMode = data.comparisonMode;
    m_layers = data.layers;
    m_dataFunctor = data.dataFunctor;
    if (m_dataFunctor)
        addToPendingTextureJobs();

    // TODO: Handle texture image ids better. At the moment we rely upon the assumption
    // in the TextureImage that its parent is a Texture. Better to set the ids from here
    // I think, and do it consistently with other types that refer to other nodes.
    //data.textureImageIds

    m_isDirty = true;
}

// RenderTread
QOpenGLTexture *Texture::getOrCreateGLTexture()
{
    // m_gl HAS to be destroyed in the OpenGL Thread
    // Will be recreated with updated values the next time
    // buildGLTexture is called


    // getOrCreateGLTexture is called by the OpenGL Render Thread
    // sceneChangeEvent is called by the QAspectThread
    // only the OpenGL Render Thread can set isDirty to false
    // only a sceneChangeEvent in QAspectThread can set isDirty to true
    // We need the lock to make sure there are no races when the OpenGL
    // thread tries to read isDirty or one of the texture properties in case
    // we are receiving a sceneChangeEvent that modifies isDirty or one of the properties
    // at the same time

    QMutexLocker lock(&m_lock);
    if (m_isDirty) {
        delete m_gl;
        m_gl = nullptr;
        m_isDirty = false;
    }

    // If the texture exists, we just update it and return
    if (m_gl != nullptr) {

        bool refreshDNA = m_filtersAndWrapUpdated || m_dataUploadRequired;

        if (m_filtersAndWrapUpdated) {
            updateWrapAndFilters();
            m_filtersAndWrapUpdated = false;
        }
        if (m_dataUploadRequired)
            updateAndLoadTextureImage();

        if (refreshDNA)
            updateDNA();

        return m_gl;
    }

    // Builds a new Texture, the texture was never created or it was destroyed
    // because it was dirty
    m_gl = buildGLTexture();

    m_gl->allocateStorage();
    if (!m_gl->isStorageAllocated()) {
        qWarning() << Q_FUNC_INFO << "texture storage allocation failed";
        return nullptr;
    }

    // Filters and WrapMode are set
    updateWrapAndFilters();
    m_filtersAndWrapUpdated = false;

    // Upload textures data the first time
    updateAndLoadTextureImage();

    // Update DNA
    updateDNA();

    // Ideally we might want to abstract that and use the GraphicsContext as a wrapper
    // around that.
#if defined(QT3D_RENDER_ASPECT_OPENGL_DEBUG)
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext()) {
        int err = ctx->functions()->glGetError();
        if (err)
            qWarning() << Q_FUNC_INFO << "GL error after generating mip-maps" << QString::number(err, 16);
    }
#endif

    return m_gl;
}

// RenderThread
QOpenGLTexture *Texture::buildGLTexture()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qWarning() << Q_FUNC_INFO << "requires an OpenGL context";
        return nullptr;
    }

    if (m_target == QAbstractTexture::TargetAutomatic) {
        qWarning() << Q_FUNC_INFO << "something went wrong, target shouldn't be automatic at this point";
        return nullptr;
    }

    QOpenGLTexture* glTex = new QOpenGLTexture(static_cast<QOpenGLTexture::Target>(m_target));

    // m_format may not be ES2 compatible. Now it's time to convert it, if necessary.
    QAbstractTexture::TextureFormat format = m_format;
    if (ctx->isOpenGLES() && ctx->format().majorVersion() < 3) {
        switch (m_format) {
        case QOpenGLTexture::RGBA8_UNorm:
        case QOpenGLTexture::RGBAFormat:
            format = QAbstractTexture::RGBAFormat;
            break;
        case QOpenGLTexture::RGB8_UNorm:
        case QOpenGLTexture::RGBFormat:
            format = QAbstractTexture::RGBFormat;
            break;
        case QOpenGLTexture::DepthFormat:
            format = QAbstractTexture::DepthFormat;
            break;
        default:
            qWarning() << Q_FUNC_INFO << "could not find a matching OpenGL ES 2.0 unsized texture format";
            break;
        }
    }

    // Map ETC1 to ETC2 when supported. This allows using features like
    // immutable storage as ETC2 is standard in GLES 3.0, while the ETC1 extension
    // is written against GLES 1.0.
    if (m_format == QAbstractTexture::RGB8_ETC1) {
        if ((ctx->isOpenGLES() && ctx->format().majorVersion() >= 3)
                || ctx->hasExtension(QByteArrayLiteral("GL_OES_compressed_ETC2_RGB8_texture"))
                || ctx->hasExtension(QByteArrayLiteral("GL_ARB_ES3_compatibility")))
            format = m_format = QAbstractTexture::RGB8_ETC2;
    }

    glTex->setFormat(format == QAbstractTexture::Automatic ?
                         QOpenGLTexture::NoFormat :
                         static_cast<QOpenGLTexture::TextureFormat>(format));
    glTex->setSize(m_width, m_height, m_depth);
    // Set layers count if texture array
    if (m_target == QAbstractTexture::Target1DArray ||
        m_target == QAbstractTexture::Target2DArray ||
        m_target == QAbstractTexture::Target3D ||
        m_target == QAbstractTexture::Target2DMultisampleArray ||
        m_target == QAbstractTexture::TargetCubeMapArray) {
        glTex->setLayers(m_layers);
    }

    if (m_generateMipMaps) {
        glTex->setMipLevels(glTex->maximumMipLevels());
    } else {
        glTex->setAutoMipMapGenerationEnabled(false);
        glTex->setMipBaseLevel(0);
        glTex->setMipMaxLevel(m_mipLevels - 1);
        glTex->setMipLevels(m_mipLevels);
    }

    if (!glTex->create()) {
        qWarning() << Q_FUNC_INFO << "creating QOpenGLTexture failed";
        return nullptr;
    }

    // FIXME : make this conditional on Qt version
    // work-around issue in QOpenGLTexture DSA emulaation which rasies
    // an Invalid Enum error
#if defined(QT3D_RENDER_ASPECT_OPENGL_DEBUG)
    int err = ctx->functions()->glGetError();
    if (err)
        qWarning() << Q_FUNC_INFO << err;
#endif

    return glTex;
}

// RenderThread
void Texture::setToGLTexture(QTextureImageData *imgData)
{
    Q_ASSERT(m_gl && m_gl->isCreated() && m_gl->isStorageAllocated());

    const int layers = imgData->layers();
    const int faces = imgData->faces();
    const int mipLevels = m_generateMipMaps ? 1 : imgData->mipLevels();

    for (int layer = 0; layer < layers; layer++) {
        for (int face = 0; face < faces; face++) {
            for (int level = 0; level < mipLevels; level++) {
                // ensure we don't accidently cause a detach / copy of the raw bytes
                const QByteArray &bytes(imgData->data(layer, face, level));

                if (imgData->isCompressed()) {
                    m_gl->setCompressedData(level,
                                            layer,
                                            static_cast<QOpenGLTexture::CubeMapFace>(QOpenGLTexture::CubeMapPositiveX + face),
                                            bytes.size(),
                                            bytes.constData());
                } else {
                    QOpenGLPixelTransferOptions uploadOptions;
                    uploadOptions.setAlignment(1);
                    m_gl->setData(level,
                                  layer,
                                  static_cast<QOpenGLTexture::CubeMapFace>(QOpenGLTexture::CubeMapPositiveX + face),
                                  imgData->pixelFormat(),
                                  imgData->pixelType(),
                                  bytes.constData(),
                                  &uploadOptions);
                }
            }
        }
    }

    // FIXME : make this conditional on Qt version
    // work-around issue in QOpenGLTexture DSA emulation which rasies
    // an Invalid Enum error
#if defined(QT3D_RENDER_ASPECT_OPENGL_DEBUG)
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext()) {
        int err = ctx->functions()->glGetError();
        if (err)
            qWarning() << Q_FUNC_INFO << err;
    }
#endif
}

// RenderThread
void Texture::setToGLTexture(TextureImage *rImg, QTextureImageData *imgData)
{
    Q_ASSERT(m_gl && m_gl->isCreated() && m_gl->isStorageAllocated());
    // ensure we don't accidently cause a detach / copy of the raw bytes
    const QByteArray &bytes(imgData->data());
    if (imgData->isCompressed()) {
        m_gl->setCompressedData(rImg->mipLevel(),
                                rImg->layer(),
                                static_cast<QOpenGLTexture::CubeMapFace>(rImg->face()),
                                bytes.size(),
                                bytes.constData());
    } else {
        QOpenGLPixelTransferOptions uploadOptions;
        uploadOptions.setAlignment(1);
        m_gl->setData(rImg->mipLevel(),
                      rImg->layer(),
                      static_cast<QOpenGLTexture::CubeMapFace>(rImg->face()),
                      imgData->pixelFormat(),
                      imgData->pixelType(),
                      bytes.constData(),
                      &uploadOptions);
    }

    // FIXME : make this conditional on Qt version
    // work-around issue in QOpenGLTexture DSA emulaation which rasies
    // an Invalid Enum error
#if defined(QT3D_RENDER_ASPECT_OPENGL_DEBUG)
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext()) {
        int err = ctx->functions()->glGetError();
        if (err)
            qWarning() << Q_FUNC_INFO << err;
    }
#endif
}

// RenderThread
void Texture::updateWrapAndFilters()
{
    m_gl->setWrapMode(QOpenGLTexture::DirectionS, static_cast<QOpenGLTexture::WrapMode>(m_wrapModeX));
    if (m_target != QAbstractTexture::Target1D &&
            m_target != QAbstractTexture::Target1DArray &&
            m_target != QAbstractTexture::TargetBuffer)
        m_gl->setWrapMode(QOpenGLTexture::DirectionT, static_cast<QOpenGLTexture::WrapMode>(m_wrapModeY));
    if (m_target == QAbstractTexture::Target3D)
        m_gl->setWrapMode(QOpenGLTexture::DirectionR, static_cast<QOpenGLTexture::WrapMode>(m_wrapModeZ));
    m_gl->setMinMagFilters(static_cast<QOpenGLTexture::Filter>(m_minificationFilter),
                           static_cast<QOpenGLTexture::Filter>(m_magnificationFilter));
    if (m_gl->hasFeature(QOpenGLTexture::AnisotropicFiltering))
        m_gl->setMaximumAnisotropy(m_maximumAnisotropy);
    if (m_gl->hasFeature(QOpenGLTexture::TextureComparisonOperators)) {
        m_gl->setComparisonFunction(static_cast<QOpenGLTexture::ComparisonFunction>(m_comparisonFunction));
        m_gl->setComparisonMode(static_cast<QOpenGLTexture::ComparisonMode>(m_comparisonMode));
    }
}

void Texture::updateDNA()
{
    const int key = m_width + m_height + m_depth + m_layers + m_mipLevels +
            (m_generateMipMaps ? 1 : 0) +
            static_cast<int>(m_target) +
            static_cast<int>(m_format) +
            static_cast<int>(m_magnificationFilter) +
            static_cast<int>(m_minificationFilter) +
            static_cast<int>(m_wrapModeX) +
            static_cast<int>(m_wrapModeY) +
            static_cast<int>(m_wrapModeZ) +
            static_cast<int>(m_comparisonFunction) +
            static_cast<int>(m_comparisonMode);
    m_textureDNA = ::qHash(key) + ::qHash(m_maximumAnisotropy);

    // apply non-unique hashes from texture images or texture data
    for (HTextureImage imgHandle : qAsConst(m_textureImages)) {
        TextureImage *img = m_textureImageManager->data(imgHandle);
        if (img)
            m_textureDNA += img->dna();
    }
    for (const HTextureData textureDataHandle : qAsConst(m_textureDataHandles))
        m_textureDNA += ::qHash(textureDataHandle.index());

    // if texture contains no potentially shared image data: texture is unique
    if (m_textureImages.empty() && m_textureDataHandles.isEmpty()) // Ensures uniqueness by adding unique QNode id to the dna
        m_textureDNA += qHash(peerId());
}

// RenderThread
GLint Texture::textureId()
{
    return getOrCreateGLTexture()->textureId();
}

// Any Thread
bool Texture::isTextureReset() const
{
    QMutexLocker lock(&m_lock);
    return m_isDirty;
}

void Texture::setTarget(QAbstractTexture::Target target)
{
    if (target != m_target) {
        m_target = target;
        m_isDirty = true;
    }
}

void Texture::setSize(int width, int height, int depth)
{
    if (width != m_width) {
        m_width = width;
        m_isDirty |= true;
    }
    if (height != m_height) {
        m_height = height;
        m_isDirty |= true;
    }
    if (depth != m_depth) {
        m_depth = depth;
        m_isDirty |= true;
    }
}

void Texture::setFormat(QAbstractTexture::TextureFormat format)
{
    if (format != m_format) {
        m_format = format;
        m_isDirty |= true;
    }
}

void Texture::setMipLevels(int mipLevels)
{
    if (mipLevels != m_mipLevels) {
        m_mipLevels = mipLevels;
        m_isDirty = true;
    }
}

void Texture::setLayers(int layers)
{
    if (layers != m_layers) {
        m_layers = layers;
        m_isDirty = true;
    }
}

// ChangeArbiter/Aspect Thread
void Texture::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    // The QOpenGLTexture has to be manipulated from the RenderThread only
    QMutexLocker lock(&m_lock);
    // We lock here so that we're sure the texture cannot be rebuilt while we are
    // modifying one of its properties
    switch (e->type()) {
    case PropertyUpdated: {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("width")) {
            setSize(propertyChange->value().toInt(), m_height, m_depth);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("height")) {
            setSize(m_width, propertyChange->value().toInt(), m_depth);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("depth")) {
            setSize(m_width, m_height, propertyChange->value().toInt());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("mipmaps")) {
            const bool oldMipMaps = m_generateMipMaps;
            m_generateMipMaps = propertyChange->value().toBool();
            m_isDirty |= (oldMipMaps != m_generateMipMaps);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("minificationFilter")) {
            QAbstractTexture::Filter oldMinFilter = m_minificationFilter;
            m_minificationFilter = static_cast<QAbstractTexture::Filter>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldMinFilter != m_minificationFilter);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("magnificationFilter")) {
            QAbstractTexture::Filter oldMagFilter = m_magnificationFilter;
            m_magnificationFilter = static_cast<QAbstractTexture::Filter>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldMagFilter != m_magnificationFilter);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeX")) {
            QTextureWrapMode::WrapMode oldWrapModeX = m_wrapModeX;
            m_wrapModeX = static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldWrapModeX != m_wrapModeX);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeY")) {
            QTextureWrapMode::WrapMode oldWrapModeY = m_wrapModeY;
            m_wrapModeY = static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldWrapModeY != m_wrapModeY);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeZ")) {
            QTextureWrapMode::WrapMode oldWrapModeZ = m_wrapModeZ;
            m_wrapModeZ =static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldWrapModeZ != m_wrapModeZ);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("format")) {
            setFormat(static_cast<QAbstractTexture::TextureFormat>(propertyChange->value().toInt()));
        } else if (propertyChange->propertyName() == QByteArrayLiteral("target")) {
            QAbstractTexture::Target oldTarget = m_target;
            m_target = static_cast<QAbstractTexture::Target>(propertyChange->value().toInt());
            m_isDirty |= (oldTarget != m_target);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("maximumAnisotropy")) {
            float oldMaximumAnisotropy = m_maximumAnisotropy;
            m_maximumAnisotropy = propertyChange->value().toFloat();
            m_filtersAndWrapUpdated |= !qFuzzyCompare(oldMaximumAnisotropy, m_maximumAnisotropy);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("comparisonFunction")) {
            QAbstractTexture::ComparisonFunction oldComparisonFunction = m_comparisonFunction;
            m_comparisonFunction = propertyChange->value().value<QAbstractTexture::ComparisonFunction>();
            m_filtersAndWrapUpdated |= (oldComparisonFunction != m_comparisonFunction);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("comparisonMode")) {
            QAbstractTexture::ComparisonMode oldComparisonMode = m_comparisonMode;
            m_comparisonMode = propertyChange->value().value<QAbstractTexture::ComparisonMode>();
            m_filtersAndWrapUpdated |= (oldComparisonMode != m_comparisonMode);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("maximumLayers")) {
            const int oldLayers = m_layers;
            m_layers = propertyChange->value().toInt();
            m_isDirty |= (oldLayers != m_layers);
        }
        // TO DO: Handle the textureGenerator change
    }
        break;

    case PropertyValueAdded: {
        const auto change = qSharedPointerCast<QPropertyNodeAddedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("textureImage")) {
            m_textureImages.append(m_textureImageManager->lookupHandle(change->addedNodeId()));
        }
    }
        break;

    case PropertyValueRemoved: {
        const auto change = qSharedPointerCast<QPropertyNodeRemovedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("textureImage")) {
            m_textureImages.removeOne(m_textureImageManager->lookupHandle(change->removedNodeId()));
            // If a TextureImage is removed from a Texture, the texture image data remains on GPU
        }
    }
        break;

    default:
        break;

    }
    markDirty(AbstractRenderer::AllDirty);
    BackendNode::sceneChangeEvent(e);
}

TextureDNA Texture::dna() const
{
    return m_textureDNA;
}

// AspectThread
void Texture::setTextureManager(TextureManager *manager)
{
    m_textureManager = manager;
}

// AspectThread
void Texture::setTextureImageManager(TextureImageManager *manager)
{
    m_textureImageManager = manager;
}

void Texture::setTextureDataManager(TextureDataManager *manager)
{
    m_textureDataManager = manager;
}

// RenderThread
void Texture::updateAndLoadTextureImage()
{
    // Upload all QTexImageData set by the QTextureGenerator
    for (const HTextureData textureDataHandle : qAsConst(m_textureDataHandles)) {
        QTextureImageData *data = m_textureDataManager->data(textureDataHandle);
        if (data != Q_NULLPTR)
            setToGLTexture(data);
    }

    // Upload all QTexImageData references by the TextureImages
    QVector<TextureImageDNA> dnas;
    for (const HTextureImage t : qAsConst(m_textureImages)) {
        TextureImage *img = m_textureImageManager->data(t);
        if (img != nullptr && img->isDirty()) {
            if (dnas.contains(img->dna())) {
                img->unsetDirty();
                continue;
            }
            QTextureImageData *data = m_textureDataManager->data(img->textureDataHandle());
            if (data != nullptr) {
                setToGLTexture(img, data);
                dnas.append(img->dna());
                img->unsetDirty();
            }
        }
    }

    m_dataUploadRequired = false;
}

void Texture::addTextureImageData(HTextureImage handle)
{
    m_textureImages.append(handle);
}

void Texture::removeTextureImageData(HTextureImage handle)
{
    m_textureImages.removeOne(handle);
}

void Texture::requestTextureDataUpdate()
{
    m_dataUploadRequired = true;
}

// Will request a new jobs, if one of the texture data has changed
// after the job was executed, requestTextureDataUpdate will be called
// Called by RenderTextureImages
void Texture::addToPendingTextureJobs()
{
    m_textureDataManager->addToPendingTextures(peerId());
}

TextureFunctor::TextureFunctor(AbstractRenderer *renderer,
                               TextureManager *textureManager,
                               TextureImageManager *textureImageManager,
                               TextureDataManager *textureDataManager)
    : m_renderer(renderer)
    , m_textureManager(textureManager)
    , m_textureImageManager(textureImageManager)
    , m_textureDataManager(textureDataManager)
{
}

Qt3DCore::QBackendNode *TextureFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    Texture *backend = m_textureManager->getOrCreateResource(change->subjectId());
    backend->setTextureManager(m_textureManager);
    backend->setTextureImageManager(m_textureImageManager);
    backend->setTextureDataManager(m_textureDataManager);
    backend->setRenderer(m_renderer);
    return backend;
}

Qt3DCore::QBackendNode *TextureFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_textureManager->lookupResource(id);
}

void TextureFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_textureManager->releaseResource(id);
}

void Texture::addTextureDataHandle(HTextureData handle)
{
    m_textureDataHandles.push_back(handle);
    // Request a new upload to the GPU
    requestTextureDataUpdate();
}

void Texture::releaseTextureDataHandles()
{
    if (m_textureDataHandles.size() > 0) {
        m_isDirty = true;
        Q_ASSERT(m_textureDataManager);
        for (HTextureData textureData : qAsConst(m_textureDataHandles))
            m_textureDataManager->release(textureData);
        m_textureDataHandles.clear();
        // Request a new upload to the GPU
        requestTextureDataUpdate();
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
