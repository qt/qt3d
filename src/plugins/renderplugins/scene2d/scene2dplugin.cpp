/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const final
    {
        Backend *backend = m_manager->getOrCreateResource(change->subjectId());
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
                    ::create(renderer, m_scene2dNodeManager), true);
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
