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

#include "axissetting_p.h"
#include <Qt3DInput/qaxissetting.h>
#include <Qt3DCore/qscenepropertychange.h>

QT_BEGIN_NAMESPACE

namespace {

QVector<int> variantListToVector(const QVariantList &list)
{
    QVector<int> v(list.size());
    int i = 0;
    Q_FOREACH (const QVariant &e, list) {
        v[i++] = e.toInt();
    }
    return v;
}

}

namespace Qt3DInput {
namespace Input {

AxisSetting::AxisSetting()
    : Qt3DCore::QBackendNode()
    , m_deadZone(0.0f)
    , m_axes(0)
    , m_filter(false)
{
}

void AxisSetting::updateFromPeer(Qt3DCore::QNode *peer)
{
    QAxisSetting *setting = static_cast<QAxisSetting *>(peer);
    m_deadZone = setting->deadZone();
    m_axes = variantListToVector(setting->axes());
    m_filter = setting->isFilterEnabled();
}

void AxisSetting::cleanup()
{
    m_deadZone = 0.0f;
    m_axes.clear();
    m_filter = false;
}

void AxisSetting::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
    if (e->type() == Qt3DCore::NodeUpdated) {
        Qt3DCore::QScenePropertyChangePtr propertyChange = qSharedPointerCast<Qt3DCore::QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("deadZone")) {
            m_deadZone = propertyChange->value().toFloat();
        } else if (propertyChange->propertyName() == QByteArrayLiteral("axes")) {
            m_axes = variantListToVector(propertyChange->value().toList());
        } else if (propertyChange->propertyName() == QByteArrayLiteral("filter")) {
            m_filter = propertyChange->value().toBool();
        }
    }
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
