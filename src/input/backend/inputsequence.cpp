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

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

InputSequence::InputSequence()
    : Qt3DCore::QBackendNode()
    , m_inputs()
    , m_timeout(0)
    , m_interval(0)
    , m_sequential(true)
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
    m_sequential = true;
    m_inputs.clear();
}

void InputSequence::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::NodeUpdated) {
        Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            m_enabled = propertyChange->value().toBool();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("timeout")) {
            m_timeout = propertyChange->value().toInt();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("interval")) {
            m_interval = propertyChange->value().toInt();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("sequential")) {
            m_sequential = propertyChange->value().toBool();
        }
    }
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

