// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QCLIPPLANE_H
#define QT3DRENDER_QCLIPPLANE_H

#include <Qt3DRender/qrenderstate.h>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QClipPlanePrivate;

class Q_3DRENDERSHARED_EXPORT QClipPlane : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(int planeIndex READ planeIndex WRITE setPlaneIndex NOTIFY planeIndexChanged)
    Q_PROPERTY(QVector3D normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(float distance READ distance WRITE setDistance NOTIFY distanceChanged)
public:
    explicit QClipPlane(Qt3DCore::QNode *parent = nullptr);
    ~QClipPlane();

    int planeIndex() const;
    QVector3D normal() const;
    float distance() const;

public Q_SLOTS:
    void setPlaneIndex(int);
    void setNormal(QVector3D);
    void setDistance(float);

Q_SIGNALS:
    void planeIndexChanged(int planeIndex);
    void normalChanged(QVector3D normal);
    void distanceChanged(float distance);

private:
    Q_DECLARE_PRIVATE(QClipPlane)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QCLIPPLANE_H
