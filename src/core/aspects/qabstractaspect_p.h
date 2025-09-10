// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QABSTRACTASPECT_P_H
#define QT3DCORE_QABSTRACTASPECT_P_H

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

#include <QtCore/QHash>
#include <Qt3DCore/qabstractaspect.h>

#include <Qt3DCore/private/qaspectjobproviderinterface_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <QtCore/private/qobject_p.h>

#include <QMutex>
#include <QList>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAbstractAspect;
class QBackendNode;
class QEntity;
class QAspectManager;
class QAbstractAspectJobManager;
class QServiceLocator;

namespace Debug {

class Q_3DCORE_PRIVATE_EXPORT AsynchronousCommandReply : public QObject
{
    Q_OBJECT
public:
    explicit AsynchronousCommandReply(const QString &commandName, QObject *parent = nullptr);

    inline QByteArray data() const { return m_data; }
    inline QString commandName() const { return m_commandName; }
    inline bool isFinished() const { return m_finished; }

    void setFinished(bool finished);
    void setData(const QByteArray &data);

Q_SIGNALS:
    void finished(AsynchronousCommandReply *reply);

private:
    QByteArray m_data;
    QString m_commandName;
    bool m_finished;
};

} // Debug

struct NodeTreeChange
{
    enum NodeTreeChangeType {
        Added = 0,
        Removed = 1
    };
    Qt3DCore::QNodeId id;
    const QMetaObject *metaObj;
    NodeTreeChangeType type;
    Qt3DCore::QNode *node;
};

class Q_3DCORE_PRIVATE_EXPORT QAbstractAspectPrivate
        : public QObjectPrivate
        , public QAspectJobProviderInterface
{
public:
    QAbstractAspectPrivate();
    ~QAbstractAspectPrivate();

    void setRootAndCreateNodes(QEntity *rootObject, const QList<NodeTreeChange> &nodesTreeChanges);

    QServiceLocator *services() const;
    QAbstractAspectJobManager *jobManager() const;

    std::vector<QAspectJobPtr> jobsToExecute(qint64 time) override;
    void jobsDone() override;      // called when all the jobs are completed
    void frameDone() override;     // called when frame is completed (after the jobs), safe to wait until next frame here

    QBackendNode *createBackendNode(const NodeTreeChange &change) const;
    void clearBackendNode(const NodeTreeChange &change) const;
    void syncDirtyFrontEndNodes(const QList<QNode *> &nodes);
    void syncDirtyEntityComponentNodes(const QList<ComponentRelationshipChange> &nodes);
    virtual void syncDirtyFrontEndNode(QNode *node, QBackendNode *backend, bool firstTime) const;

    virtual void onEngineAboutToShutdown();

    // TODO: Make these public in 5.8
    template<class Frontend>
    void unregisterBackendType();
    void unregisterBackendType(const QMetaObject &mo);

    Q_DECLARE_PUBLIC(QAbstractAspect)

    QBackendNodeMapperPtr mapperForNode(const QMetaObject *metaObj) const;

    QEntity *m_root;
    QNodeId m_rootId;
    QAspectManager *m_aspectManager;
    QAbstractAspectJobManager *m_jobManager;
    QChangeArbiter *m_arbiter;
    QHash<const QMetaObject*, QBackendNodeMapperPtr> m_backendCreatorFunctors;
    QMutex m_singleShotMutex;
    std::vector<QAspectJobPtr> m_singleShotJobs;

    static QAbstractAspectPrivate *get(QAbstractAspect *aspect);
};

template<class Frontend>
void QAbstractAspectPrivate::unregisterBackendType()
{
    unregisterBackendType(Frontend::staticMetaObject);
}

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QABSTRACTASPECT_P_H
