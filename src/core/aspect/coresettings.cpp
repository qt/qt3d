// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "coresettings_p.h"
#include <Qt3DCore/private/qcoreaspect_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

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

} // namespace Qt3DCore

QT_END_NAMESPACE
