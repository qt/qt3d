// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "inputsequence_p.h"

#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qinputsequence.h>
#include <QtCore/QDateTime>

#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/qinputsequence_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

InputSequence::InputSequence()
    : AbstractActionInput()
    , m_sequences()
    , m_inputsToTrigger()
    , m_timeout(0)
    , m_buttonInterval(0)
    , m_startTime(0)
    , m_lastInputTime(0)
{
}

void InputSequence::cleanup()
{
    setEnabled(false);
    m_timeout = 0;
    m_buttonInterval = 0;
    m_startTime = 0;
    m_lastInputTime = 0;
    m_lastInputId = Qt3DCore::QNodeId();
    m_sequences.clear();
    m_inputsToTrigger.clear();
}

void InputSequence::setStartTime(qint64 time)
{
    m_startTime = time;
}

void InputSequence::reset()
{
    m_startTime = 0;
    m_lastInputTime = 0;
    m_inputsToTrigger = m_sequences;
    m_lastInputId = Qt3DCore::QNodeId();
}

bool InputSequence::actionTriggered(Qt3DCore::QNodeId input, const qint64 currentTime)
{
    if (input != m_inputsToTrigger.first())
        return false;

    // Save the last input
    m_lastInputId = input;
    // Return false if we've spent too much time in between two sequences
    if ((m_lastInputTime != 0) && ((currentTime - m_lastInputTime) > m_buttonInterval)) {
        reset();
        return false;
    }

    // Update lastInputTime and remove the inputs to trigger from the sequence
    m_lastInputTime = currentTime;
    m_inputsToTrigger.removeOne(input);

    // If we have no more remaining inputs in the sequences of inputs
    if (m_inputsToTrigger.isEmpty()) {
        // All Triggered
        reset();
        return true;
    }
    return false;
}

void InputSequence::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    AbstractActionInput::syncFromFrontEnd(frontEnd, firstTime);
    const QInputSequence *node = qobject_cast<const QInputSequence *>(frontEnd);
    if (!node)
        return;

    m_timeout = milliToNano(node->timeout());
    m_buttonInterval = milliToNano(node->buttonInterval());
    m_sequences = Qt3DCore::qIdsForNodes(node->sequences());
    m_inputsToTrigger = m_sequences;
}

bool InputSequence::process(InputHandler *inputHandler, qint64 currentTime)
{
    if (!isEnabled())
        return false;

    if (m_startTime != 0) {
        // Check if we are still inside the time limit for the sequence
        if ((currentTime - m_startTime) > m_timeout) {
            reset();
            return false;
        }
    }

    bool triggered = false;
    for (const Qt3DCore::QNodeId &actionInputId : std::as_const(m_sequences)) {
        AbstractActionInput *actionInput = inputHandler->lookupActionInput(actionInputId);
        if (actionInput && actionInput->process(inputHandler, currentTime)) {
            triggered |= actionTriggered(actionInputId, currentTime);
            if (m_startTime == 0)
                m_startTime = currentTime;
        }
    }
    return triggered;
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

