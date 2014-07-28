/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QNODE_H
#define QT3D_QNODE_H

#include <QObject>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qobservableinterface.h>
#include <Qt3DCore/qchangearbiter.h>

#include <QReadWriteLock>
#include <QUuid>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QNode;
class QNodePrivate;
class QEntity;

typedef QList<QNode *> NodeList;

class QT3DCORESHARED_EXPORT QNode : public QObject, public QObservableInterface
{
    Q_OBJECT

public:
    explicit QNode(QNode *parent = 0);
    ~QNode();

    void dump();

    const QUuid uuid() const;

    NodeList children() const;
    void addChild(QNode *childNode);
    void removeChild(QNode *childNode);

    QNode *clone(QNode *clonedParent = 0) const;
    virtual void copy(const QNode *ref);

    void removeAllChildren();

    virtual QEntity* asEntity();

    QNode *parentNode() const;

    void registerObserver(QObserverInterface *observer) Q_DECL_OVERRIDE;
    void unregisterObserver(QObserverInterface *observer) Q_DECL_OVERRIDE;

protected:
    virtual void notifyObservers(const QSceneChangePtr &change);
    virtual QNode *doClone(QNode *clonedParent = 0) const = 0;

    Q_DECLARE_PRIVATE(QNode)
    QNode(QNodePrivate &dd, QNode *parent = 0);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_sendQueuedChanges())
};

} // namespace Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::QNode *)

#endif
