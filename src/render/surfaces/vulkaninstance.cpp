/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
