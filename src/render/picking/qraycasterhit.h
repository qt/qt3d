/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QT3DRENDER_QRAYCASTERHIT_H
#define QT3DRENDER_QRAYCASTERHIT_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qt3drender_global.h>

#include <QtCore/QSharedData>
#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRayCasterHitData;
class QAbstractRayCasterPrivate;

class Q_3DRENDERSHARED_EXPORT QRayCasterHit
{
    Q_GADGET
    Q_PROPERTY(QRayCasterHit::HitType type READ type CONSTANT)
    Q_PROPERTY(Qt3DCore::QNodeId entityId READ entityId CONSTANT)
    Q_PROPERTY(Qt3DCore::QEntity *entity READ entity CONSTANT)
    Q_PROPERTY(float distance READ distance CONSTANT)
    Q_PROPERTY(QVector3D localIntersection READ localIntersection CONSTANT)
    Q_PROPERTY(QVector3D worldIntersection READ worldIntersection CONSTANT)
    Q_PROPERTY(uint primitiveIndex READ primitiveIndex CONSTANT)
    Q_PROPERTY(uint vertex1Index READ vertex1Index CONSTANT)
    Q_PROPERTY(uint vertex2Index READ vertex2Index CONSTANT)
    Q_PROPERTY(uint vertex3Index READ vertex3Index CONSTANT)
public:
    enum HitType {
        TriangleHit,
        LineHit,
        PointHit,
        EntityHit
    };
    Q_ENUM(HitType)

    QRayCasterHit();
    explicit QRayCasterHit(QRayCasterHit::HitType type, Qt3DCore::QNodeId id, float distance,
                  const QVector3D &localIntersect, const QVector3D &worldIntersect,
                  uint primitiveIndex, uint v1, uint v2, uint v3);
    QRayCasterHit(const QRayCasterHit &other);
    ~QRayCasterHit();

    QRayCasterHit &operator =(const QRayCasterHit &other);

    HitType type() const;
    Qt3DCore::QNodeId entityId() const;
    Qt3DCore::QEntity *entity() const;
    float distance() const;
    QVector3D localIntersection() const;
    QVector3D worldIntersection() const;
    uint primitiveIndex() const;
    uint vertex1Index() const;
    uint vertex2Index() const;
    uint vertex3Index() const;

    Q_INVOKABLE QString toString();

private:
    friend class QAbstractRayCasterPrivate;
    void setEntity(Qt3DCore::QEntity *entity) const;

    QSharedDataPointer<QRayCasterHitData> d;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DRENDER_QRAYCASTERHIT_H
