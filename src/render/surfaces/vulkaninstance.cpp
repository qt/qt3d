// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "vulkaninstance_p.h"

#if QT_CONFIG(qt3d_vulkan)

#include <QVulkanInstance>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QVulkanInstance &staticVulkanInstance() noexcept
{
    static QVulkanInstance* vkInstance = []
    {
        QVulkanInstance* v = new QVulkanInstance;
#if defined(NDEBUG)
        constexpr bool debug_mode = false;
#else
        constexpr bool debug_mode = true;
#endif
        if (debug_mode || qgetenv("QT3D_VULKAN_VALIDATION").toInt())
            v->setLayers({ "VK_LAYER_KHRONOS_validation" });

        if (!v->create())
            qWarning("Failed to create Vulkan instance");

        return v;
    }();
    return *vkInstance;
}

} // Qt3DRender

QT_END_NAMESPACE

#endif
