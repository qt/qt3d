// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QOBJECTPICKER_P_H
#define QT3DRENDER_QOBJECTPICKER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/private/objectpicker_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \internal
*/
class Q_3DRENDERSHARED_PRIVATE_EXPORT QObjectPickerPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QObjectPickerPrivate()
        : QComponentPrivate()
        , m_hoverEnabled(false)
        , m_dragEnabled(false)
        , m_pressed(false)
        , m_containsMouse(false)
        , m_acceptedLastPressedEvent(true)
        , m_priority(0)
    {
        m_shareable = false;
    }

    Q_DECLARE_PUBLIC(QObjectPicker)
    bool m_hoverEnabled;
    bool m_dragEnabled;
    bool m_pressed;
    bool m_containsMouse;
    bool m_acceptedLastPressedEvent;
    int m_priority;

    enum EventType {
        Pressed,
        Released,
        Clicked,
        Moved
    };

    void propagateEvent(QPickEvent *event, EventType type);

    void pressedEvent(QPickEvent *event);
    void clickedEvent(QPickEvent *event);
    void movedEvent(QPickEvent *event);
    void releasedEvent(QPickEvent *event);

    void setPressed(bool pressed);
    void setContainsMouse(bool containsMouse);
};

struct QObjectPickerEvent
{
    QPickEventPtr event;
    Qt3DCore::QNodeId viewportNodeId;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QObjectPickerEvent);

#endif // QT3DRENDER_QOBJECTPICKER_P_H
