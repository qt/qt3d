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

#ifndef QT3DRENDER_QRAYCASTER_H
#define QT3DRENDER_QRAYCASTER_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/QAbstractRayCaster>

#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_EXPORT QRayCaster : public QAbstractRayCaster
{
    Q_OBJECT
    Q_PROPERTY(QVector3D origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(QVector3D direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(float length READ length WRITE setLength NOTIFY lengthChanged)

public:
    explicit QRayCaster(QNode *parent = nullptr);
    ~QRayCaster();

    QVector3D origin() const;
    QVector3D direction() const;
    float length() const;

public Q_SLOTS:
    void setOrigin(const QVector3D& origin);
    void setDirection(const QVector3D& direction);
    void setLength(float length);

    void trigger();
    void trigger(const QVector3D& origin, const QVector3D& direction, float length);

    Hits pick(const QVector3D& origin, const QVector3D& direction, float length);

Q_SIGNALS:
    void originChanged(const QVector3D &origin);
    void directionChanged(const QVector3D &direction);
    void lengthChanged(float length);

protected:
    explicit QRayCaster(QAbstractRayCasterPrivate &dd, QNode *parent = nullptr);
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DRENDER_QRAYCASTER_H
