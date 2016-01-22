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

#include "qmesh.h"
#include "qmesh_p.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <Qt3DRender/private/objloader_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/qurlhelper_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class MeshFunctor : public QGeometryFunctor
{
public :
    MeshFunctor(const QUrl &sourcePath, const QString &subMesh = QString());
    QGeometry *operator()() Q_DECL_OVERRIDE;
    bool operator ==(const QGeometryFunctor &other) const Q_DECL_OVERRIDE;
    QT3D_FUNCTOR(MeshFunctor)

private:
    QUrl m_sourcePath;
    QString m_subMesh;
};


QMeshPrivate::QMeshPrivate()
    : QGeometryRendererPrivate()
{
}

/*!
 * \qmltype Mesh
 * \instantiates Qt3DRender::QMesh
 * \inqmlmodule Qt3D.Render
 * \brief A custom mesh.
 */

/*!
 * \qmlproperty url Mesh::source
 *
 * Holds the source url to the file containing the custom mesh.
 */

/*!
 * \qmlproperty string Mesh::subMesh
 *
 * Holds the subMesh for the mesh.
 */

/*!
 * \class Qt3DRender::QMesh
 * \inmodule Qt3DRender
 *
 * \inherits Qt3DRender::QGeometryRenderer
 *
 * \brief A custom mesh.
 */

/*!
 * Constructs a new QMesh with \a parent.
 */
QMesh::QMesh(QNode *parent)
    : QGeometryRenderer(*new QMeshPrivate, parent)
{
}

/*!
 * Destroys this mesh.
 */
QMesh::~QMesh()
{
    QNode::cleanup();
}

/*! \internal */
QMesh::QMesh(QMeshPrivate &dd, QNode *parent)
    : QGeometryRenderer(dd, parent)
{
}

void QMesh::setSource(const QUrl& source)
{
    Q_D(QMesh);
    if (d->m_source == source)
        return;
    d->m_source = source;
    // update the functor
    QGeometryRenderer::setGeometryFunctor(QGeometryFunctorPtr(new MeshFunctor(d->m_source, d->m_subMesh)));
    emit sourceChanged(source);
}

/*!
 * \property QMesh::source
 *
 * Holds the \a source url to the file containing the custom mesh.
 */
QUrl QMesh::source() const
{
    Q_D(const QMesh);
    return d->m_source;
}

void QMesh::setSubMesh(const QString &subMesh)
{
    Q_D(QMesh);
    if (d->m_subMesh == subMesh)
        return;
    d->m_subMesh = subMesh;
    // update the functor
    QGeometryRenderer::setGeometryFunctor(QGeometryFunctorPtr(new MeshFunctor(d->m_source, d->m_subMesh)));
    emit subMeshChanged(subMesh);
}

/*!
 * \property QMesh::subMesh
 *
 * Holds the subMesh for the mesh.
 */
QString QMesh::subMesh() const
{
    Q_D(const QMesh);
    return d->m_subMesh;
}

/*!
 * \internal
 */
MeshFunctor::MeshFunctor(const QUrl &sourcePath, const QString& subMesh)
    : QGeometryFunctor()
    , m_sourcePath(sourcePath)
    , m_subMesh(subMesh)
{
}

/*!
 * \internal
 */
QGeometry *MeshFunctor::operator()()
{
    if (m_sourcePath.isEmpty()) {
        qCWarning(Render::Jobs) << Q_FUNC_INFO << "Mesh is empty, nothing to load";
        return Q_NULLPTR;
    }

    // TO DO : Maybe use Assimp instead of ObjLoader to handle more sources
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled(true);
    loader.setTangentGenerationEnabled(true);
    qCDebug(Render::Jobs) << Q_FUNC_INFO << "Loading mesh from" << m_sourcePath << " part:" << m_subMesh;


    // TO DO: Handle file download if remote url
    QString filePath = Qt3DRender::QUrlHelper::urlToLocalFileOrQrc(m_sourcePath);

    if (loader.load(filePath, m_subMesh))
        return loader.geometry();

    qCWarning(Render::Jobs) << Q_FUNC_INFO << "OBJ load failure for:" << filePath;
    return Q_NULLPTR;
}

/*!
 * \internal
 */
bool MeshFunctor::operator ==(const QGeometryFunctor &other) const
{
    const MeshFunctor *otherFunctor = functor_cast<MeshFunctor>(&other);
    if (otherFunctor != Q_NULLPTR)
        return (otherFunctor->m_sourcePath == m_sourcePath);
    return false;
}

} // namespace Qt3DRender

QT_END_NAMESPACE
