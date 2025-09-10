// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "inputchord_p.h"

#include <Qt3DInput/qinputchord.h>

#include <Qt3DInput/private/qinputchord_p.h>
#include <Qt3DInput/private/inputhandler_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

InputChord::InputChord()
    : AbstractActionInput()
    , m_chords()
    , m_inputsToTrigger()
    , m_timeout(0)
    , m_startTime(0)
{
}

void InputChord::cleanup()
{
    setEnabled(false);
    m_timeout = 0;
    m_startTime = 0;
    m_chords.clear();
    m_inputsToTrigger.clear();
}

void InputChord::reset()
{
    m_startTime = 0;
    m_inputsToTrigger = m_chords;
}

bool InputChord::actionTriggered(Qt3DCore::QNodeId input)
{
    m_inputsToTrigger.removeOne(input);
    if (m_inputsToTrigger.isEmpty()) {
        //All Triggered
        reset();
        return true;
    }
    return false;
}

void InputChord::setStartTime(qint64 time)
{
    m_startTime = time;
}

void InputChord::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    AbstractActionInput::syncFromFrontEnd(frontEnd, firstTime);
    const QInputChord *node = qobject_cast<const QInputChord *>(frontEnd);
    if (!node)
        return;

    m_timeout = milliToNano(node->timeout());
    m_chords = Qt3DCore::qIdsForNodes(node->chords());
    m_inputsToTrigger = m_chords;
}

bool InputChord::process(InputHandler *inputHandler, qint64 currentTime)
{
    if (!isEnabled())
        return false;

    const qint64 startTime = m_startTime;
    bool triggered = false;
    int activeInputs = 0;
    for (const Qt3DCore::QNodeId &actionInputId : std::as_const(m_chords)) {
        AbstractActionInput *actionInput = inputHandler->lookupActionInput(actionInputId);
        if (actionInput && actionInput->process(inputHandler, currentTime)) {
            triggered |= actionTriggered(actionInputId);
            activeInputs++;
            if (startTime == 0)
                m_startTime = currentTime;
        }
    }

    if (startTime != 0) {
        // Check if we are still inside the time limit for the chord
        if ((currentTime - startTime) > m_timeout) {
            reset();
            if (activeInputs > 0)
                m_startTime = startTime;
            return false;
        }
    }

    return triggered;
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

