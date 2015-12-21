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

#include "actioninput_p.h"
#include <Qt3DInput/qactioninput.h>
#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

namespace {

QVector<int> listToIntArray(const QVariantList &l)
{
    QVector<int> array;
    array.reserve(l.size());
    Q_FOREACH (const QVariant &v, l)
        array.push_back(v.toInt());
    return array;
}

} // anonymous

ActionInput::ActionInput()
    : Qt3DCore::QBackendNode()
    , m_keys(0)
    , m_enabled(false)
{
}

void ActionInput::updateFromPeer(Qt3DCore::QNode *peer)
{
    QActionInput *input = static_cast<QActionInput *>(peer);
    m_enabled = input->isEnabled();
    m_keys = listToIntArray(input->keys());
    if (input->sourceDevice())
        m_sourceDevice = input->sourceDevice()->id();
}

void ActionInput::cleanup()
{
    m_enabled = false;
    m_sourceDevice = Qt3DCore::QNodeId();
    m_keys.clear();
}

void ActionInput::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::NodeUpdated) {
        Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("sourceDevice")) {
            m_sourceDevice = propertyChange->value().value<Qt3DCore::QNodeId>();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            m_enabled = propertyChange->value().toBool();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("keys")) {
            m_keys = listToIntArray(propertyChange->value().toList());
        }
    }
}

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

