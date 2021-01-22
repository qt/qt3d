/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QPICKPOINTEVENT_H
#define QT3DRENDER_QPICKPOINTEVENT_H

#include <Qt3DRender/qpickevent.h>
#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QPickPointEventPrivate;

class Q_3DRENDERSHARED_EXPORT QPickPointEvent : public QPickEvent
{
    Q_OBJECT
    Q_PROPERTY(uint pointIndex READ pointIndex CONSTANT)
public:
    QPickPointEvent();
    QPickPointEvent(const QPointF &position, const QVector3D& worldIntersection, const QVector3D& localIntersection, float distance,
                    uint pointIndex, Buttons button, int buttons, int modifiers);
    ~QPickPointEvent();

public:
    uint pointIndex() const;

private:
    Q_DECLARE_PRIVATE(QPickPointEvent)
};

typedef QSharedPointer<QPickPointEvent> QPickPointEventPtr;

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPICKPOINTEVENT_H
