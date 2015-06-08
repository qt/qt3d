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

#ifndef QT3D_QNODE_H
#define QT3D_QNODE_H

#include <QObject>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qscenechange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QNode;
class QNodePrivate;
class QEntity;
class QAspectEngine;
class QSceneInterface;

typedef QList<QNode *> QNodeList;
typedef QSharedPointer<QNode> QNodePtr;

#define QT3D_CLONEABLE(Class)                \
    QNode *doClone() const Q_DECL_OVERRIDE { \
        Class *clone_ = new Class;            \
        clone_->copy(this);                   \
        return clone_;                        \
    }

class QT3DCORESHARED_EXPORT QNode : public QObject
{
    Q_OBJECT
public:
    explicit QNode(QNode *parent = 0);
    ~QNode();

    const QNodeId id() const;
    QNode *parentNode() const;

    bool notificationsBlocked() const;
    bool blockNotifications(bool block);

protected:
    // Clone should only be made in the main thread
    static QNode *clone(QNode *node);

    QNode(QNodePrivate &dd, QNode *parent = 0);
    virtual void copy(const QNode *ref);
    virtual void sceneChangeEvent(const QSceneChangePtr &change);
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    QSceneInterface *scene() const;

private:
    Q_DECLARE_PRIVATE(QNode)
    virtual QNode *doClone() const = 0;

    friend class QAspectEngine;
    friend class QPostman;
    friend class QScene;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif
