// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QNODE_P_H
#define QT3DCORE_QNODE_P_H

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

#include <Qt3DCore/qnode.h>

#include <functional>
#include <vector>

#include <Qt3DCore/private/propertychangehandler_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <QtCore/private/qobject_p.h>
#include <QQueue>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNode;
class QAspectEngine;

class Q_3DCORE_PRIVATE_EXPORT QNodePrivate : public QObjectPrivate
{
public:
    QNodePrivate();
    ~QNodePrivate();

    void init(QNode *parent);

    virtual void setScene(QScene *scene);
    QScene *scene() const;

    void setArbiter(QChangeArbiter *arbiter);

    void notifyPropertyChange(const char *name, const QVariant &value);
    void notifyDynamicPropertyChange(const QByteArray &name, const QVariant &value);

    void insertTree(QNode *treeRoot, int depth = 0);
    void updatePropertyTrackMode();

    virtual void update();
    void markDirty(QScene::DirtyNodeSet changes);

    Q_DECLARE_PUBLIC(QNode)

    // For now this just protects access to the m_changeArbiter.
    // Later on we may decide to extend support for multiple observers.
    QChangeArbiter *m_changeArbiter;
    QMetaObject *m_typeInfo;
    QScene *m_scene;
    mutable QNodeId m_id;
    QNodeId m_parentId; // Store this so we have it even in parent's QObject dtor
    bool m_blockNotifications;
    bool m_hasBackendNode;
    bool m_enabled;
    bool m_notifiedParent;

    static QNodePrivate *get(QNode *q);
    static const QNodePrivate *get(const QNode *q);
    static void nodePtrDeleter(QNode *q);

    template<typename Caller, typename NodeType>
    using DestructionFunctionPointer = void (Caller::*)(NodeType *);

    template<typename Caller, typename NodeType, typename PropertyType>
    void registerDestructionHelper(NodeType *, DestructionFunctionPointer<Caller, NodeType>, PropertyType);

    template<typename Caller, typename NodeType>
    void registerDestructionHelper(NodeType *node, DestructionFunctionPointer<Caller, NodeType> func, NodeType *&)
    {
        // If the node is destoyed, we make sure not to keep a dangling pointer to it
        Q_Q(QNode);
        auto f = [q, func]() { (static_cast<Caller *>(q)->*func)(nullptr); };
        m_destructionConnections.push_back({node, QObject::connect(node, &QNode::nodeDestroyed, q, f)});
    }

    template<typename Caller, typename NodeType>
    void registerDestructionHelper(NodeType *node, DestructionFunctionPointer<Caller, NodeType> func, QList<NodeType*> &)
    {
        // If the node is destoyed, we make sure not to keep a dangling pointer to it
        Q_Q(QNode);
        auto f = [q, func, node]() { (static_cast<Caller *>(q)->*func)(node); };
        m_destructionConnections.push_back({node, QObject::connect(node, &QNode::nodeDestroyed, q, f)});
    }

    template<typename Caller, typename NodeType>
    void registerDestructionHelper(NodeType *node, DestructionFunctionPointer<Caller, NodeType> func, std::vector<NodeType*> &)
    {
        // If the node is destoyed, we make sure not to keep a dangling pointer to it
        Q_Q(QNode);
        auto f = [q, func, node]() { (static_cast<Caller *>(q)->*func)(node); };
        m_destructionConnections.push_back({node, QObject::connect(node, &QNode::nodeDestroyed, q, f)});
    }

    template<typename Caller, typename ValueType>
    using DestructionFunctionValue = void (Caller::*)(const ValueType&);

    template<typename Caller, typename NodeType, typename ValueType>
    void registerDestructionHelper(NodeType *node, DestructionFunctionValue<Caller, ValueType> func, NodeType *&,
                                   const ValueType &resetValue)
    {
        // If the node is destoyed, we make sure not to keep a dangling pointer to it
        Q_Q(QNode);
        auto f = [q, func, resetValue]() { (static_cast<Caller *>(q)->*func)(resetValue); };
        m_destructionConnections.push_back({node, QObject::connect(node, &QNode::nodeDestroyed, q, f)});
    }

    template<typename Caller, typename NodeType>
    void registerPrivateDestructionHelper(NodeType *node, DestructionFunctionPointer<Caller, NodeType> func)
    {
        Q_Q(QNode);
        // If the node is destoyed, we make sure not to keep a dangling pointer to it
        auto f = [this, func, node]() { (static_cast<Caller *>(this)->*func)(node); };
        m_destructionConnections.push_back({node, QObject::connect(node, &QNode::nodeDestroyed, q, f)});
    }

    void unregisterDestructionHelper(QNode *node)
    {
        m_destructionConnections.erase(std::remove_if(m_destructionConnections.begin(),
                                                      m_destructionConnections.end(),
                                                      [node] (const QPair<QNode *, QMetaObject::Connection> &nodeConnectionPair) {
                                                          if (nodeConnectionPair.first == node) {
                                                              QObject::disconnect(nodeConnectionPair.second);
                                                              return true;
                                                          }
                                                          return false;
                                                      }),
                                       m_destructionConnections.end());
    }

    static const QMetaObject *findStaticMetaObject(const QMetaObject *metaObject);

    void _q_postConstructorInit();
    void _q_ensureBackendNodeCreated();

private:
    void createBackendNode();
    void notifyDestructionChangesAndRemoveFromScene();
    void _q_addChild(QNode *childNode);
    void _q_removeChild(QNode *childNode);
    void _q_setParentHelper(QNode *parent);
    void registerNotifiedProperties();
    void unregisterNotifiedProperties();
    void propertyChanged(int propertyIndex);

    void setSceneHelper(QNode *root);
    void unsetSceneHelper(QNode *root);
    void addEntityComponentToScene(QNode *root);

    friend class PropertyChangeHandler<QNodePrivate>;
    bool m_propertyChangesSetup;
    PropertyChangeHandler<QNodePrivate> m_signals;
    QList<QPair<QNode *, QMetaObject::Connection>> m_destructionConnections;
};

class NodePostConstructorInit : public QObject
{
    Q_OBJECT
public:
    NodePostConstructorInit(QObject *parent = nullptr);
    virtual ~NodePostConstructorInit();
    void removeNode(QNode *node);
    void addNode(QNode *node);

public Q_SLOTS:
    void processNodes();

private:
    QQueue<QNodePrivate *> m_nodesToConstruct;
    bool m_requestedProcessing;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_NODE_P_H
