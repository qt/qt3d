/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "keyeventdispatcherjob_p.h"
#include "inputhandler_p.h"
#include "keyboardinput_p.h"
#include "inputmanagers_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

KeyEventDispatcherJob::KeyEventDispatcherJob(const QNodeId &input, const QList<QKeyEvent> &events)
    : QAspectJob()
    , m_inputHandler(Q_NULLPTR)
    , m_keyboardInput(input)
    , m_events(events)
{
}

void KeyEventDispatcherJob::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void KeyEventDispatcherJob::run()
{
    KeyboardInput *input = m_inputHandler->keyboardInputManager()->lookupResource(m_keyboardInput);
    if (input)
        Q_FOREACH (const QKeyEvent &e, m_events) {
            // Send events to frontend
            input->keyEvent(Q3DKeyEventPtr(new Q3DKeyEvent(e)));
        }
}

} // Input

} // Qt3D

QT_END_NAMESPACE
