/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qhash.h>
#include "rendertexture_p.h"

#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/texturedata.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/private/texturedatamanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

RenderTexture::RenderTexture()
    : QBackendNode()
    , m_gl(Q_NULLPTR)
    , m_width(1)
    , m_height(1)
    , m_depth(1)
    , m_layers(1)
    , m_generateMipMaps(false)
    , m_target(QAbstractTextureProvider::Target2D)
    , m_format(QAbstractTextureProvider::RGBA8_UNorm)
    , m_magnificationFilter(QAbstractTextureProvider::Nearest)
    , m_minificationFilter(QAbstractTextureProvider::Nearest)
    , m_wrapModeX(QTextureWrapMode::ClampToEdge)
    , m_wrapModeY(QTextureWrapMode::ClampToEdge)
    , m_wrapModeZ(QTextureWrapMode::ClampToEdge)
    , m_maximumAnisotropy(1.0f)
    , m_comparisonFunction(QAbstractTextureProvider::CompareLessEqual)
    , m_comparisonMode(QAbstractTextureProvider::CompareNone)
    , m_isDirty(false)
    , m_filtersAndWrapUpdated(false)
    , m_dataUploadRequired(false)
    , m_unique(false)
    , m_lock(new QMutex())
    , m_textureDNA(0)
    , m_textureManager(Q_NULLPTR)
    , m_textureImageManager(Q_NULLPTR)
    , m_textureDataManager(Q_NULLPTR)
{
    // We need backend -> frontend notifications to update the status of the texture
}

RenderTexture::~RenderTexture()
{
    if (m_lock != Q_NULLPTR)
        delete m_lock;
    m_lock = Q_NULLPTR;
}

void RenderTexture::cleanup()
{
    m_gl = Q_NULLPTR;
    m_width = 1;
    m_height = 1;
    m_depth = 1;
    m_layers = 1;
    m_generateMipMaps = false;
    m_target = QAbstractTextureProvider::Target2D;
    m_format = QAbstractTextureProvider::RGBA8_UNorm;
    m_magnificationFilter = QAbstractTextureProvider::Nearest;
    m_minificationFilter = QAbstractTextureProvider::Nearest;
    m_wrapModeX = QTextureWrapMode::ClampToEdge;
    m_wrapModeY = QTextureWrapMode::ClampToEdge;
    m_wrapModeZ = QTextureWrapMode::ClampToEdge;
    m_maximumAnisotropy = 1.0f;
    m_comparisonFunction = QAbstractTextureProvider::CompareLessEqual;
    m_comparisonMode = QAbstractTextureProvider::CompareNone;
    m_isDirty = false;
    m_filtersAndWrapUpdated = false;
    m_dataUploadRequired = false;
    m_unique = false;
    m_textureDNA = 0;
    m_textureImages.clear();
    m_textureManager = Q_NULLPTR;
    m_textureImageManager = Q_NULLPTR;
    m_textureDataManager = Q_NULLPTR;
}

// AspectThread
void RenderTexture::updateFromPeer(QNode *peer)
{
    QAbstractTextureProvider *texture = static_cast<QAbstractTextureProvider *>(peer);

    QMutexLocker lock(m_lock);
    if (texture != Q_NULLPTR) {
        m_isDirty = true;
        m_width = texture->width();
        m_height = texture->height();
        m_depth = texture->depth();
        m_generateMipMaps = texture->generateMipMaps();
        m_target = texture->target();
        m_format = texture->format();
        m_magnificationFilter = texture->magnificationFilter();
        m_minificationFilter = texture->minificationFilter();
        m_wrapModeX = texture->wrapMode()->x();
        m_wrapModeY = texture->wrapMode()->y();
        m_wrapModeZ = texture->wrapMode()->z();
        m_maximumAnisotropy = texture->maximumAnisotropy();
        m_comparisonFunction = texture->comparisonFunction();
        m_comparisonMode = texture->comparisonMode();
        m_layers = texture->maximumLayers();
        m_unique = texture->isUnique();
    }
}

// RenderTread
QOpenGLTexture *RenderTexture::getOrCreateGLTexture()
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

    QMutexLocker lock(m_lock);
    if (m_isDirty) {
        delete m_gl;
        m_gl = Q_NULLPTR;
        m_isDirty = false;
    }

    // If the texture exists, we just update it and return
    if (m_gl != Q_NULLPTR) {

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
        return Q_NULLPTR;
    }

    // Filters and WrapMode are set
    updateWrapAndFilters();
    m_filtersAndWrapUpdated = false;

    // Upload textures data the first time
    updateAndLoadTextureImage();

    // Update DNA
    updateDNA();

    // Ideally we might want to abstract that and use the QGraphicsContext as a wrapper
    // around that.
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext()) {
        int err = ctx->functions()->glGetError();
        if (err)
            qWarning() << Q_FUNC_INFO << "GL error after generating mip-maps" << QString::number(err, 16);
    }

    return m_gl;
}

// RenderThread
QOpenGLTexture *RenderTexture::buildGLTexture()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        qWarning() << Q_FUNC_INFO << "requires an OpenGL context";
        return Q_NULLPTR;
    }

    QOpenGLTexture* glTex = new QOpenGLTexture(static_cast<QOpenGLTexture::Target>(m_target));

    if (m_format == QAbstractTextureProvider::Automatic)
        qWarning() << Q_FUNC_INFO << "something went wrong, format shouldn't be automatic at this point";

    // m_format may not be ES2 compatible. Now it's time to convert it, if necessary.
    QAbstractTextureProvider::TextureFormat format = m_format;
    if (ctx->isOpenGLES() && ctx->format().majorVersion() < 3) {
        switch (m_format) {
        case QOpenGLTexture::RGBA8_UNorm:
        case QOpenGLTexture::RGBAFormat:
            format = QAbstractTextureProvider::RGBAFormat;
            break;
        case QOpenGLTexture::RGB8_UNorm:
        case QOpenGLTexture::RGBFormat:
            format = QAbstractTextureProvider::RGBFormat;
            break;
        case QOpenGLTexture::DepthFormat:
            format = QAbstractTextureProvider::DepthFormat;
            break;
        default:
            qWarning() << Q_FUNC_INFO << "could not find a matching OpenGL ES 2.0 unsized texture format";
            break;
        }
    }

    glTex->setFormat(format == QAbstractTextureProvider::Automatic ?
                         QOpenGLTexture::NoFormat :
                         static_cast<QOpenGLTexture::TextureFormat>(format));
    glTex->setSize(m_width, m_height, m_depth);
    // Set layers count if texture array
    if (m_target == QAbstractTextureProvider::Target1DArray ||
            m_target == QAbstractTextureProvider::Target2DArray ||
            m_target == QAbstractTextureProvider::Target3D ||
            m_target == QAbstractTextureProvider::Target2DMultisampleArray ||
            m_target == QAbstractTextureProvider::TargetCubeMapArray)
        glTex->setLayers(m_layers);

    if (m_generateMipMaps)
        glTex->setMipLevels(glTex->maximumMipLevels());

    if (!glTex->create()) {
        qWarning() << Q_FUNC_INFO << "creating QOpenGLTexture failed";
        return Q_NULLPTR;
    }

    // FIXME : make this conditional on Qt version
    // work-around issue in QOpenGLTexture DSA emulaation which rasies
    // an Invalid Enum error
    int err = ctx->functions()->glGetError();
    if (err)
        qWarning() << Q_FUNC_INFO << err;

    return glTex;
}

// RenderThread
void RenderTexture::setToGLTexture(RenderTextureImage *rImg, TexImageData *imgData)
{
    Q_ASSERT(m_gl && m_gl->isCreated() && m_gl->isStorageAllocated());
    // ensure we don't accidently cause a detach / copy of the raw bytes
    const QByteArray &bytes(imgData->data());
    if (imgData->isCompressed()) {
        m_gl->setCompressedData(rImg->mipmapLevel(),
                                rImg->layer(),
                                static_cast<QOpenGLTexture::CubeMapFace>(rImg->face()),
                                bytes.size(),
                                bytes.constData());
    } else {
        QOpenGLPixelTransferOptions uploadOptions;
        uploadOptions.setAlignment(1);
        m_gl->setData(rImg->mipmapLevel(),
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
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext()) {
        int err = ctx->functions()->glGetError();
        if (err)
            qWarning() << Q_FUNC_INFO << err;
    }
}

// RenderThread
void RenderTexture::updateWrapAndFilters()
{
    m_gl->setWrapMode(QOpenGLTexture::DirectionS, static_cast<QOpenGLTexture::WrapMode>(m_wrapModeX));
    if (m_target != QAbstractTextureProvider::Target1D &&
            m_target != QAbstractTextureProvider::Target1DArray &&
            m_target != QAbstractTextureProvider::TargetBuffer)
        m_gl->setWrapMode(QOpenGLTexture::DirectionT, static_cast<QOpenGLTexture::WrapMode>(m_wrapModeY));
    if (m_target == QAbstractTextureProvider::Target3D)
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

void RenderTexture::updateDNA()
{
    int key = m_width + m_height + m_depth + m_layers +
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
    Q_FOREACH (HTextureImage imgHandle, m_textureImages) {
        RenderTextureImage *img = m_textureImageManager->data(imgHandle);
        if (img)
            m_textureDNA += img->dna();
    }
    if (m_unique) // Ensures uniqueness by adding unique QNode id to the dna
        m_textureDNA += qHash(peerUuid());
}

// RenderThread
GLint RenderTexture::textureId()
{
    return getOrCreateGLTexture()->textureId();
}

// Any Thread
bool RenderTexture::isTextureReset() const
{
    QMutexLocker lock(m_lock);
    return m_isDirty;
}

void RenderTexture::setSize(int width, int height, int depth)
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

void RenderTexture::setFormat(QAbstractTextureProvider::TextureFormat format)
{
    if (format != m_format) {
        m_format = format;
        m_isDirty |= true;
    }
}

// ChangeArbiter/Aspect Thread
void RenderTexture::sceneChangeEvent(const QSceneChangePtr &e)
{
    // The QOpenGLTexture has to be manipulated from the RenderThread only
    QMutexLocker lock(m_lock);
    // We lock here so that we're sure the texture cannot be rebuilt while we are
    // modifying one of its properties
    QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
    switch (e->type()) {
    case NodeUpdated: {
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
            QAbstractTextureProvider::Filter oldMinFilter = m_minificationFilter;
            m_minificationFilter = static_cast<QAbstractTextureProvider::Filter>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldMinFilter != m_minificationFilter);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("magnificationFilter")) {
            QAbstractTextureProvider::Filter oldMagFilter = m_magnificationFilter;
            m_magnificationFilter = static_cast<QAbstractTextureProvider::Filter>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldMagFilter != m_magnificationFilter);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeX")) {
            QTextureWrapMode::WrapMode oldWrapModeX = m_wrapModeX;
            m_wrapModeX = static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldWrapModeX != m_wrapModeX);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeX")) {
            QTextureWrapMode::WrapMode oldWrapModeY = m_wrapModeY;
            m_wrapModeY = static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldWrapModeY != m_wrapModeY);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeX")) {
            QTextureWrapMode::WrapMode oldWrapModeZ = m_wrapModeZ;
            m_wrapModeZ =static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            m_filtersAndWrapUpdated |= (oldWrapModeZ != m_wrapModeZ);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("format")) {
            setFormat(static_cast<QAbstractTextureProvider::TextureFormat>(propertyChange->value().toInt()));
        } else if (propertyChange->propertyName() == QByteArrayLiteral("target")) {
            QAbstractTextureProvider::Target oldTarget = m_target;
            m_target = static_cast<QAbstractTextureProvider::Target>(propertyChange->value().toInt());
            m_isDirty |= (oldTarget != m_target);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("maximumAnisotropy")) {
            float oldMaximumAnisotropy = m_maximumAnisotropy;
            m_maximumAnisotropy = propertyChange->value().toFloat();
            m_filtersAndWrapUpdated |= !qFuzzyCompare(oldMaximumAnisotropy, m_maximumAnisotropy);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("comparisonFunction")) {
            QAbstractTextureProvider::ComparisonFunction oldComparisonFunction = m_comparisonFunction;
            m_comparisonFunction = propertyChange->value().value<QAbstractTextureProvider::ComparisonFunction>();
            m_filtersAndWrapUpdated |= (oldComparisonFunction != m_comparisonFunction);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("comparisonMode")) {
            QAbstractTextureProvider::ComparisonMode oldComparisonMode = m_comparisonMode;
            m_comparisonMode = propertyChange->value().value<QAbstractTextureProvider::ComparisonMode>();
            m_filtersAndWrapUpdated |= (oldComparisonMode != m_comparisonMode);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("maximumLayers")) {
            const int oldLayers = m_layers;
            m_layers = propertyChange->value().toInt();
            m_isDirty |= (oldLayers != m_layers);
        } else if (propertyChange->propertyName() == QByteArrayLiteral("unique")) {
            const bool oldUnique = m_unique;
            m_unique = propertyChange->value().toBool();
            // Will force a DNA update
            m_filtersAndWrapUpdated |= (oldUnique != m_unique);
        }
    }
        break;

    case NodeAdded: {
        if (propertyChange->propertyName() == QByteArrayLiteral("textureImage")) {
            m_textureImages.append(m_textureImageManager->lookupHandle(propertyChange->value().value<QNodeId>()));
        }
    }
        break;

    case NodeRemoved: {
        if (propertyChange->propertyName() == QByteArrayLiteral("textureImage")) {
            m_textureImages.removeOne(m_textureImageManager->lookupHandle(propertyChange->value().value<QNodeId>()));
            // If a TextureImage is removed from a Texture, the texture image data remains on GPU
        }
    }
        break;

    default:
        break;

    }
}

TextureDNA RenderTexture::dna() const
{
    return m_textureDNA;
}

// AspectThread
void RenderTexture::setTextureManager(TextureManager *manager)
{
    m_textureManager = manager;
}

// AspectThread
void RenderTexture::setTextureImageManager(TextureImageManager *manager)
{
    m_textureImageManager = manager;
}

void RenderTexture::setTextureDataManager(TextureDataManager *manager)
{
    m_textureDataManager = manager;
}

// RenderThread
void RenderTexture::updateAndLoadTextureImage()
{
    Q_FOREACH (HTextureImage t, m_textureImages) {
        RenderTextureImage *img = m_textureImageManager->data(t);
        if (img != Q_NULLPTR && img->isDirty()) {
            TexImageData *data = m_textureDataManager->data(img->textureDataHandle());
            if (data != Q_NULLPTR) {
                setToGLTexture(img, data);
                img->unsetDirty();
            }
        }
    }
    m_dataUploadRequired = false;
}

void RenderTexture::addTextureImageData(HTextureImage handle)
{
    m_textureImages.append(handle);
}

void RenderTexture::removeTextureImageData(HTextureImage handle)
{
    m_textureImages.removeOne(handle);
}

void RenderTexture::requestTextureDataUpdate()
{
    m_dataUploadRequired = true;
}

// Will request a new jobs, if one of the texture data has changed
// after the job was executed, requestTextureDataUpdate will be called
// Called by RenderTextureImages
void RenderTexture::addToPendingTextureJobs()
{
    m_textureDataManager->addToPendingTextures(peerUuid());
}

RenderTextureFunctor::RenderTextureFunctor(TextureManager *textureManager,
                                           TextureImageManager *textureImageManager,
                                           TextureDataManager *textureDataManager)
    : m_textureManager(textureManager)
    , m_textureImageManager(textureImageManager)
    , m_textureDataManager(textureDataManager)
{
}

QBackendNode *RenderTextureFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    RenderTexture *backend = m_textureManager->getOrCreateResource(frontend->id());
    backend->setFactory(factory);
    backend->setTextureManager(m_textureManager);
    backend->setTextureImageManager(m_textureImageManager);
    backend->setTextureDataManager(m_textureDataManager);
    backend->setPeer(frontend);
    return backend;
}

QBackendNode *RenderTextureFunctor::get(const QNodeId &id) const
{
    return m_textureManager->lookupResource(id);
}

void RenderTextureFunctor::destroy(const QNodeId &id) const
{
    m_textureManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
