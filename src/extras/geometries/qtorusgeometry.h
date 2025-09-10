// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTORUSGEOMETRY_H
#define QT3DEXTRAS_QTORUSGEOMETRY_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/qgeometry.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;

} // Qt3DCore

namespace Qt3DExtras {

class QTorusGeometryPrivate;

class Q_3DEXTRASSHARED_EXPORT QTorusGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(int rings READ rings WRITE setRings NOTIFY ringsChanged)
    Q_PROPERTY(int slices READ slices WRITE setSlices NOTIFY slicesChanged)
    Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(float minorRadius READ minorRadius WRITE setMinorRadius NOTIFY minorRadiusChanged)
    Q_PROPERTY(Qt3DCore::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *texCoordAttribute READ texCoordAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit QTorusGeometry(QNode *parent = nullptr);
    ~QTorusGeometry();

    void updateVertices();
    void updateIndices();

    int rings() const;
    int slices() const;
    float radius() const;
    float minorRadius() const;

    Qt3DCore::QAttribute *positionAttribute() const;
    Qt3DCore::QAttribute *normalAttribute() const;
    Qt3DCore::QAttribute *texCoordAttribute() const;
    Qt3DCore::QAttribute *indexAttribute() const;

public Q_SLOTS:
    void setRings(int rings);
    void setSlices(int slices);
    void setRadius(float radius);
    void setMinorRadius(float minorRadius);

Q_SIGNALS:
    void radiusChanged(float radius);
    void ringsChanged(int rings);
    void slicesChanged(int slices);
    void minorRadiusChanged(float minorRadius);


protected:
    QTorusGeometry(QTorusGeometryPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QTorusGeometry)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTORUSGEOMETRY_H
