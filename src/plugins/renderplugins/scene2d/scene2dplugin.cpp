// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "scene2dplugin.h"

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DQuickScene2D/qscene2d.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

template <typename Backend>
class Scene2DBackendNodeMapper : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit Scene2DBackendNodeMapper(Render::AbstractRenderer *renderer,
                                      Render::Scene2DNodeManager *manager)
        : m_manager(manager)
        , m_renderer(renderer)
    {
    }

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const final
    {
        Backend *backend = m_manager->getOrCreateResource(id);
        backend->setRenderer(m_renderer);
        return backend;
    }

    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const final
    {
        return m_manager->lookupResource(id);
    }

    void destroy(Qt3DCore::QNodeId id) const final
    {
        m_manager->releaseResource(id);
    }

private:
    Render::Scene2DNodeManager *m_manager;
    Render::AbstractRenderer *m_renderer;
};

Scene2DPlugin::Scene2DPlugin()
    : m_scene2dNodeManager(new Render::Scene2DNodeManager())
{

}

Scene2DPlugin::~Scene2DPlugin()
{
    delete m_scene2dNodeManager;
}

bool Scene2DPlugin::registerBackendTypes(QRenderAspect *aspect,
                                         AbstractRenderer *renderer)
{
    registerBackendType(aspect, Qt3DRender::Quick::QScene2D::staticMetaObject,
                QSharedPointer<Scene2DBackendNodeMapper<Render::Quick::Scene2D> >
                    ::create(renderer, m_scene2dNodeManager));
    return true;
}
bool Scene2DPlugin::unregisterBackendTypes(QRenderAspect *aspect)
{
    unregisterBackendType(aspect, Qt3DRender::Quick::QScene2D::staticMetaObject);
    return true;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
