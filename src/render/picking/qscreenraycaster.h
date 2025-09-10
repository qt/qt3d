// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    Qt3DRender::QAbstractRayCaster::Hits pick(const QPoint &position);

Q_SIGNALS:
    void positionChanged(const QPoint &position);

protected:
    explicit QScreenRayCaster(QAbstractRayCasterPrivate &dd, QNode *parent = nullptr);
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DRENDER_QSCREENRAYCASTER_H
