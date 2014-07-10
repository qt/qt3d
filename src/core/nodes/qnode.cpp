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

#include "qnode.h"
#include "qnode_p.h"

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <QEvent>
#include <QMetaObject>
#include <QMetaProperty>
#include "corelogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QNode::QNode(QNode *parent)
    : QObject(*new QNodePrivate(this), parent)
{
    if (parent)
        parent->addChild(this);
}

QNode::QNode(QNodePrivate &dd, QNode *parent)
    : QObject(dd, parent)
{
    if (parent)
        parent->addChild(this);
}

QNode::~QNode()
{
    removeAllChildren();
}

void QNode::dump()
{
    const QMetaObject *meta = metaObject();
    QStringList result;
    for (int i = 0; i < meta->propertyCount(); ++i) {
        const QMetaProperty metaProperty = meta->property(i);
        const QVariant value = property(metaProperty.name());
        result += QString(QStringLiteral("%1 %2 = %3;"))
                .arg(QString::fromLatin1(metaProperty.typeName()))
                .arg(QString::fromLatin1(metaProperty.name()))
                .arg(value.toString());
    }

    qCDebug(Nodes) << result.join(QStringLiteral("\n"));

    foreach (QObject *child, children()) {
        QNode *node = qobject_cast<QNode *>(child);
        if (!node)
            continue;
        node->dump();
    }
}

const QUuid QNode::uuid() const
{
    Q_D(const QNode);
    return d->m_uuid;
}

NodeList QNode::children() const
{
    Q_D(const QNode);
    return d->m_children;
}

void QNode::addChild(QNode *childNode)
{
    Q_ASSERT(childNode);
    Q_D(QNode);
    if (d->m_children.contains(childNode))
        return ;

    d->m_children.append(childNode);
    childNode->setParent(this);

    QScenePropertyChangePtr e(new QScenePropertyChange(NodeCreated, this));
    e->setPropertyName(QByteArrayLiteral("node"));
    e->setValue(QVariant::fromValue(childNode));
    notifyObservers(e);
}

void QNode::removeChild(QNode *childNode)
{
    Q_ASSERT(childNode);
    if (childNode->parent() != this)
        qCWarning(Nodes) << Q_FUNC_INFO << "not a child of " << this;

    Q_D(QNode);
    d->m_children.removeOne(childNode);
    childNode->setParent(NULL);

    QScenePropertyChangePtr e(new QScenePropertyChange(NodeAboutToBeDeleted, this));
    e->setPropertyName(QByteArrayLiteral("node"));
    e->setValue(QVariant::fromValue(childNode->uuid()));
    notifyObservers(e);
}

void QNode::removeAllChildren()
{
    Q_FOREACH (QObject *child, children())
        if (qobject_cast<QNode *>(child))
            removeChild(qobject_cast<QNode *>(child));
}

QEntity *QNode::asEntity()
{
    return Q_NULLPTR;
}

QNode *QNode::parentNode() const
{
    return qobject_cast<QNode*>(parent());
}

void QNode::registerObserver(QObserverInterface *observer)
{
    Q_CHECK_PTR(observer);

    // For now we only care about the QChangeArbiter observing us
    QChangeArbiter *changeArbiter = dynamic_cast<QChangeArbiter *>(observer);
    if (changeArbiter) {
        Q_D(QNode);
        QWriteLocker locker(&d->m_observerLock);
        d->m_changeArbiter = changeArbiter;
    }
}

void QNode::unregisterObserver(QObserverInterface *observer)
{
    Q_CHECK_PTR(observer);

    // For now we only care about the QChangeArbiter observing us
    Q_D(QNode);
    QChangeArbiter *changeArbiter = dynamic_cast<QChangeArbiter *>(observer);
    if (changeArbiter == d->m_changeArbiter) {
        QWriteLocker locker(&d->m_observerLock);
        d->m_changeArbiter = Q_NULLPTR;
    }
}

void QNode::notifyObservers(const QSceneChangePtr &change)
{
    Q_CHECK_PTR(change);
    Q_D(QNode);
    QReadLocker locker(&d->m_observerLock);
    QChangeArbiter *changeArbiter = d->m_changeArbiter;
    locker.unlock();
    // There is a deadlock issue as sceneChangeEventWithLock locks the QChangeArbiter's mutex
    // while d->m_observerLock is locked by the locker right above.
    // In the case that a call the QChangeArbiter registerObserver which locks the QChangeArviter's mutex
    // and calls registerObserver on the same Node with locks d->m_observerLock
    if (changeArbiter != Q_NULLPTR)
        changeArbiter->sceneChangeEventWithLock(change);
}

} // namespace Qt3D

QT_END_NAMESPACE
