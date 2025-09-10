// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QCHANGEARBITER_P_H
#define QT3DCORE_QCHANGEARBITER_P_H

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

#include <Qt3DCore/qnodeid.h>
#include <QtCore/QFlags>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QReadWriteLock>
#include <QtCore/QThreadStorage>
#include <QtCore/QVariant>

#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNode;
class QObservableInterface;
class QAbstractAspectJobManager;
class QSceneObserverInterface;
class QAbstractPostman;
class QScene;
class QEntity;
class QComponent;

struct ComponentRelationshipChange
{
    QNode *node;
    QNode *subNode;

    enum RelationShip {
        Added = 0,
        Removed
    };
    RelationShip change;
};

class Q_3DCORE_PRIVATE_EXPORT QChangeArbiter
        : public QObject
{
    Q_OBJECT
public:
    explicit QChangeArbiter(QObject *parent = nullptr);
    ~QChangeArbiter();

    void addDirtyFrontEndNode(QNode *node);
    void addDirtyEntityComponentNodes(QEntity *entity, QComponent *component,
                                      ComponentRelationshipChange::RelationShip change);

    void removeDirtyFrontEndNode(QNode *node);
    QList<QNode *> takeDirtyFrontEndNodes();
    QList<ComponentRelationshipChange> takeDirtyEntityComponentNodes();

    void setScene(Qt3DCore::QScene *scene);

    QScene *scene() const;

Q_SIGNALS:
    void receivedChange();

protected:
    QScene *m_scene;
    QList<QNode *> m_dirtyFrontEndNodes;
    QList<ComponentRelationshipChange> m_dirtyEntityComponentNodeChanges;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QCHANGEARBITER_P_H
