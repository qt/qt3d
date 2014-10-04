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

#include <Qt3DRenderer/private/objloader_p.h>

#include <Qt3DCore/qscenepropertychange.h>
#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

class MeshFunctor : public QAbstractMeshFunctor
{
public :
    MeshFunctor(const QString &sourcePath);
    QAbstractMeshDataPtr operator()() Q_DECL_OVERRIDE;
    bool operator ==(const QAbstractMeshFunctor &other) const Q_DECL_OVERRIDE;

private:
    QString m_sourcePath;
};


QMeshPrivate::QMeshPrivate(QMesh *qq)
    : QAbstractMeshPrivate(qq)
{}

void QMeshPrivate::copy(const QNodePrivate *ref)
{
    QAbstractMeshPrivate::copy(ref);
    const QMeshPrivate *mesh = static_cast<const QMeshPrivate *>(ref);
    m_source = mesh->m_source;
}

QMesh::QMesh(QNode *parent)
    : QAbstractMesh(*new QMeshPrivate(this), parent)
{
}

QMesh::QMesh(QMeshPrivate &dd, QNode *parent)
    : QAbstractMesh(dd, parent)
{
}

QMesh *QMesh::doClone() const
{
    QMesh *clone = new QMesh();
    clone->d_func()->copy(d_func());
    return clone;
}

void QMesh::setSource( const QString& source )
{
    Q_D(QMesh);
    if (d->m_source == source)
        return;
    d->m_source = source;
    emit sourceChanged();
    // Let aspects know about the change
    QAbstractMesh::setDirty(true);
}

QString QMesh::source() const
{
    Q_D(const QMesh);
    return d->m_source;
}

QAbstractMeshFunctorPtr QMesh::meshFunctor() const
{
    Q_D(const QMesh);
    return QAbstractMeshFunctorPtr(new MeshFunctor(d->m_source));
}

MeshFunctor::MeshFunctor(const QString &sourcePath)
    : QAbstractMeshFunctor()
    , m_sourcePath(sourcePath)
{
}

QAbstractMeshDataPtr MeshFunctor::operator()()
{
    if (m_sourcePath.isEmpty()) {
        qCWarning(Render::Jobs) << Q_FUNC_INFO << "Mesh is empty, nothing to load";
        return QMeshDataPtr();
    }

    // TO DO : Maybe use Assimp instead of ObjLoader to handle more sources
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled(true);
    loader.setTangentGenerationEnabled(true);
    qCDebug(Render::Jobs) << Q_FUNC_INFO << "Loading mesh from" << m_sourcePath;

    if (loader.load(m_sourcePath))
        return QMeshDataPtr(loader.mesh());

    qCWarning(Render::Jobs) << Q_FUNC_INFO << "OBJ load failure for:" << m_sourcePath;
    return QMeshDataPtr();
}

bool MeshFunctor::operator ==(const QAbstractMeshFunctor &other) const
{
    const MeshFunctor *otherFunctor = dynamic_cast<const MeshFunctor *>(&other);
    if (otherFunctor != Q_NULLPTR)
        return (otherFunctor->m_sourcePath == m_sourcePath);
    return false;
}

} // namespace Qt3D

QT_END_NAMESPACE
