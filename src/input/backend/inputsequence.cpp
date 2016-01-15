/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "inputsequence_p.h"
#include <Qt3DInput/qinputsequence.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <QDateTime>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

InputSequence::InputSequence()
    : Qt3DCore::QBackendNode()
    , m_inputs()
    , m_inputsToTrigger()
    , m_timeout(0)
    , m_interval(0)
    , m_sequential(true)
    , m_startTime(0)
    , m_lastInputTime(0)
    , m_enabled(false)
{
}

void InputSequence::updateFromPeer(Qt3DCore::QNode *peer)
{
    QInputSequence *input = static_cast<QInputSequence *>(peer);
    m_enabled = input->isEnabled();
    m_timeout = input->timeout();
    m_interval = input->interval();
    m_sequential = input->sequential();
    Q_FOREACH (QAbstractActionInput *i, input->inputs())
        m_inputs.push_back(i->id());
}

void InputSequence::cleanup()
{
    m_enabled = false;
    m_timeout = 0;
    m_interval = 0;
    m_startTime = 0;
    m_lastInputTime = 0;
    m_lastInputId = Qt3DCore::QNodeId();
    m_sequential = true;
    m_inputs.clear();
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
    m_inputsToTrigger = m_inputs;
    m_lastInputId = Qt3DCore::QNodeId();
}

bool InputSequence::actionTriggered(Qt3DCore::QNodeId input, const qint64 currentTime)
{
    // If we are running the root of a sequence and input is not the first element of the sequence
    // reset and return false
    if (m_sequential && (!m_inputs.empty() && m_inputsToTrigger.first() != input)) {
        if (!(input == m_lastInputId && ((currentTime - m_lastInputTime) < 200)))
            reset();
        return false;
    }

    // Otherwise save the last input
    m_lastInputId = input;
    // Return false if we've spent too much time in between two sequences
    if ((m_lastInputTime != 0) && ((currentTime - m_lastInputTime) > m_interval)) {
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

void InputSequence::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
    if (e->type() == Qt3DCore::NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            m_enabled = propertyChange->value().toBool();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("timeout")) {
            m_timeout = propertyChange->value().toInt();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("interval")) {
            m_interval = propertyChange->value().toInt();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("sequential")) {
            m_sequential = propertyChange->value().toBool();
        }
    } else if (e->type() == Qt3DCore::NodeAdded) {
        if (propertyChange->propertyName() == QByteArrayLiteral("input")) {
            m_inputs.push_back(propertyChange->value().value<Qt3DCore::QNodeId>());
            m_inputsToTrigger.push_back(propertyChange->value().value<Qt3DCore::QNodeId>());
        }
    } else if (e->type() == Qt3DCore::NodeRemoved) {
        if (propertyChange->propertyName() == QByteArrayLiteral("input")) {
            m_inputs.removeOne(propertyChange->value().value<Qt3DCore::QNodeId>());
            m_inputsToTrigger.removeOne(propertyChange->value().value<Qt3DCore::QNodeId>());
        }
    }
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

