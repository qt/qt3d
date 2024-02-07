// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "textureimage_p.h"
#include <Qt3DRender/qtextureimage.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/qabstracttextureimage_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

using namespace Qt3DCore;

TextureImage::TextureImage()
    : BackendNode(ReadWrite)
    , m_dirty(false)
    , m_layer(0)
    , m_mipLevel(0)
    , m_face(QAbstractTexture::CubeMapPositiveX)
{
}

TextureImage::~TextureImage()
{
}

void TextureImage::cleanup()
{
    m_generator.reset();
    m_dirty = false;
    m_layer = 0;
    m_mipLevel = 0;
    m_face = QAbstractTexture::CubeMapPositiveX;
}

void TextureImage::syncFromFrontEnd(const QNode *frontEnd, bool firstTime)
{
    const QAbstractTextureImage *node = qobject_cast<const QAbstractTextureImage *>(frontEnd);
    if (!node)
        return;

    const bool oldEnabled = isEnabled();
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    m_dirty |= (oldEnabled != isEnabled());

    if (node->layer() != m_layer) {
        m_layer = node->layer();
        m_dirty = true;
    }

    if (node->mipLevel() != m_mipLevel) {
        m_mipLevel = node->mipLevel();
        m_dirty = true;
    }

    if (node->face() != m_face) {
        m_face = node->face();
        m_dirty = true;
    }

    const QAbstractTextureImagePrivate *d = static_cast<const QAbstractTextureImagePrivate *>(QNodePrivate::get(node));
    if (d->dataGenerator() != m_generator) {
        m_generator = d->dataGenerator();
        m_dirty = true;
    }

    if (m_dirty)
        markDirty(AbstractRenderer::AllDirty);
}

void TextureImage::unsetDirty()
{
    m_dirty = false;
}

TextureImageFunctor::TextureImageFunctor(AbstractRenderer *renderer,
                                         TextureImageManager *textureImageManager)
    : m_renderer(renderer)
    , m_textureImageManager(textureImageManager)
{
}

Qt3DCore::QBackendNode *TextureImageFunctor::create(Qt3DCore::QNodeId id) const
{
    TextureImage *backend = m_textureImageManager->getOrCreateResource(id);
    backend->setRenderer(m_renderer);
    return backend;
}

Qt3DCore::QBackendNode *TextureImageFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_textureImageManager->lookupResource(id);
}

void TextureImageFunctor::destroy(Qt3DCore::QNodeId id) const
{
    m_textureImageManager->releaseResource(id);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
