// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QCONEGEOMETRYVIEW_H
#define QT3DEXTRAS_QCONEGEOMETRYVIEW_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/qgeometryview.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class Q_3DEXTRASSHARED_EXPORT QConeGeometryView : public Qt3DCore::QGeometryView
{
    Q_OBJECT
    Q_PROPERTY(int rings READ rings WRITE setRings NOTIFY ringsChanged)
    Q_PROPERTY(int slices READ slices WRITE setSlices NOTIFY slicesChanged)
    Q_PROPERTY( bool hasTopEndcap READ hasTopEndcap WRITE setHasTopEndcap NOTIFY hasTopEndcapChanged )
    Q_PROPERTY( bool hasBottomEndcap READ hasBottomEndcap WRITE setHasBottomEndcap NOTIFY hasBottomEndcapChanged )
    Q_PROPERTY( float topRadius READ topRadius WRITE setTopRadius NOTIFY topRadiusChanged )
    Q_PROPERTY( float bottomRadius READ bottomRadius WRITE setBottomRadius NOTIFY bottomRadiusChanged )
    Q_PROPERTY(float length READ length WRITE setLength NOTIFY lengthChanged)
public:
    explicit QConeGeometryView(Qt3DCore::QNode *parent = nullptr);
    ~QConeGeometryView();

    int rings() const;
    int slices() const;
    bool hasTopEndcap() const;
    bool hasBottomEndcap() const;
    float topRadius() const;
    float bottomRadius() const;
    float length() const;

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

private:
    // As this is a default provided geometry renderer, no one should be able
    // to modify the QGeometryRenderer's properties

    void setInstanceCount(int instanceCount);
    void setVertexCount(int vertexCount);
    void setIndexOffset(int indexOffset);
    void setFirstInstance(int firstInstance);
    void setRestartIndexValue(int index);
    void setPrimitiveRestartEnabled(bool enabled);
    void setGeometry(Qt3DCore::QGeometry *geometry);
    void setPrimitiveType(PrimitiveType primitiveType);
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QCONEGEOMETRYVIEW_H
