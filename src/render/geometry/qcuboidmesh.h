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

#ifndef QT3DRENDER_CUBOIDMESH_H
#define QT3DRENDER_CUBOIDMESH_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QT3DRENDERSHARED_EXPORT QCuboidMesh : public QGeometryRenderer
{
    Q_OBJECT

    Q_PROPERTY(float xExtent READ xExtent WRITE setXExtent NOTIFY xExtentChanged)
    Q_PROPERTY(float yExtent READ yExtent WRITE setYExtent NOTIFY yExtentChanged)
    Q_PROPERTY(float zExtent READ zExtent WRITE setZExtent NOTIFY zExtentChanged)
    Q_PROPERTY(QSize yzMeshResolution READ yzMeshResolution WRITE setYZMeshResolution NOTIFY yzMeshResolutionChanged)
    Q_PROPERTY(QSize xzMeshResolution READ xzMeshResolution WRITE setXZMeshResolution NOTIFY xzMeshResolutionChanged)
    Q_PROPERTY(QSize xyMeshResolution READ xyMeshResolution WRITE setXYMeshResolution NOTIFY xyMeshResolutionChanged)

public:
    explicit QCuboidMesh(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QCuboidMesh();

    float xExtent() const;
    float yExtent() const;
    float zExtent() const;
    QSize yzMeshResolution() const;
    QSize xzMeshResolution() const;
    QSize xyMeshResolution() const;

public Q_SLOTS:
    void setXExtent(float xExtent);
    void setYExtent(float yExtent);
    void setZExtent(float zExtent);
    void setYZMeshResolution(const QSize &resolution);
    void setXZMeshResolution(const QSize &resolution);
    void setXYMeshResolution(const QSize &resolution);

Q_SIGNALS:
    void xExtentChanged(float xExtent);
    void yExtentChanged(float yExtent);
    void zExtentChanged(float zExtent);

    void yzMeshResolutionChanged(const QSize &yzMeshResolution);
    void xzMeshResolutionChanged(const QSize &xzMeshResolution);
    void xyMeshResolutionChanged(const QSize &xyMeshResolution);

private:
    // As this is a default provided geometry renderer, no one should be able
    // to modify the QGeometryRenderer's properties

    void setInstanceCount(int instanceCount);
    void setPrimitiveCount(int primitiveCount);
    void setBaseVertex(int baseVertex);
    void setBaseInstance(int baseInstance);
    void setRestartIndex(int index);
    void setPrimitiveRestart(bool enabled);
    void setGeometry(QGeometry *geometry);
    void setPrimitiveType(PrimitiveType primitiveType);
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_CUBOIDMESH_H
