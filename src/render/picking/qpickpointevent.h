// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
