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
******************************************************************************/

#ifndef QT3DRENDER_QPICKTRIANGLEEVENT_H
#define QT3DRENDER_QPICKTRIANGLEEVENT_H

#include <Qt3DRender/qpickevent.h>
#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QPickTriangleEventPrivate;

class Q_3DRENDERSHARED_EXPORT QPickTriangleEvent : public QPickEvent
{
    Q_OBJECT
    Q_PROPERTY(uint triangleIndex READ triangleIndex CONSTANT)
    Q_PROPERTY(uint vertex1Index READ vertex1Index CONSTANT)
    Q_PROPERTY(uint vertex2Index READ vertex2Index CONSTANT)
    Q_PROPERTY(uint vertex3Index READ vertex3Index CONSTANT)
    Q_PROPERTY(QVector3D uvw READ uvw CONSTANT)
public:
    QPickTriangleEvent();
    QPickTriangleEvent(const QPointF &position, const QVector3D& worldIntersection, const QVector3D& localIntersection, float distance,
                       uint triangleIndex, uint vertex1Index, uint vertex2Index, uint vertex3Index);
    QPickTriangleEvent(const QPointF &position, const QVector3D& worldIntersection, const QVector3D& localIntersection, float distance,
                       uint triangleIndex, uint vertex1Index, uint vertex2Index, uint vertex3Index, Buttons button, int buttons, int modifiers,
                       const QVector3D &uvw);
    ~QPickTriangleEvent();

public:
    uint triangleIndex() const;
    uint vertex1Index() const;
    uint vertex2Index() const;
    uint vertex3Index() const;
    QVector3D uvw() const;

private:
    Q_DECLARE_PRIVATE(QPickTriangleEvent)
};

typedef QSharedPointer<QPickTriangleEvent> QPickTriangleEventPtr;

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPICKTRIANGLEEVENT_H
