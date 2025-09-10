// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QASPECTENGINE_P_H
#define QT3DCORE_QASPECTENGINE_P_H

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

#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <QtCore/qsharedpointer.h>

#include <Qt3DCore/private/qaspectfactory_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qabstractfrontendnodemanager_p.h>
#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QEntity;
class QNode;
class QAspectManager;
class QScene;

class Q_3DCORE_PRIVATE_EXPORT QAspectEnginePrivate : public QObjectPrivate, public QAbstractFrontEndNodeManager
{
public:
    QAspectEnginePrivate();
    ~QAspectEnginePrivate();

    Q_DECLARE_PUBLIC(QAspectEngine)

    QAspectFactory m_factory;
    QAspectManager *m_aspectManager;
    QScene *m_scene;
    QSharedPointer<QEntity> m_root;
    QList<QAbstractAspect*> m_aspects;
    QHash<QString, QAbstractAspect *> m_namedAspects;
    bool m_initialized;
    QAspectEngine::RunMode m_runMode;

    void initialize();
    void shutdown();

    void exitSimulationLoop();

    void initNodeTree(QNode *node);
    void initNode(QNode *node);
    void initEntity(QEntity *entity);
    void addNode(QNode *node);
    void removeNode(QNode *node);

    QNode *lookupNode(QNodeId id) const override;
    QList<QNode *> lookupNodes(const QList<QNodeId> &ids) const override;

    static QAspectEnginePrivate *get(QAspectEngine *engine);
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DCORE_QASPECTENGINE_P_H
