// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QSPHEREGEOMETRY_H
#define QT3DEXTRAS_QSPHEREGEOMETRY_H

#include <Qt3DCore/qgeometry.h>
#include <Qt3DExtras/qt3dextras_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;

} // Qt3DCore

namespace Qt3DExtras {

class QSphereGeometryPrivate;

class Q_3DEXTRASSHARED_EXPORT QSphereGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(int rings READ rings WRITE setRings NOTIFY ringsChanged)
    Q_PROPERTY(int slices READ slices WRITE setSlices NOTIFY slicesChanged)
    Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(bool generateTangents READ generateTangents WRITE setGenerateTangents NOTIFY generateTangentsChanged)
    Q_PROPERTY(Qt3DCore::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *texCoordAttribute READ texCoordAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *tangentAttribute READ tangentAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit QSphereGeometry(QNode *parent = nullptr);
    ~QSphereGeometry();

    void updateVertices();
    void updateIndices();

    bool generateTangents() const;
    int rings() const;
    int slices() const;
    float radius() const;

    Qt3DCore::QAttribute *positionAttribute() const;
    Qt3DCore::QAttribute *normalAttribute() const;
    Qt3DCore::QAttribute *texCoordAttribute() const;
    Qt3DCore::QAttribute *tangentAttribute() const;
    Qt3DCore::QAttribute *indexAttribute() const;

public Q_SLOTS:
    void setRings(int rings);
    void setSlices(int slices);
    void setRadius(float radius);
    void setGenerateTangents(bool gen);

Q_SIGNALS:
    void radiusChanged(float radius);
    void ringsChanged(int rings);
    void slicesChanged(int slices);
    void generateTangentsChanged(bool generateTangents);

protected:
    QSphereGeometry(QSphereGeometryPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QSphereGeometry)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSPHEREGEOMETRY_H
