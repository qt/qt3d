// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "texture_p.h"

#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

#include <Qt3DRender/private/texture_p.h>
#include <Qt3DRender/private/qabstracttexture_p.h>
#include <Qt3DRender/private/managers_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

Texture::Texture()
    // We need backend -> frontend notifications to update the status of the texture
    : BackendNode(ReadWrite)
    , m_dirty(DirtyImageGenerators|DirtyProperties|DirtyParameters|DirtyDataGenerator)
    , m_sharedTextureId(-1)
{
}

Texture::~Texture()
{
    // We do not abandon the api texture
    // because if the dtor is called that means
    // the manager was destroyed otherwise cleanup
    // would have been called
}

void Texture::addDirtyFlag(DirtyFlags flags)
{
    QMutexLocker lock(&m_flagsMutex);
    m_dirty |= flags;
    if (m_renderer)
        markDirty(AbstractRenderer::TexturesDirty);
}

Texture::DirtyFlags Texture::dirtyFlags()
{
    QMutexLocker lock(&m_flagsMutex);
    return m_dirty;
}

void Texture::unsetDirty()
{
    QMutexLocker lock(&m_flagsMutex);
    m_dirty = Texture::NotDirty;
}

// This is called by Renderer::updateGLResources
// when the texture has been marked for cleanup
void Texture::cleanup()
{
    // Whoever calls this must make sure to also check if this
    // texture is being referenced by a shared API specific texture (GLTexture)
    m_dataFunctor.reset();
    m_textureImageIds.clear();
    m_pendingTextureDataUpdates.clear();
    m_sharedTextureId = -1;

    // set default values
    m_properties = {};
    m_parameters = {};

    m_dirty = NotDirty;
}

void Texture::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const QAbstractTexture *node = qobject_cast<const QAbstractTexture *>(frontEnd);
    if (!node)
        return;

    TextureProperties p = m_properties;
    p.width = node->width();
    p.height = node->height();
    p.depth = node->depth();
    p.format = node->format();
    p.target = node->target();
    p.generateMipMaps = node->generateMipMaps();
    p.layers = node->layers();
    p.samples = node->samples();
    p.mipLevels = node->mipLevels();
    if (p != m_properties) {
        m_properties = p;
        addDirtyFlag(DirtyProperties);
    }

    TextureParameters q = m_parameters;
    q.magnificationFilter = node->magnificationFilter();
    q.minificationFilter = node->minificationFilter();
    q.wrapModeX = const_cast<QAbstractTexture *>(node)->wrapMode()->x();
    q.wrapModeY = const_cast<QAbstractTexture *>(node)->wrapMode()->y();
    q.wrapModeZ = const_cast<QAbstractTexture *>(node)->wrapMode()->z();
    q.maximumAnisotropy = node->maximumAnisotropy();
    q.comparisonFunction = node->comparisonFunction();
    q.comparisonMode = node->comparisonMode();
    if (q != m_parameters) {
        m_parameters = q;
        addDirtyFlag(DirtyParameters);
    }

    QAbstractTexturePrivate *dnode = static_cast<QAbstractTexturePrivate *>(QAbstractTexturePrivate::get(const_cast<QAbstractTexture *>(node)));
    auto newGenerator = dnode->dataFunctor();
    if (newGenerator != m_dataFunctor) {
        setDataGenerator(newGenerator);
        QAbstractTexturePrivate *dTexture = static_cast<QAbstractTexturePrivate *>(QNodePrivate::get(const_cast<QNode *>(frontEnd)));
        dTexture->setStatus(QAbstractTexture::Loading);
    }

    if (dnode) {
        for (const QTextureDataUpdate &pendingUpdate : dnode->m_pendingDataUpdates)
            addTextureDataUpdate(pendingUpdate);
        dnode->m_pendingDataUpdates.clear();

        auto ids = Qt3DCore::qIdsForNodes(dnode->m_textureImages);
        std::sort(std::begin(ids), std::end(ids));
        if (ids != m_textureImageIds) {
            m_textureImageIds = ids;
            addDirtyFlag(DirtyImageGenerators);
        }

        if (dnode->m_sharedTextureId != m_sharedTextureId) {
            m_sharedTextureId = dnode->m_sharedTextureId;
            addDirtyFlag(DirtySharedTextureId);
        }
    }
}

// Called by syncFromFrontend or TextureDownloadRequest (both in AspectThread context)
void Texture::setDataGenerator(const QTextureGeneratorPtr &generator)
{
    m_dataFunctor = generator;
    addDirtyFlag(DirtyDataGenerator);
}

bool Texture::isValid(TextureImageManager *manager) const
{
    for (const QNodeId &id : m_textureImageIds) {
        TextureImage *img = manager->lookupResource(id);
        if (img == nullptr)
            return false;
    }
    return true;
}

void Texture::addTextureDataUpdate(const QTextureDataUpdate &update)
{
    m_pendingTextureDataUpdates.push_back(update);
    addDirtyFlag(DirtyPendingDataUpdates);
}


TextureFunctor::TextureFunctor(AbstractRenderer *renderer,
                               TextureManager *textureNodeManager)
    : m_renderer(renderer)
    , m_textureNodeManager(textureNodeManager)
{
}

Qt3DCore::QBackendNode *TextureFunctor::create(Qt3DCore::QNodeId id) const
{
    Texture *backend = m_textureNodeManager->getOrCreateResource(id);
    backend->setRenderer(m_renderer);
    // Remove id from cleanupList if for some reason we were in the dirty list of texture
    // (Can happen when a node destroyed is followed by a node created change
    // in the same loop, when changing parent for instance)
    m_textureNodeManager->removeTextureIdToCleanup(id);
    return backend;
}

Qt3DCore::QBackendNode *TextureFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_textureNodeManager->lookupResource(id);
}

void TextureFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_textureNodeManager->addTextureIdToCleanup(id);
    // We add ourselves to the dirty list to tell the shared texture managers
    // in the renderer that this texture has been destroyed
    m_textureNodeManager->releaseResource(id);
}


} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
