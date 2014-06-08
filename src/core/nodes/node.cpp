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

#include "node.h"

#include "entity.h"

#include <Qt3DCore/qscenepropertychange.h>

#include <QEvent>
#include <QMetaObject>
#include <QMetaProperty>
#include "corelogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

Node::Node(Node *parent)
    : QObject(parent)
    , m_changeArbiter(Q_NULLPTR)
{
    if (parent)
        parent->addChild(this);
}

Node::~Node()
{
}

void Node::dump()
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
        Node *node = qobject_cast<Node *>(child);
        if (!node)
            continue;
        node->dump();
    }
}

NodeList Node::children() const
{
    return m_children;
}

void Node::addChild(Node *childNode)
{
    Q_CHECK_PTR( childNode );

    if (m_children.contains(childNode))
        return ;

    m_children.append(childNode);
    childNode->setParent(this);
    QScenePropertyChangePtr e(new QScenePropertyChange(NodeCreated, this));
    e->m_propertyName = QByteArrayLiteral("node");
    e->m_value = QVariant::fromValue(childNode);
    notifyObservers(e);
}

void Node::removeChild(Node *childNode)
{
    Q_CHECK_PTR( childNode );
    if (childNode->parent() != this)
        qCWarning(Nodes) << Q_FUNC_INFO << "not a child";

    m_children.removeOne(childNode);
    childNode->setParent(NULL);
    QScenePropertyChangePtr e(new QScenePropertyChange(NodeAboutToBeDeleted, this));
    e->m_propertyName = QByteArrayLiteral("node");
    e->m_value = QVariant::fromValue(childNode);
    notifyObservers(e);
}

void Node::removeAllChildren()
{
    foreach (QObject *child, children()) {
        child->deleteLater();
    }

    m_children.clear();
}

Entity *Node::asEntity()
{
    return Q_NULLPTR;
}

Node *Node::parentNode() const
{
    return qobject_cast<Node*>(parent());
}

void Node::registerObserver(QObserverInterface *observer)
{
    Q_CHECK_PTR(observer);

    // For now we only care about the QChangeArbiter observing us
    QChangeArbiter *changeArbiter = dynamic_cast<QChangeArbiter *>(observer);
    if (changeArbiter) {
        QWriteLocker locker(&m_observerLock);
        m_changeArbiter = changeArbiter;
    }
}

void Node::unregisterObserver(QObserverInterface *observer)
{
    Q_CHECK_PTR(observer);

    // For now we only care about the QChangeArbiter observing us
    QChangeArbiter *changeArbiter = dynamic_cast<QChangeArbiter *>(observer);
    if (changeArbiter) {
        QWriteLocker locker(&m_observerLock);
        m_changeArbiter = Q_NULLPTR;
    }
}

void Node::notifyObservers(const QSceneChangePtr &change)
{
    Q_CHECK_PTR(change);
    QReadLocker locker(&m_observerLock);
    if (m_changeArbiter)
        m_changeArbiter->sceneChangeEventWithLock(change);
}

} // namespace Qt3D

QT_END_NAMESPACE
