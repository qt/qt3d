// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QASPECTMANAGER_P_H
#define QT3DCORE_QASPECTMANAGER_P_H

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

#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/private/qabstractfrontendnodemanager_p.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qnodeid.h>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QSemaphore>
#include <QtCore/QVariant>

#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3DCore {

class QEntity;
class QScheduler;
class QChangeArbiter;
class QAbstractAspect;
class QAbstractAspectJobManager;
class QAspectEngine;
class QServiceLocator;
class QScene;
class NodePostConstructorInit;
struct NodeTreeChange;
#if QT_CONFIG(animation)
class RequestFrameAnimation;
#endif

class Q_3DCORE_PRIVATE_EXPORT QAspectManager : public QObject, public QAbstractFrontEndNodeManager
{
    Q_OBJECT
public:
    explicit QAspectManager(QAspectEngine *parent = nullptr);
    ~QAspectManager();

    QAspectEngine * engine() const { return  m_engine; }
    QScheduler *scheduler() const { return m_scheduler; }

    void setRunMode(QAspectEngine::RunMode mode);
    void enterSimulationLoop();
    void exitSimulationLoop();

    bool isShuttingDown() const;

public Q_SLOTS:
    void initialize();
    void shutdown();
    void processFrame();

    void setRootEntity(Qt3DCore::QEntity *root, const QList<QNode *> &nodes);
    void addNodes(const QList<QNode *> &nodes);
    void removeNodes(const QList<QNode *> &nodes);
    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void unregisterAspect(Qt3DCore::QAbstractAspect *aspect);

public:
    const QList<QAbstractAspect *> &aspects() const;
    QAbstractAspect *aspect(const QString &name) const;
    QAbstractAspect *aspect(const QMetaObject *metaType) const;
    QAbstractAspectJobManager *jobManager() const;
    QChangeArbiter *changeArbiter() const;
    QServiceLocator *serviceLocator() const;
    void setPostConstructorInit(NodePostConstructorInit *postConstructorInit);

    QNode *lookupNode(QNodeId id) const override;
    QList<QNode *> lookupNodes(const QList<QNodeId> &ids) const override;
    QScene *scene() const;

    int jobsInLastFrame() const { return m_jobsInLastFrame; }
    void dumpJobsOnNextFrame();

private:
#if !QT_CONFIG(animation)
    bool event(QEvent *event) override;
#endif
    void requestNextFrame();

    QAspectEngine *m_engine;
    QList<QAbstractAspect *> m_aspects;
    QEntity *m_root;
    QVariantMap m_data;
    QScheduler *m_scheduler;
    QAbstractAspectJobManager *m_jobManager;
    QChangeArbiter *m_changeArbiter;
    QScopedPointer<QServiceLocator> m_serviceLocator;
    bool m_simulationLoopRunning;
    QAspectEngine::RunMode m_driveMode;
    QList<NodeTreeChange> m_nodeTreeChanges;
    NodePostConstructorInit* m_postConstructorInit;

#if QT_CONFIG(animation)
    RequestFrameAnimation *m_simulationAnimation;
#endif
    int m_jobsInLastFrame;
    bool m_dumpJobs;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QASPECTMANAGER_P_H
