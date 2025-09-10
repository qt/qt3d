// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QNODE_H
#define QT3DCORE_QNODE_H

#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QObject>

#define Q_NODE_NULLPTR static_cast<Qt3DCore::QNode *>(nullptr)

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNode;
class QNodePrivate;
class QAspectEngine;

#if defined(QT_BUILD_INTERNAL)
class QBackendNodeTester;
#endif

using QNodeVector = QList<QNode *>;
using QNodePtr = QSharedPointer<QNode>;

class Q_3DCORESHARED_EXPORT QNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QNode *parent READ parentNode WRITE setParent NOTIFY parentChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
public:

    explicit QNode(QNode *parent = nullptr);
    virtual ~QNode();

    QNodeId id() const;
    QNode *parentNode() const;

    bool notificationsBlocked() const;
    bool blockNotifications(bool block);

    QNodeVector childNodes() const;

    bool isEnabled() const;

public Q_SLOTS:
    void setParent(QNode *parent);
    void setEnabled(bool isEnabled);

Q_SIGNALS:
    void parentChanged(QObject *parent);
    void enabledChanged(bool enabled);
    void nodeDestroyed();

protected:
    explicit QNode(QNodePrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QNode)

    // We only want setParent(QNode *) to be callable
    // when dealing with QNode objects
    void setParent(QObject *) = delete;

    Q_PRIVATE_SLOT(d_func(), void _q_postConstructorInit())
    Q_PRIVATE_SLOT(d_func(), void _q_addChild(Qt3DCore::QNode *))
    Q_PRIVATE_SLOT(d_func(), void _q_removeChild(Qt3DCore::QNode *))
    Q_PRIVATE_SLOT(d_func(), void _q_setParentHelper(Qt3DCore::QNode *))

    friend class QAspectEngine;
    friend class QAspectEnginePrivate;
    friend class QAbstractAspectPrivate;
    friend class QScene;

#if defined(QT_BUILD_INTERNAL)
    friend class QBackendNodeTester;
#endif
};

inline QNodeId qIdForNode(QNode *node){ return node ? node->id() : QNodeId(); }

template<typename T>
inline QNodeIdVector qIdsForNodes(const T &nodes)
{
    QNodeIdVector ids;
    ids.reserve(nodes.size());
    for (const auto n : nodes)
        ids.push_back(n->id());
    return ids;
}

struct QNodeIdTypePair
{
    QNodeIdTypePair() noexcept
        : id()
        , type(nullptr)
    {}

    explicit QNodeIdTypePair(QNodeId _id, const QMetaObject *_type) noexcept
        : id(_id)
        , type(_type)
    {}

    QNodeId id;
    const QMetaObject *type;
};
QT3D_DECLARE_TYPEINFO(Qt3DCore, QNodeIdTypePair, Q_PRIMITIVE_TYPE)

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif
