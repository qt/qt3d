// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QPLANEGEOMETRY_H
#define QT3DEXTRAS_QPLANEGEOMETRY_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DCore/qgeometry.h>
#include <QtCore/QSize>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;

} // Qt3DCore

namespace Qt3DExtras {

class QPlaneGeometryPrivate;

class Q_3DEXTRASSHARED_EXPORT QPlaneGeometry : public Qt3DCore::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QSize resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(bool mirrored READ mirrored WRITE setMirrored NOTIFY mirroredChanged REVISION 9)
    Q_PROPERTY(Qt3DCore::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *texCoordAttribute READ texCoordAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *tangentAttribute READ tangentAttribute CONSTANT)
    Q_PROPERTY(Qt3DCore::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit QPlaneGeometry(QNode *parent = nullptr);
    ~QPlaneGeometry();

    void updateVertices();
    void updateIndices();

    QSize resolution() const;
    float width() const;
    float height() const;
    bool mirrored() const;

    Qt3DCore::QAttribute *positionAttribute() const;
    Qt3DCore::QAttribute *normalAttribute() const;
    Qt3DCore::QAttribute *texCoordAttribute() const;
    Qt3DCore::QAttribute *tangentAttribute() const;
    Qt3DCore::QAttribute *indexAttribute() const;

public Q_SLOTS:
    void setResolution(const QSize &resolution);
    void setWidth(float width);
    void setHeight(float height);
    void setMirrored(bool mirrored);

Q_SIGNALS:
    void resolutionChanged(const QSize &resolution);
    void widthChanged(float width);
    void heightChanged(float height);
    void mirroredChanged(bool mirrored);

protected:
    QPlaneGeometry(QPlaneGeometryPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QPlaneGeometry)
};

} // Qt3DExpoerimental

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QPLANEGEOMETRY_H
