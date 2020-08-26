/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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

#include "coresettings_p.h"
#include <Qt3DCore/private/qcoreaspect_p.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

CoreSettings::CoreSettings()
    : QBackendNode()
{
}

void CoreSettings::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    Q_UNUSED(firstTime);

    const QCoreSettings *node = qobject_cast<const QCoreSettings *>(frontEnd);
    if (!node)
        return;

    Q_ASSERT(m_aspect);
    auto daspect = QCoreAspectPrivate::get(m_aspect);
    daspect->m_boundingVolumesEnabled = node->boundingVolumesEnabled();
}

CoreSettingsFunctor::CoreSettingsFunctor(QCoreAspect *aspect)
    : m_aspect(aspect)
    , m_settings(nullptr)
{
}

Qt3DCore::QBackendNode *CoreSettingsFunctor::create(Qt3DCore::QNodeId) const
{
    if (m_settings != nullptr) {
        qWarning() << "Core settings already exists";
        return nullptr;
    }

    m_settings = new CoreSettings;
    m_settings->setAspect(m_aspect);
    return m_settings;
}

Qt3DCore::QBackendNode *CoreSettingsFunctor::get(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
    return m_settings;
}

void CoreSettingsFunctor::destroy(Qt3DCore::QNodeId id) const
{
    Q_UNUSED(id);
    delete m_settings;
    m_settings = nullptr;
}

QT_END_NAMESPACE
