// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QCUBOIDGEOMETRY_H
#define QT3DEXTRAS_QCUBOIDGEOMETRY_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/qgeometry.h>
#include <QtCore/QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;

} // Qt3DCore

namespace Qt3DExtras {

class QCuboidGeometryPrivate;

class Q_3DEXTRASSHARED_EXPORT QCuboidGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(float xExtent READ xExtent WRITE setXExtent NOTIFY xExtentChanged)
    Q_PROPERTY(float yExtent READ yExtent WRITE setYExtent NOTIFY yExtentChanged)
    Q_PROPERTY(float zExtent READ zExtent WRITE setZExtent NOTIFY zExtentChanged)
    Q_PROPERTY(QSize xyMeshResolution READ xyMeshResolution WRITE setXYMeshResolution NOTIFY xyMeshResolutionChanged)
    Q_PROPERTY(QSize yzMeshResolution READ yzMeshResolution WRITE setYZMeshResolution NOTIFY yzMeshResolutionChanged)
    Q_PROPERTY(QSize xzMeshResolution READ xzMeshResolution WRITE setXZMeshResolution NOTIFY xzMeshResolutionChanged)
    Q_PROPERTY(Qt3DCore::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *texCoordAttribute READ texCoordAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *tangentAttribute READ tangentAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit QCuboidGeometry(QNode *parent = nullptr);
    ~QCuboidGeometry();

    void updateIndices();
    void updateVertices();

    float xExtent() const;
    float yExtent() const;
    float zExtent() const;
    QSize yzMeshResolution() const;
    QSize xyMeshResolution() const;
    QSize xzMeshResolution() const;

    Qt3DCore::QAttribute *positionAttribute() const;
    Qt3DCore::QAttribute *normalAttribute() const;
    Qt3DCore::QAttribute *texCoordAttribute() const;
    Qt3DCore::QAttribute *tangentAttribute() const;
    Qt3DCore::QAttribute *indexAttribute() const;

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

protected:
    QCuboidGeometry(QCuboidGeometryPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QCuboidGeometry)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QCUBOIDGEOMETRY_H
