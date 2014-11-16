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

#include "qmeshdata.h"
#include "qmeshdata_p.h"
#include <Qt3DCore/qabstractattribute.h>
#include <Qt3DCore/qabstractbuffer.h>
#include <QOpenGLVertexArrayObject>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QMeshDataPrivate::QMeshDataPrivate(QMeshData *qq)
    : q_ptr(qq)
    , m_verticesPerPatch(0)
    , m_primitiveType(0)
{
}

QMeshData::QMeshData(PrimitiveTypes primitiveType)
    : d_ptr(new QMeshDataPrivate(this))
{
    setPrimitiveType(primitiveType);
}

QMeshData::~QMeshData()
{
}

QMeshData::QMeshData(QMeshDataPrivate &dd)
    : d_ptr(&dd)
{
}

void QMeshData::addAttribute(const QString &name, QAbstractAttributePtr attr)
{
    Q_D(QMeshData);
    d->m_attributes.insert(name, attr);
}

void QMeshData::setIndexAttribute(QAbstractAttributePtr attr)
{
    Q_D(QMeshData);
    d->m_indexAttr = attr;
}

QStringList QMeshData::attributeNames() const
{
    Q_D(const QMeshData);
    return d->m_attributes.keys();
}

QAbstractAttributePtr QMeshData::attributeByName(QString name) const
{
    Q_D(const QMeshData);
    return d->m_attributes.value(name);
}

QAbstractAttributePtr QMeshData::indexAttribute() const
{
    Q_D(const QMeshData);
    return d->m_indexAttr;
}

void QMeshData::setVerticesPerPatch(int verticesPerPatch)
{
    Q_D(QMeshData);
    d->m_verticesPerPatch = verticesPerPatch;
}

int QMeshData::verticesPerPatch() const
{
    Q_D(const QMeshData);
    return d->m_verticesPerPatch;
}

int QMeshData::primitiveCount() const
{
    Q_D(const QMeshData);
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

QList<QAbstractBufferPtr> QMeshData::buffers() const
{
    Q_D(const QMeshData);
    QSet<QAbstractBufferPtr> r;
    if (d->m_indexAttr)
        r.insert(d->m_indexAttr->buffer());

    Q_FOREACH (QAbstractAttributePtr v, d->m_attributes.values())
        r.insert(v->buffer());

    return r.toList();
}

void QMeshData::setBoundingBox(const QAxisAlignedBoundingBox &bbox)
{
    Q_D(QMeshData);
    d->m_bbox = bbox;
}

void QMeshData::computeBoundsFromAttribute(const QString &name)
{
    Q_D(QMeshData);
    QAbstractAttributePtr attr = attributeByName(name);
    if (!attr) {
        qWarning() << Q_FUNC_INFO << "unknoen attribute:" << name;
        return;
    }
    d->m_bbox.clear();
    d->m_bbox.update(attr->asVector3D());
}

QAxisAlignedBoundingBox QMeshData::boundingBox() const
{
    Q_D(const QMeshData);
    return d->m_bbox;
}

void QMeshData::setPrimitiveType(PrimitiveTypes primitiveType)
{
    Q_D(QMeshData);
    d->m_primitiveType = primitiveType;
}

int QMeshData::primitiveType() const
{
    Q_D(const QMeshData);
    return d->m_primitiveType;
}

} // Qt3D

QT_END_NAMESPACE
