/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qpickpointevent.h"
#include "qpickevent_p.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPickPointEventPrivate : public QPickEventPrivate
{
public:
    QPickPointEventPrivate()
        : QPickEventPrivate()
        , m_pointIndex(0)
    {
    }

    uint m_pointIndex;
};

/*!
    \class Qt3DRender::QPickPointEvent
    \inmodule Qt3DRender

    \brief The QPickPointEvent class holds information when a segment of a point cloud is picked.

    \sa QPickEvent
    \since 5.10
*/

/*!
 * \qmltype PickPointEvent
 * \instantiates Qt3DRender::QPickPointEvent
 * \inqmlmodule Qt3D.Render
 * \brief PickPointEvent holds information when a segment of a point cloud is picked.
 * \sa ObjectPicker
 */


/*!
  \fn Qt3DRender::QPickPointEvent::QPickPointEvent()
  Constructs a new QPickPointEvent.
 */
QPickPointEvent::QPickPointEvent()
    : QPickEvent(*new QPickPointEventPrivate())
{
}

QPickPointEvent::QPickPointEvent(const QPointF &position, const QVector3D &worldIntersection,
                                 const QVector3D &localIntersection, float distance,
                                 uint pointIndex,
                                 QPickEvent::Buttons button, int buttons, int modifiers)
    : QPickEvent(*new QPickPointEventPrivate())
{
    Q_D(QPickPointEvent);
    d->m_position = position;
    d->m_distance = distance;
    d->m_worldIntersection = worldIntersection;
    d->m_localIntersection = localIntersection;
    d->m_pointIndex = pointIndex;
    d->m_button = button;
    d->m_buttons = buttons;
    d->m_modifiers = modifiers;
}

/*! \internal */
QPickPointEvent::~QPickPointEvent()
{
}

/*!
    \qmlproperty uint Qt3D.Render::PickPointEvent::pointIndex
    Specifies the index of the point that was picked
*/
/*!
  \property Qt3DRender::QPickPointEvent::pointIndex
    Specifies the index of the point that was picked
 */
/*!
 * \brief QPickPointEvent::pointIndex
 * Returns the index of the picked point
 */
uint QPickPointEvent::pointIndex() const
{
    Q_D(const QPickPointEvent);
    return d->m_pointIndex;
}

} // Qt3DRender

QT_END_NAMESPACE

