/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeometry.h"
#include "qgeometry_p.h"
#include <private/qnode_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/qabstractattribute.h>
#include <Qt3DRender/qboundingvolumespecifier.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

void QGeometryPrivate::_q_boundingVolumeSpecifierChanged(QAbstractAttribute *)
{
    if (m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeUpdated, QSceneChange::Node, m_id));
        change->setPropertyName("boundingVolumeSpecifierPositionAttribute");
        Qt3DCore::QNodeId positionAttributeId;
        if (m_boundingVolumeSpecifier.positionAttribute())
            positionAttributeId = m_boundingVolumeSpecifier.positionAttribute()->id();
        change->setValue(QVariant::fromValue(positionAttributeId));
        notifyObservers(change);
    }
}

QGeometry::QGeometry(QNode *parent)
    : QNode(*new QGeometryPrivate(), parent)
{
    Q_D(QGeometry);
    QObject::connect(&d->m_boundingVolumeSpecifier, SIGNAL(positionAttributeChanged(QAbstractAttribute *)),
                     this, SLOT(_q_boundingVolumeSpecifierChanged(QAbstractAttribute *)));
}

QGeometry::QGeometry(QGeometryPrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
    Q_D(QGeometry);
    QObject::connect(&d->m_boundingVolumeSpecifier, SIGNAL(positionAttributeChanged(QAbstractAttribute *)),
                     this, SLOT(_q_boundingVolumeSpecifierChanged(QAbstractAttribute *)));
}

QGeometry::~QGeometry()
{
    QNode::cleanup();
}

void QGeometry::addAttribute(QAbstractAttribute *attribute)
{
    Q_D(QGeometry);
    if (!d->m_attributes.contains(attribute)) {
        d->m_attributes.append(attribute);

        // We need to add it as a child of the current node if it has been declared inline
        // Or not previously added as a child of the current node so that
        // 1) The backend gets notified about it's creation
        // 2) When the current node is destroyed, it gets destroyed as well
        if (!attribute->parent())
            attribute->setParent(this);

        if (d->m_changeArbiter != Q_NULLPTR) {
            QScenePropertyChangePtr change(new QScenePropertyChange(NodeAdded, QSceneChange::Node, id()));
            change->setPropertyName("attribute");
            change->setValue(QVariant::fromValue(attribute->id()));
            d->notifyObservers(change);
        }
    }
}

void QGeometry::removeAttribute(QAbstractAttribute *attribute)
{
    Q_D(QGeometry);
    if (d->m_changeArbiter != Q_NULLPTR) {
        QScenePropertyChangePtr change(new QScenePropertyChange(NodeRemoved, QSceneChange::Node, id()));
        change->setPropertyName("attribute");
        change->setValue(QVariant::fromValue(attribute->id()));
        d->notifyObservers(change);
    }
    d->m_attributes.removeOne(attribute);
}

void QGeometry::setVerticesPerPatch(int verticesPerPatch)
{
    Q_D(QGeometry);
    if (d->m_verticesPerPatch != verticesPerPatch) {
        d->m_verticesPerPatch = verticesPerPatch;
        emit verticesPerPatchChanged(verticesPerPatch);
    }
}

int QGeometry::verticesPerPatch() const
{
    Q_D(const QGeometry);
    return d->m_verticesPerPatch;
}

QBoundingVolumeSpecifier *QGeometry::boundingVolumeSpecifier()
{
    Q_D(QGeometry);
    return &d->m_boundingVolumeSpecifier;
}

QAttributeList QGeometry::attributes() const
{
    Q_D(const QGeometry);
    return d->m_attributes;
}

void QGeometry::copy(const QNode *ref)
{
    QNode::copy(ref);
    const QGeometry *geometry = static_cast<const QGeometry *>(ref);
    d_func()->m_verticesPerPatch = geometry->d_func()->m_verticesPerPatch;
    Q_FOREACH (QAbstractAttribute *attribute, geometry->d_func()->m_attributes)
        d_func()->m_attributes.append(qobject_cast<QAbstractAttribute *>(QNode::clone(attribute)));
    // Copy bounding volume specifier attribute
    if (geometry->d_func()->m_boundingVolumeSpecifier.positionAttribute() != Q_NULLPTR)
        d_func()->m_boundingVolumeSpecifier.setPositionAttribute(qobject_cast<QAbstractAttribute *>(QNode::clone(geometry->d_func()->m_boundingVolumeSpecifier.positionAttribute())));
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qgeometry.cpp"
