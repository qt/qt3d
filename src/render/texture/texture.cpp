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

#include "texture_p.h"

#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>

#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/qabstracttexture_p.h>
#include <Qt3DRender/private/gltexturemanager_p.h>
#include <Qt3DRender/private/managers_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {
namespace Render {

Texture::Texture()
    // We need backend -> frontend notifications to update the status of the texture
    : BackendNode(ReadWrite)
    , m_dirty(Generators|Properties|Parameters)
    , m_texture(nullptr)
    , m_textureManager(nullptr)
    , m_textureImageManager(nullptr)
{
    // set default values
    m_properties.width = 1;
    m_properties.height = 1;
    m_properties.depth = 1;
    m_properties.layers = 1;
    m_properties.mipLevels = 1;
    m_properties.samples = 1;
    m_properties.generateMipMaps = false;
    m_properties.format = QAbstractTexture::RGBA8_UNorm;
    m_properties.target = QAbstractTexture::Target2D;

    m_parameters.magnificationFilter = QAbstractTexture::Nearest;
    m_parameters.minificationFilter = QAbstractTexture::Nearest;
    m_parameters.wrapModeX = QTextureWrapMode::ClampToEdge;
    m_parameters.wrapModeY = QTextureWrapMode::ClampToEdge;
    m_parameters.wrapModeZ = QTextureWrapMode::ClampToEdge;
    m_parameters.maximumAnisotropy = 1.0f;
    m_parameters.comparisonFunction = QAbstractTexture::CompareLessEqual;
    m_parameters.comparisonMode = QAbstractTexture::CompareNone;
}

Texture::~Texture()
{
    // We do not abandon the api texture
    // because if the dtor is called that means
    // the manager was destroyed otherwise cleanup
    // would have been called
}

void Texture::setTextureManager(GLTextureManager *manager)
{
    m_textureManager = manager;
}

void Texture::setTextureImageManager(TextureImageManager *manager)
{
    m_textureImageManager = manager;
}

/**
 * Asks the TextureManager for a Texture that fits this TextureNode's Properties.
 * If this node controls a non-shared texture already, this texture will just be
 * modified.
 */
void Texture::updateTexture()
{
    // TODO: for implementing unique, non-shared, non-cached textures.
    // for now, every texture is shared by default
    const bool isUnique = false;

    // first call? just allocate and leave
    if (!m_texture) {
        m_texture = isUnique ? m_textureManager->createUnique(this) : m_textureManager->getOrCreateShared(this);
        m_dirty = 0;
        return;
    }

    // if this texture is a shared texture, we might need to look for a new TextureImpl
    // and abandon the old one
    if (m_textureManager->isShared(m_texture)) {
        m_textureManager->abandon(m_texture, this);
        m_texture = m_textureManager->getOrCreateShared(this);
        m_dirty = 0;
        return;
    }

    // this texture node is the only one referring to the TextureImpl.
    // we could thus directly modify the texture. Instead, for non-unique textures,
    // we first see if there is already a matching texture present.
    if (!isUnique) {
        GLTexture *newSharedTex = m_textureManager->tryFindShared(this);

        if (newSharedTex && newSharedTex != m_texture) {
            m_textureManager->abandon(m_texture, this);
            m_texture = newSharedTex;
            m_dirty = 0;
            return;
        }
    }


    // we hold a reference to a unique or exclusive access to a shared texture
    // we can thus modify the texture directly.

    if (m_dirty.testFlag(Properties)) {
        if (m_textureManager->setProperties(m_texture, m_properties))
            m_dirty.setFlag(Properties, false);
        else
            qWarning() << "[Qt3DRender::TextureNode] updateTexture: TextureImpl.setProperties failed, should be non-shared";
    }

    if (m_dirty.testFlag(Parameters)) {
        if (m_textureManager->setParameters(m_texture, m_parameters))
            m_dirty.setFlag(Parameters, false);
        else
            qWarning() << "[Qt3DRender::TextureNode] updateTexture: TextureImpl.setParameters failed, should be non-shared";
    }

    if (m_dirty.testFlag(Generators)) {
        if (m_textureManager->setImages(m_texture, m_textureImages))
            m_dirty.setFlag(Generators, false);
        else
            qWarning() << "[Qt3DRender::TextureNode] updateTexture: TextureImpl.setGenerators failed, should be non-shared";
    }

}

void Texture::addDirtyFlag(DirtyFlags flags)
{
    m_dirty |= flags;
    updateTexture();
}

void Texture::addTextureImage(Qt3DCore::QNodeId id)
{
    if (!m_textureImageManager) {
        qWarning() << "[Qt3DRender::TextureNode] addTextureImage: invalid TextureImageManager";
        return;
    }

    const HTextureImage handle = m_textureImageManager->lookupHandle(id);
    if (handle.isNull()) {
        qWarning() << "[Qt3DRender::TextureNode] addTextureImage: image handle is NULL";
    } else if (!m_textureImages.contains(handle)) {
        m_textureImages << handle;
        addDirtyFlag(Generators);
    }
}

void Texture::removeTextureImage(Qt3DCore::QNodeId id)
{
    if (!m_textureImageManager) {
        qWarning() << "[Qt3DRender::TextureNode] removeTextureImage: invalid TextureImageManager";
        return;
    }

    const HTextureImage handle = m_textureImageManager->lookupHandle(id);
    if (handle.isNull()) {
        qWarning() << "[Qt3DRender::TextureNode] removeTextureImage: image handle is NULL";
    } else {
        m_textureImages.removeAll(handle);
        addDirtyFlag(Generators);
    }
}

void Texture::cleanup()
{
    if (m_texture)
        m_textureManager->abandon(m_texture, this);
    m_texture = nullptr;
    m_dataFunctor.reset();
    m_textureImages.clear();

    // set default values
    m_properties.width = 1;
    m_properties.height = 1;
    m_properties.depth = 1;
    m_properties.layers = 1;
    m_properties.mipLevels = 1;
    m_properties.samples = 1;
    m_properties.generateMipMaps = false;
    m_properties.format = QAbstractTexture::RGBA8_UNorm;
    m_properties.target = QAbstractTexture::Target2D;

    m_parameters.magnificationFilter = QAbstractTexture::Nearest;
    m_parameters.minificationFilter = QAbstractTexture::Nearest;
    m_parameters.wrapModeX = QTextureWrapMode::ClampToEdge;
    m_parameters.wrapModeY = QTextureWrapMode::ClampToEdge;
    m_parameters.wrapModeZ = QTextureWrapMode::ClampToEdge;
    m_parameters.maximumAnisotropy = 1.0f;
    m_parameters.comparisonFunction = QAbstractTexture::CompareLessEqual;
    m_parameters.comparisonMode = QAbstractTexture::CompareNone;
}

// ChangeArbiter/Aspect Thread
void Texture::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    DirtyFlags dirty;

    switch (e->type()) {
    case PropertyUpdated: {
        QPropertyUpdatedChangePtr propertyChange = qSharedPointerCast<QPropertyUpdatedChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("width")) {
            m_properties.width = propertyChange->value().toInt();
            dirty = Properties;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("height")) {
            m_properties.height = propertyChange->value().toInt();
            dirty = Properties;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("depth")) {
            m_properties.depth = propertyChange->value().toInt();
            dirty = Properties;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("maximumLayers")) {
            m_properties.layers = propertyChange->value().toInt();
            dirty = Properties;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("format")) {
            m_properties.format = static_cast<QAbstractTexture::TextureFormat>(propertyChange->value().toInt());
            dirty = Properties;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("target")) {
            m_properties.target = static_cast<QAbstractTexture::Target>(propertyChange->value().toInt());
            dirty = Properties;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("mipmaps")) {
            m_properties.generateMipMaps = propertyChange->value().toBool();
            dirty = Properties;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("minificationFilter")) {
            m_parameters.minificationFilter = static_cast<QAbstractTexture::Filter>(propertyChange->value().toInt());
            dirty = Parameters;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("magnificationFilter")) {
            m_parameters.magnificationFilter = static_cast<QAbstractTexture::Filter>(propertyChange->value().toInt());
            dirty = Parameters;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeX")) {
            m_parameters.wrapModeX = static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            dirty = Parameters;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeY")) {
            m_parameters.wrapModeY = static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            dirty = Parameters;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("wrapModeZ")) {
            m_parameters.wrapModeZ =static_cast<QTextureWrapMode::WrapMode>(propertyChange->value().toInt());
            dirty = Parameters;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("maximumAnisotropy")) {
            m_parameters.maximumAnisotropy = propertyChange->value().toFloat();
            dirty = Parameters;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("comparisonFunction")) {
            m_parameters.comparisonFunction = propertyChange->value().value<QAbstractTexture::ComparisonFunction>();
            dirty = Parameters;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("comparisonMode")) {
            m_parameters.comparisonMode = propertyChange->value().value<QAbstractTexture::ComparisonMode>();
            dirty = Parameters;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("layers")) {
            m_properties.layers = propertyChange->value().toInt();
            dirty = Properties;
        } else if (propertyChange->propertyName() == QByteArrayLiteral("samples")) {
            m_properties.samples = propertyChange->value().toInt();
            dirty = Properties;
        }

        // TO DO: Handle the textureGenerator change
    }
        break;

    case PropertyValueAdded: {
        const auto change = qSharedPointerCast<QPropertyNodeAddedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("textureImage")) {
            addTextureImage(change->addedNodeId());
        }
    }
        break;

    case PropertyValueRemoved: {
        const auto change = qSharedPointerCast<QPropertyNodeRemovedChange>(e);
        if (change->propertyName() == QByteArrayLiteral("textureImage")) {
            removeTextureImage(change->removedNodeId());
        }
    }
        break;

    default:
        break;

    }

    addDirtyFlag(dirty);

    markDirty(AbstractRenderer::AllDirty);
    BackendNode::sceneChangeEvent(e);
}

void Texture::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<QAbstractTextureData>>(change);
    const auto &data = typedChange->data;

    m_properties.target = data.target;
    m_properties.format = data.format;
    m_properties.width = data.width;
    m_properties.height = data.height;
    m_properties.depth = data.depth;
    m_properties.generateMipMaps = data.autoMipMap;
    m_properties.layers = data.layers;
    m_properties.samples = data.samples;
    m_parameters.minificationFilter = data.minFilter;
    m_parameters.magnificationFilter = data.magFilter;
    m_parameters.wrapModeX = data.wrapModeX;
    m_parameters.wrapModeY = data.wrapModeY;
    m_parameters.wrapModeZ = data.wrapModeZ;
    m_parameters.maximumAnisotropy = data.maximumAnisotropy;
    m_parameters.comparisonFunction = data.comparisonFunction;
    m_parameters.comparisonMode = data.comparisonMode;
    m_dataFunctor = data.dataFunctor;

    addDirtyFlag(DirtyFlags(Generators|Properties|Parameters));
}


TextureFunctor::TextureFunctor(AbstractRenderer *renderer,
                               TextureManager *textureNodeManager,
                               GLTextureManager *textureManager,
                               TextureImageManager *textureImageManager)
    : m_renderer(renderer)
    , m_textureNodeManager(textureNodeManager)
    , m_textureManager(textureManager)
    , m_textureImageManager(textureImageManager)
{
}

Qt3DCore::QBackendNode *TextureFunctor::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    Texture *backend = m_textureNodeManager->getOrCreateResource(change->subjectId());
    backend->setTextureManager(m_textureManager);
    backend->setTextureImageManager(m_textureImageManager);
    backend->setRenderer(m_renderer);
    return backend;
}

Qt3DCore::QBackendNode *TextureFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_textureNodeManager->lookupResource(id);
}

void TextureFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_textureNodeManager->releaseResource(id);
}


} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
