// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QGEOMETRY_H
#define QT3DCORE_QGEOMETRY_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qattribute.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QGeometryPrivate;

class Q_3DCORESHARED_EXPORT QGeometry : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QAttribute *boundingVolumePositionAttribute READ boundingVolumePositionAttribute WRITE setBoundingVolumePositionAttribute NOTIFY boundingVolumePositionAttributeChanged)
    Q_PROPERTY(QVector3D minExtent READ minExtent NOTIFY minExtentChanged QT3D_PROPERTY_REVISION(2, 13))
    Q_PROPERTY(QVector3D maxExtent READ maxExtent NOTIFY maxExtentChanged QT3D_PROPERTY_REVISION(2, 13))
public:
    explicit QGeometry(Qt3DCore::QNode *parent = nullptr);
    ~QGeometry();

    QList<QAttribute *> attributes() const;
    Q_INVOKABLE void addAttribute(Qt3DCore::QAttribute *attribute);
    Q_INVOKABLE void removeAttribute(Qt3DCore::QAttribute *attribute);

    QAttribute *boundingVolumePositionAttribute() const;
    QVector3D minExtent() const;
    QVector3D maxExtent() const;

public Q_SLOTS:
    void setBoundingVolumePositionAttribute(QAttribute *boundingVolumePositionAttribute);

Q_SIGNALS:
    void boundingVolumePositionAttributeChanged(QAttribute *boundingVolumePositionAttribute);
    QT3D_REVISION(2, 13) void minExtentChanged(const QVector3D &minExtent);
    QT3D_REVISION(2, 13) void maxExtentChanged(const QVector3D &maxExtent);
protected:
    explicit QGeometry(QGeometryPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QGeometry)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QGEOMETRY_H
