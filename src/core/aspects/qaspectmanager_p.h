/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include <Qt3DCore/qnodecreatedchange.h>
#include <Qt3DCore/qaspectengine.h>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QSemaphore>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3DCore {

class QNode;
class QEntity;
class QScheduler;
class QChangeArbiter;
class QAbstractAspect;
class QAbstractAspectJobManager;
class QAspectEngine;
class QServiceLocator;
class NodePostConstructorInit;
struct NodeTreeChange;
#if QT_CONFIG(animation)
class RequestFrameAnimation;
#endif

class Q_3DCORE_PRIVATE_EXPORT QAspectManager : public QObject
{
    Q_OBJECT
public:
    explicit QAspectManager(QAspectEngine *parent = nullptr);
    ~QAspectManager();

    void setRunMode(QAspectEngine::RunMode mode);
    void enterSimulationLoop();
    void exitSimulationLoop();

    bool isShuttingDown() const;

public Q_SLOTS:
    void initialize();
    void shutdown();
    void processFrame();

    void setRootEntity(Qt3DCore::QEntity *root, const QVector<QNode *> &nodes);
    void addNodes(const QVector<QNode *> &nodes);
    void removeNodes(const QVector<QNode *> &nodes);
    void registerAspect(Qt3DCore::QAbstractAspect *aspect);
    void unregisterAspect(Qt3DCore::QAbstractAspect *aspect);

public:
    const QVector<QAbstractAspect *> &aspects() const;
    QAbstractAspectJobManager *jobManager() const;
    QChangeArbiter *changeArbiter() const;
    QServiceLocator *serviceLocator() const;
    void setPostConstructorInit(NodePostConstructorInit *postConstructorInit);

    QNode *lookupNode(QNodeId id) const;
    QVector<QNode *> lookupNodes(const QVector<QNodeId> &ids) const;

    int jobsInLastFrame() const { return m_jobsInLastFrame; }
    void dumpJobsOnNextFrame();

private:
#if !QT_CONFIG(animation)
    bool event(QEvent *event) override;
#endif
    void requestNextFrame();

    QAspectEngine *m_engine;
    QVector<QAbstractAspect *> m_aspects;
    QEntity *m_root;
    QVariantMap m_data;
    QScheduler *m_scheduler;
    QAbstractAspectJobManager *m_jobManager;
    QChangeArbiter *m_changeArbiter;
    QScopedPointer<QServiceLocator> m_serviceLocator;
    bool m_simulationLoopRunning;
    QAspectEngine::RunMode m_driveMode;
    QVector<NodeTreeChange> m_nodeTreeChanges;
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
