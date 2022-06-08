// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QCONEGEOMETRY_H
#define QT3DEXTRAS_QCONEGEOMETRY_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/qgeometry.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QAttribute;
} // Render

namespace Qt3DExtras {

class QConeGeometryPrivate;

class Q_3DEXTRASSHARED_EXPORT QConeGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
    Q_PROPERTY( bool hasTopEndcap READ hasTopEndcap WRITE setHasTopEndcap NOTIFY hasTopEndcapChanged )
    Q_PROPERTY( bool hasBottomEndcap READ hasBottomEndcap WRITE setHasBottomEndcap NOTIFY hasBottomEndcapChanged )
    Q_PROPERTY(int rings READ rings WRITE setRings NOTIFY ringsChanged)
    Q_PROPERTY(int slices READ slices WRITE setSlices NOTIFY slicesChanged)
    Q_PROPERTY( float topRadius READ topRadius WRITE setTopRadius NOTIFY topRadiusChanged )
    Q_PROPERTY( float bottomRadius READ bottomRadius WRITE setBottomRadius NOTIFY bottomRadiusChanged )
    Q_PROPERTY(float length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(Qt3DCore::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *texCoordAttribute READ texCoordAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit QConeGeometry(QNode *parent = nullptr);
    ~QConeGeometry();

    void updateVertices();
    void updateIndices();

    bool hasTopEndcap() const;
    bool hasBottomEndcap() const;
    float topRadius() const;
    float bottomRadius() const;
    int rings() const;
    int slices() const;
    float length() const;

    Qt3DCore::QAttribute *positionAttribute() const;
    Qt3DCore::QAttribute *normalAttribute() const;
    Qt3DCore::QAttribute *texCoordAttribute() const;
    Qt3DCore::QAttribute *indexAttribute() const;

public Q_SLOTS:
    void setHasTopEndcap( bool hasTopEndcap );
    void setHasBottomEndcap( bool hasBottomEndcap );
    void setTopRadius( float topRadius );
    void setBottomRadius( float bottomRadius );
    void setRings( int rings );
    void setSlices( int slices );
    void setLength( float length );

Q_SIGNALS:
    void hasTopEndcapChanged( bool hasTopEndcap );
    void hasBottomEndcapChanged( bool hasBottomEndcap );
    void topRadiusChanged( float topRadius );
    void bottomRadiusChanged( float bottomRadius );
    void ringsChanged( int rings );
    void slicesChanged( int slices );
    void lengthChanged( float length );

protected:
    QConeGeometry(QConeGeometryPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QConeGeometry)
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QCONEGEOMETRY_H
