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

#include "mouseeventfilter_p.h"

#include <Qt3DInput/QKeyEvent>
#include <QtCore/QEvent>

#include <Qt3DInput/private/inputhandler_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

// The MouseEventFilter must be in the same thread as the view it will be monitoring

MouseEventFilter::MouseEventFilter(QObject *parent)
    : QObject(parent)
    , m_inputHandler(nullptr)
{
}

void MouseEventFilter::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

// Triggered in the view thread (usually the main thread)
bool MouseEventFilter::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj);
    switch (e->type()) {
    // When dealing with QtQuick events, there are nove MouseMove events
    // only hover events which are transmitted if hoverEnabled was set to true on the Scene3D
    case QEvent::HoverMove:
    {
        const QHoverEvent *event = static_cast<QHoverEvent *>(e);
        m_inputHandler->appendMouseEvent(QMouseEvent(QEvent::MouseMove, event->posF(), Qt::NoButton,  Qt::NoButton, event->modifiers()));
        break;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        // Creates copy and store event to be processed later on in an InputAspect job
        m_inputHandler->appendMouseEvent(QMouseEvent(*static_cast<QMouseEvent *>(e)));
        break;
#if QT_CONFIG(wheelevent)
    case QEvent::Wheel:
        // Creates copy and store event to be processed later on in an InputAspect job
        m_inputHandler->appendWheelEvent(QWheelEvent(*static_cast<QWheelEvent *>(e)));
        break;
#endif
    default:
        break;
    }

    return false;
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
