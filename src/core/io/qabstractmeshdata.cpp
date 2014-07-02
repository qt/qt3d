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

#include "qabstractmeshdata.h"
#include "qabstractmeshdata_p.h"
#include "qabstractattribute.h"
#include "qabstractbuffer.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

QAbstractMeshDataPrivate::QAbstractMeshDataPrivate(QAbstractMeshData *qq)
    : q_ptr(qq)
{
}

QAbstractMeshData::QAbstractMeshData()
    : d_ptr(new QAbstractMeshDataPrivate(this))
{
}

QAbstractMeshData::~QAbstractMeshData()
{
}

QAbstractMeshData::QAbstractMeshData(QAbstractMeshDataPrivate &dd)
    : d_ptr(&dd)
{
}

void QAbstractMeshData::addAttribute(const QString &name, QAbstractAttributePtr attr)
{
    Q_D(QAbstractMeshData);
    d->m_attributes[name] = attr;
}

void QAbstractMeshData::setIndexAttribute(QAbstractAttributePtr attr)
{
    Q_D(QAbstractMeshData);
    d->m_indexAttr = attr;
}

QStringList QAbstractMeshData::attributeNames() const
{
    Q_D(const QAbstractMeshData);
    return d->m_attributes.keys();
}

QAbstractAttributePtr QAbstractMeshData::attributeByName(QString name) const
{
    Q_D(const QAbstractMeshData);
    return d->m_attributes.value(name);
}

QAbstractAttributePtr QAbstractMeshData::indexAttribute() const
{
    Q_D(const QAbstractMeshData);
    return d->m_indexAttr;
}

int QAbstractMeshData::primitiveCount() const
{
    Q_D(const QAbstractMeshData);
    if (d->m_indexAttr) {
        return d->m_indexAttr->count();
    } else {
        // assume all attribute arrays have the same size
        // will break with instanced drawing, but probably per-instance
        // arrays aren't coming from this code-path.
        // Maybe.
        return d->m_attributes.first()->count();
    }
}

QList<QAbstractBufferPtr> QAbstractMeshData::buffers() const
{
    Q_D(const QAbstractMeshData);
    QSet<QAbstractBufferPtr> r;
    if (d->m_indexAttr)
        r.insert(d->m_indexAttr->buffer());

    Q_FOREACH (QAbstractAttributePtr v, d->m_attributes.values())
        r.insert(v->buffer());

    return r.toList();
}

void QAbstractMeshData::setBoundingBox(const AxisAlignedBoundingBox &bbox)
{
    Q_D(QAbstractMeshData);
    d->m_bbox = bbox;
}

void QAbstractMeshData::computeBoundsFromAttribute(const QString &name)
{
    Q_D(QAbstractMeshData);
    QAbstractAttributePtr attr = attributeByName(name);
    if (!attr) {
        qWarning() << Q_FUNC_INFO << "unknoen attribute:" << name;
        return;
    }
    d->m_bbox.clear();
    d->m_bbox.update(attr->asVector3D());
}

AxisAlignedBoundingBox QAbstractMeshData::boundingBox() const
{
    Q_D(const QAbstractMeshData);
    return d->m_bbox;
}

} // Qt3D

QT_END_NAMESPACE
