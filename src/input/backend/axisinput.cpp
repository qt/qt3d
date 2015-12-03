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

#include "axisinput_p.h"
#include <Qt3DInput/qaxisinput.h>
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

AxisInput::AxisInput()
    : Qt3DCore::QBackendNode()
    , m_axis(0)
    , m_scale(0.0f)
    , m_enabled(false)
{
}

// AxisInput can have two types of behavior
// Axis input with source device and axis -> will retrieve the axis value from the source device
// Axis input with source device and scale + keys -> will be used as a multiplication factor with other AxisInput
void AxisInput::updateFromPeer(Qt3DCore::QNode *peer)
{
    QAxisInput *input = static_cast<QAxisInput *>(peer);
    m_axis = input->axis();
    m_scale = input->scale();
    m_enabled = input->isEnabled();
    m_keys = listToIntArray(input->keys());
    if (input->sourceDevice())
        m_sourceDevice = input->sourceDevice()->id();
}

void AxisInput::cleanup()
{
    m_axis = 0;
    m_enabled = false;
    m_scale = 0.0f;
    m_keys.clear();
    m_sourceDevice = Qt3DCore::QNodeId();
}

void AxisInput::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::NodeUpdated) {
        Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("sourceDevice")) {
            m_sourceDevice = propertyChange->value().value<Qt3DCore::QNodeId>();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("scale")) {
            m_scale = propertyChange->value().toFloat();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("axis")) {
            m_axis = propertyChange->value().toInt();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("enabled")) {
            m_enabled = propertyChange->value().toBool();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("keys")) {
            m_keys = listToIntArray(propertyChange->value().toList());
        }
    }
}

} // Input

} // Qt3DInput

QT_END_NAMESPACE
