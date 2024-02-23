// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef ORBITTRANSFORMCONTROLLER_H
#define ORBITTRANSFORMCONTROLLER_H

#include <QObject>
#include <QMatrix4x4>

#include <Qt3DCore/qtransform.h>

QT_BEGIN_NAMESPACE

class OrbitTransformController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QTransform* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(float angle READ angle WRITE setAngle NOTIFY angleChanged)

public:
    OrbitTransformController(QObject *parent = 0);

    void setTarget(Qt3DCore::QTransform *target);
    Qt3DCore::QTransform *target() const;

    void setRadius(float radius);
    float radius() const;

    void setAngle(float angle);
    float angle() const;

signals:
    void targetChanged();
    void radiusChanged();
    void angleChanged();

protected:
    void updateMatrix();

private:
    Qt3DCore::QTransform *m_target;
    QMatrix4x4 m_matrix;
    float m_radius;
    float m_angle;
};

QT_END_NAMESPACE

#endif // ORBITTRANSFORMCONTROLLER_H
