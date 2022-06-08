// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_SCENE_P_H
#define QT3DRENDER_RENDER_SCENE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qsceneloader.h>
#include <QtGlobal>
#include <QUrl>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {
namespace Render {

class SceneManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT Scene : public BackendNode
{
public:
    Scene();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    QUrl source() const;
    void setSceneManager(SceneManager *manager);

    void cleanup();
    void setStatus(QSceneLoader::Status status);

private:
    SceneManager *m_sceneManager;
    QUrl m_source;
};

class RenderSceneFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit RenderSceneFunctor(AbstractRenderer *renderer, SceneManager *sceneManager);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    SceneManager *m_sceneManager;
    AbstractRenderer *m_renderer;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_SCENE_P_H
