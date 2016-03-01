/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DCORE_QNODE_H
#define QT3DCORE_QNODE_H

#include <QObject>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qscenechange.h>
#include <Qt3DCore/qabstractnodefactory.h>

#define Q_NODE_NULLPTR static_cast<Qt3DCore::QNode *>(Q_NULLPTR)

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNode;
class QNodePrivate;
class QEntity;
class QAspectEngine;

typedef QList<QNode *> QNodeList;
typedef QSharedPointer<QNode> QNodePtr;

#define QT3DCORE_QUOTE(str) #str
#define QT3D_CLONEABLE(Class)                \
    friend class Qt3DCore::QAbstractNodeFactory;       \
    QNode *doClone() const Q_DECL_OVERRIDE { \
        Class *clone_ = Qt3DCore::QAbstractNodeFactory::createNode<Class>(QT3DCORE_QUOTE(Class)); \
        clone_->copy(this);                   \
        return clone_;                        \
    }

// Each QNode subclass should call QNode::cleanup in it dtor
// QNode::cleanup checks that a flags wasn't set to true,
// sets it to true and sends a clone to the backend

class QT3DCORESHARED_EXPORT QNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QNode *parent READ parentNode WRITE setParent NOTIFY parentChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
public:
    explicit QNode(QNode *parent = Q_NULLPTR);
    virtual ~QNode();

    const QNodeId id() const;
    QNode *parentNode() const;

    bool notificationsBlocked() const;
    bool blockNotifications(bool block);

    QNodeList childrenNodes() const;

    bool isEnabled() const;

public Q_SLOTS:
    virtual void setParent(QNode *parent);
    void setEnabled(bool isEnabled);

Q_SIGNALS:
    void parentChanged(QObject *parent);
    void enabledChanged(bool enabled);

protected:
    // Clone should only be made in the main thread
    static QNode *clone(QNode *node);

    QNode(QNodePrivate &dd, QNode *parent = Q_NULLPTR);
    virtual void copy(const QNode *ref);
    virtual void sceneChangeEvent(const QSceneChangePtr &change);

    void cleanup();

private:
    Q_DECLARE_PRIVATE(QNode)
    virtual QNode *doClone() const = 0;

    // We only want setParent(QNode *) to be callable
    // when dealing with QNode objects
    void setParent(QObject *) Q_DECL_EQ_DELETE;

    Q_PRIVATE_SLOT(d_func(), void _q_addChild(QNode *))
    Q_PRIVATE_SLOT(d_func(), void _q_removeChild(QNode *))

    friend class QAspectEngine;
    friend class QPostman;
    friend class QScene;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif
