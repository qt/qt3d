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

#include "qabstractmesh.h"
#include "qabstractmesh_p.h"
#include <Qt3DCore/qscenepropertychange.h>

/*!
 * \class Qt3D::QAbstractMesh
 *
 * \brief Provides an abstract class that should be the base of all Mesh
 * primitives in a scene
 *
 * QAbstractMesh subclasses should encapsulate vertices needed to render a Mesh.
 * These should match and be packed according to what the aspect they live in expects.
 *
 * \sa QAbstractTechnique, Component
 */

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QAbstractMeshPrivate
    \internal
*/
QAbstractMeshPrivate::QAbstractMeshPrivate()
    : QComponentPrivate()
{
}

void QAbstractMesh::copy(const QNode *ref)
{
    QComponent::copy(ref);
    const QAbstractMesh *abstractMesh = static_cast<const QAbstractMesh*>(ref);
    d_func()->m_id = abstractMesh->d_func()->m_id;
}


QAbstractMesh::QAbstractMesh(QNode *parent)
    : QComponent(*new QAbstractMeshPrivate, parent)
{
}

/*! \internal */
QAbstractMesh::QAbstractMesh(QAbstractMeshPrivate &dd, QNode *parent)
    : QComponent(dd, parent)
{
}

void QAbstractMesh::update()
{
    Q_D(QAbstractMesh);
        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, id()));
            change->setPropertyName("meshFunctor");
            change->setValue(QVariant::fromValue(meshFunctor()));
            d->notifyObservers(change);
            // TO DO see if we can clear the d->m_dirty on request.
            // This would allow to send a single notification for classes that have several property changes occur
            // over a single given frame or maybe that's the job of the QChangeArbiter
        }
}

} // Qt3D

QT_END_NAMESPACE
