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
****************************************************************************/

#ifndef QT3DRENDER_QSCREENRAYCASTER_H
#define QT3DRENDER_QSCREENRAYCASTER_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/QAbstractRayCaster>

#include <QtGui/QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_EXPORT QScreenRayCaster : public QAbstractRayCaster
{
    Q_OBJECT

    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
public:
    explicit QScreenRayCaster(QNode *parent = nullptr);
    ~QScreenRayCaster();

    QPoint position() const;

public Q_SLOTS:
    void setPosition(const QPoint &position);

    void trigger();
    void trigger(const QPoint &position);

Q_SIGNALS:
    void positionChanged(const QPoint &position);

protected:
    explicit QScreenRayCaster(QAbstractRayCasterPrivate &dd, QNode *parent = nullptr);
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DRENDER_QSCREENRAYCASTER_H
