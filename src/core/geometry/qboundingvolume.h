// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QBOUNDINGVOLUME_H
#define QT3DCORE_QBOUNDINGVOLUME_H

#include <QtGui/qvector3d.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qgeometryview.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QBoundingVolumePrivate;

class Q_3DCORESHARED_EXPORT QBoundingVolume : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(QGeometryView* view READ view WRITE setView NOTIFY viewChanged)
    Q_PROPERTY(QVector3D implicitMinPoint READ implicitMinPoint NOTIFY implicitMinPointChanged)
    Q_PROPERTY(QVector3D implicitMaxPoint READ implicitMaxPoint NOTIFY implicitMaxPointChanged)
    Q_PROPERTY(bool implicitPointsValid READ areImplicitPointsValid NOTIFY implicitPointsValidChanged)
    Q_PROPERTY(QVector3D minPoint READ minPoint WRITE setMinPoint NOTIFY minPointChanged)
    Q_PROPERTY(QVector3D maxPoint READ maxPoint WRITE setMaxPoint NOTIFY maxPointChanged)
public:
    explicit QBoundingVolume(Qt3DCore::QNode *parent = nullptr);
    ~QBoundingVolume();

    QGeometryView* view() const;
    QVector3D implicitMinPoint() const;
    QVector3D implicitMaxPoint() const;
    bool areImplicitPointsValid() const;
    QVector3D minPoint() const;
    QVector3D maxPoint() const;

public Q_SLOTS:
    void setView(QGeometryView *view);
    void setMinPoint(const QVector3D &minPoint);
    void setMaxPoint(const QVector3D &maxPoint);

    bool updateImplicitBounds();

Q_SIGNALS:
    void viewChanged(QGeometryView *view);
    void implicitMinPointChanged(const QVector3D &implicitMinPoint);
    void implicitMaxPointChanged(const QVector3D &implicitMaxPoint);
    void implicitPointsValidChanged(bool implicitPointsValid);
    void minPointChanged(QVector3D minPoint);
    void maxPointChanged(QVector3D maxPoint);

protected:
    QBoundingVolume(QBoundingVolumePrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QBoundingVolume)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QBOUNDINGVOLUME_H
