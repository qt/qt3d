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

#include "assignkeyboardfocusjob_p.h"
#include "keyboardcontroller_p.h"
#include "keyboardinput_p.h"
#include "inputhandler_p.h"
#include "inputmanagers_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

AssignKeyboardFocusJob::AssignKeyboardFocusJob(const QNodeId &controller)
    : QAspectJob()
    , m_inputHandler(Q_NULLPTR)
    , m_keyboardController(controller)
{
}

void AssignKeyboardFocusJob::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void AssignKeyboardFocusJob::run()
{
    KeyboardController *controller = m_inputHandler->keyboardControllerManager()->lookupResource(m_keyboardController);
    Q_FOREACH (const HKeyboardInput handle, controller->keyboardInputsHandles()) {
        KeyboardInput *input = m_inputHandler->keyboardInputManager()->data(handle);
        if (input) {
            bool hasFocus = input->peerUuid() == controller->lastKeyboardInputRequester();
            input->setFocus(hasFocus);
            if (hasFocus)
                controller->setCurrentFocusItem(input->peerUuid());
        }
    }
}

} // Input

} // Qt3D

QT_END_NAMESPACE
