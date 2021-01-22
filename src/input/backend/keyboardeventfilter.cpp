/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "keyboardeventfilter_p.h"

#include <Qt3DInput/QKeyEvent>
#include <QtCore/QEvent>

#include <Qt3DInput/private/inputhandler_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

// The KeyboardEventFilter must be in the same thread as the view it will be monitoring

KeyboardEventFilter::KeyboardEventFilter(QObject *parent)
    : QObject(parent)
    , m_inputHandler(nullptr)
{
}

void KeyboardEventFilter::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

// Triggered in the view thread (usually the main thread)
bool KeyboardEventFilter::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj);
    if (e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease) {
        // Store event to be processed later on in an InputAspect job
        m_inputHandler->appendKeyEvent(QT_PREPEND_NAMESPACE(QKeyEvent)(*static_cast<QT_PREPEND_NAMESPACE(QKeyEvent) *>(e)));
    }
    return false;
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
