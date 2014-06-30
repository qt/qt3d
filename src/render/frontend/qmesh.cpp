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

#include "qmesh.h"
#include "qmesh_p.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include <objloader.h>

#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QMeshPrivate::QMeshPrivate(QMesh *qq)
    : QAbstractMeshPrivate(qq)
    , m_sourceDirty(false)
{}

QMesh::QMesh(Node *parent)
    : QAbstractMesh(*new QMeshPrivate(this), parent)
{
}

QMesh::QMesh(QMeshPrivate &dd, Node *parent)
    : QAbstractMesh(dd, parent)
{
}

void QMesh::setSource( const QString& source )
{
    Q_D(QMesh);
    if (QAbstractMesh::source() == source)
        return;
    QAbstractMesh::setSource(source);
    d->m_sourceDirty = true;

    // Let aspects know about the change
    QScenePropertyChangePtr e(new QScenePropertyChange(ComponentUpdated, this));
    e->m_propertyName = QByteArrayLiteral("source");
    e->m_value = QAbstractMesh::source();
    notifyObservers(e);
}

MeshDataPtr QMesh::data() const
{
    Q_D(const QMesh);
    return d->m_data;
}

void QMesh::setData(MeshDataPtr data)
{
    Q_D(QMesh);
    d->m_data = data;
}

} // namespace Qt3D

QT_END_NAMESPACE
