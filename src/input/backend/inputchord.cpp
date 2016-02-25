/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "inputchord_p.h"
#include <Qt3DInput/qinputchord.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

InputChord::InputChord()
    : Qt3DCore::QBackendNode()
    , m_inputs()
    , m_inputsToTrigger()
    , m_timeout(0)
    , m_startTime(0)
    , m_enabled(false)
{
}

void InputChord::updateFromPeer(Qt3DCore::QNode *peer)
{
    QInputChord *input = static_cast<QInputChord *>(peer);
    m_enabled = input->isEnabled();
    m_timeout = input->timeout();
    Q_FOREACH (QAbstractActionInput *i, input->inputs())
        m_inputs.push_back(i->id());
}

void InputChord::cleanup()
{
    m_enabled = false;
    m_timeout = 0;
    m_startTime = 0;
    m_inputs.clear();
    m_inputsToTrigger.clear();
}

void InputChord::reset()
{
    m_startTime = 0;
    m_inputsToTrigger.clear();
    Q_FOREACH (Qt3DCore::QNodeId input, m_inputs)
        m_inputsToTrigger.push_back(input);
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

void InputChord::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
    if (e->type() == Qt3DCore::NodeUpdated) {
        if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            m_enabled = propertyChange->value().toBool();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("timeout")) {
            m_timeout = propertyChange->value().toInt();
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

