/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qcollisionaspect.h"
#include "qcollisionaspect_p.h"

#include <Qt3DCore/qnodevisitor.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qaspectfactory.h>
#include <Qt3DCore/qservicelocator.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QCollisionAspectPrivate
    \internal
*/
QCollisionAspectPrivate::QCollisionAspectPrivate()
    : QAbstractAspectPrivate()
    , m_time(0)
{
    m_aspectType = QAbstractAspect::AspectCollision;
}

QCollisionAspect::QCollisionAspect(QObject *parent)
    : QAbstractAspect(*new QCollisionAspectPrivate(), parent)
{
    registerBackendTypes();
}

/*! \internal */
QCollisionAspect::QCollisionAspect(QCollisionAspectPrivate &dd, QObject *parent)
    : QAbstractAspect(dd, parent)
{
    registerBackendTypes();
}

void QCollisionAspect::registerBackendTypes()
{
    //registerBackendType<QSphereCollider>(QBackendNodeFunctorPtr(new Collision::SphereColliderFunctor(d_func()->m_manager.data())));
}

QVector<QAspectJobPtr> QCollisionAspect::jobsToExecute(qint64 time)
{
    Q_D(QCollisionAspect);
    d->m_time = time;

    // Create jobs that will get exectued by the threadpool
    QVector<QAspectJobPtr> jobs;
    return jobs;
}

void QCollisionAspect::sceneNodeAdded(QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
    QNode *n = nodePtr.data();
    QNodeVisitor visitor;
    visitor.traverse(n, this, &QCollisionAspect::visitNode);
}

void QCollisionAspect::sceneNodeRemoved(QSceneChangePtr &e)
{
    QScenePropertyChangePtr propertyChange = e.staticCast<QScenePropertyChange>();
    QNodePtr nodePtr = propertyChange->value().value<QNodePtr>();
    QNode *n = nodePtr.data();
    QAbstractAspect::clearBackendNode(n);
}

void QCollisionAspect::setRootEntity(QEntity *rootObject)
{
    QNodeVisitor visitor;
    visitor.traverse(rootObject, this, &QCollisionAspect::visitNode);
}

void QCollisionAspect::onInitialize(const QVariantMap &data)
{
    Q_UNUSED(data);
}

void QCollisionAspect::onCleanup()
{
}

void QCollisionAspect::visitNode(QNode *node)
{
    QAbstractAspect::createBackendNode(node);
}

} // Qt3D

QT_END_NAMESPACE

QT3D_REGISTER_NAMESPACED_ASPECT("collision", QT_PREPEND_NAMESPACE(Qt3D), QCollisionAspect)

