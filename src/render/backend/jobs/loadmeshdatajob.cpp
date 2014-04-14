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

#include "loadmeshdatajob.h"

#include <objloader.h>
#include <sphere.h>

#include <renderer.h>
#include <meshmanager.h>

#include <QDebug>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

LoadMeshDataJob::LoadMeshDataJob(const QString &source, const HMeshData &meshDataHandle)
    : QJob()
    , m_source(source)
    , m_meshDataHandle(meshDataHandle)
{
}

void LoadMeshDataJob::run()
{
    qDebug() << "Entering" << Q_FUNC_INFO << QThread::currentThread();

    // Load the mesh from disk (or wherever)
    qDebug() << "Loading mesh from" << m_source;

    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled(true);

    MeshData *meshData = Q_NULLPTR;
    if (loader.load(m_source) &&
            (meshData = m_renderer->meshManager()->data(m_meshDataHandle)) != Q_NULLPTR) {
        qDebug() << Q_FUNC_INFO << "Loaded OBJ ok";
        meshData = loader.mesh();
        AttributePtr attr = meshData->attributeByName(QStringLiteral("position"));
        if (!attr) {
            qWarning() << Q_FUNC_INFO << "unknown attribute: position";
            return;
        }
    } else {
        qWarning() << Q_FUNC_INFO << "OBJ load failure for:" << m_source;
    }

    //Qt3D::Sphere sphere = Qt3D::Sphere::fromPoints(loader.vertices());

    qDebug() << "Exiting" << Q_FUNC_INFO << QThread::currentThread();
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
