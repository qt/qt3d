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
****************************************************************************/

#include "vulkaninstance_p.h"
#include <QVulkanInstance>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QVulkanInstance &staticVulkanInstance() noexcept
{
    static QVulkanInstance* vkInstance = []
    {
        QVulkanInstance* v = new QVulkanInstance;
#ifndef Q_OS_ANDROID
        v->setLayers({ "VK_LAYER_LUNARG_standard_validation" });
#else
        v->setLayers(QByteArrayList()
                               << "VK_LAYER_GOOGLE_threading"
                               << "VK_LAYER_LUNARG_parameter_validation"
                               << "VK_LAYER_LUNARG_object_tracker"
                               << "VK_LAYER_LUNARG_core_validation"
                               << "VK_LAYER_LUNARG_image"
                               << "VK_LAYER_LUNARG_swapchain"
                               << "VK_LAYER_GOOGLE_unique_objects");
#endif

        if (!v->create()) {
            qWarning("Failed to create Vulkan instance");
        }
        return v;
    }();
    return *vkInstance;
}

} // Qt3DRender

QT_END_NAMESPACE
