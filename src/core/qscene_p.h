// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QSCENE_P_H
#define QT3DCORE_QSCENE_P_H

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

#include <QtCore/QScopedPointer>

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qabstractfrontendnodemanager_p.h>
#include <QtCore/qscopedpointer.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QScenePrivate;
class QAspectEngine;
class NodePostConstructorInit;
class QChangeArbiter;

class Q_3DCORE_PRIVATE_EXPORT QScene : public QAbstractFrontEndNodeManager
{
public:
    // Changes made to backend nodes are reported to the Renderer
    enum DirtyNodeFlag {
        TransformDirty      = 1 << 0,
        GeometryDirty       = 1 << 1,
        EntityEnabledDirty  = 1 << 2,
        BuffersDirty        = 1 << 3,
        AllDirty            = 0xffffff
    };
    Q_DECLARE_FLAGS(DirtyNodeSet, DirtyNodeFlag)

    QScene(QAspectEngine *engine = nullptr);
    ~QScene();

    QAspectEngine *engine() const;

    void addObservable(QNode *observable);
    void removeObservable(QNode *observable);

    QNode *lookupNode(QNodeId id) const override;
    QList<QNode *> lookupNodes(const QList<QNodeId> &ids) const override;

    QNode *rootNode() const;

    void setArbiter(QChangeArbiter *arbiter);
    QChangeArbiter *arbiter() const;

    // Component -> Entities
    QList<QNodeId> entitiesForComponent(QNodeId id) const;
    void addEntityForComponent(QNodeId componentUuid, QNodeId entityUuid);
    void removeEntityForComponent(QNodeId componentUuid, QNodeId entityUuid);
    bool hasEntityForComponent(QNodeId componentUuid, QNodeId entityUuid);

    NodePostConstructorInit* postConstructorInit() const;

    void markDirty(DirtyNodeSet changes);
    DirtyNodeSet dirtyBits();
    void clearDirtyBits();

private:
    Q_DECLARE_PRIVATE(QScene)
    QScopedPointer<QScenePrivate> d_ptr;

    void setRootNode(QNode *root);
    friend class QAspectEnginePrivate;
};

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DCore::QScene*) // LCOV_EXCL_LINE

#endif // QT3DCORE_QSCENE_P_H
