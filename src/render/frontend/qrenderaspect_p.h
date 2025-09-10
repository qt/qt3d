// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERASPECT_P_H
#define QT3DRENDER_QRENDERASPECT_P_H

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

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/private/expandboundingvolumejob_p.h>
#include <Qt3DRender/private/updateworldtransformjob_p.h>
#include <Qt3DRender/private/updateworldboundingvolumejob_p.h>
#include <Qt3DRender/private/calcboundingvolumejob_p.h>
#include <Qt3DRender/private/updateskinningpalettejob_p.h>
#include <Qt3DRender/private/updateentitylayersjob_p.h>
#include <Qt3DRender/private/updatetreeenabledjob_p.h>
#include <Qt3DRender/private/genericlambdajob_p.h>
#include <Qt3DRender/private/pickboundingvolumejob_p.h>
#include <Qt3DRender/private/raycastingjob_p.h>

#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

class QSurface;
class QScreen;
class QRhi;
namespace Qt3DRender {

class QSceneImporter;

namespace Render {
class AbstractRenderer;
class NodeManagers;
class QRenderPlugin;
}

namespace Render {
class OffscreenSurfaceHelper;
class PickEventFilter;

using SynchronizerJobPtr = GenericLambdaJobPtr<std::function<void()>>;

class UpdateLevelOfDetailJob;
typedef QSharedPointer<UpdateLevelOfDetailJob> UpdateLevelOfDetailJobPtr;
}

class Q_3DRENDERSHARED_PRIVATE_EXPORT QRenderAspectPrivate : public Qt3DCore::QAbstractAspectPrivate
{
public:
    QRenderAspectPrivate(QRenderAspect::SubmissionType submissionType);
    ~QRenderAspectPrivate();

    Q_DECLARE_PUBLIC(QRenderAspect)

    static QRenderAspectPrivate* findPrivate(Qt3DCore::QAspectEngine *engine);
    static QRenderAspectPrivate *get(QRenderAspect *q);

    void jobsDone() override;
    void frameDone() override;

    void createNodeManagers();
    void onEngineStartup();
    void onEngineAboutToShutdown() override;

    void registerBackendTypes();
    void unregisterBackendTypes();
    void loadSceneImporters();
    void loadRenderPlugin(const QString &pluginName);
    void registerBackendType(const QMetaObject &, const Qt3DCore::QBackendNodeMapperPtr &functor);
    std::vector<Qt3DCore::QAspectJobPtr> createGeometryRendererJobs() const;
    std::vector<Qt3DCore::QAspectJobPtr> createPreRendererJobs() const;
    std::vector<Qt3DCore::QAspectJobPtr> createRenderBufferJobs() const;
    Render::AbstractRenderer *loadRendererPlugin();

    bool processMouseEvent(QObject *obj, QMouseEvent *event);
    bool processKeyEvent(QObject *obj, QKeyEvent *event);

    Render::NodeManagers *m_nodeManagers;
    Render::AbstractRenderer *m_renderer;

    bool m_initialized;
    const bool m_renderAfterJobs;
    bool m_sceneImportersLoaded;
    QList<QSceneImporter *> m_sceneImporters;
    QList<QString> m_loadedPlugins;
    QList<Render::QRenderPlugin *> m_renderPlugins;
    Render::OffscreenSurfaceHelper *m_offscreenHelper;
    QScreen *m_screen = nullptr;

    Render::UpdateTreeEnabledJobPtr m_updateTreeEnabledJob;
    Render::UpdateWorldTransformJobPtr m_worldTransformJob;
    Render::ExpandBoundingVolumeJobPtr m_expandBoundingVolumeJob;
    Render::CalculateBoundingVolumeJobPtr m_calculateBoundingVolumeJob;
    Render::UpdateWorldBoundingVolumeJobPtr m_updateWorldBoundingVolumeJob;
    Render::UpdateSkinningPaletteJobPtr m_updateSkinningPaletteJob;
    Render::UpdateLevelOfDetailJobPtr m_updateLevelOfDetailJob;
    Render::UpdateEntityLayersJobPtr m_updateEntityLayersJob;
    Render::SynchronizerJobPtr m_syncLoadingJobs;
    Render::PickBoundingVolumeJobPtr m_pickBoundingVolumeJob;
    Render::RayCastingJobPtr m_rayCastingJob;

    QScopedPointer<Render::PickEventFilter> m_pickEventFilter;
    QRenderAspect::SubmissionType m_submissionType;

    static QMutex m_pluginLock;
    static QList<QString> m_pluginConfig;
    static QList<QRenderAspectPrivate *> m_instances;
    static void configurePlugin(const QString &plugin);
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERASPECT_P_H
