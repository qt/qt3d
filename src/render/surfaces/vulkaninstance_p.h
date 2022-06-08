// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_VULKANINSTANCE_P_H
#define QT3DRENDER_VULKANINSTANCE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/qtguiglobal.h>
#include <Qt3DRender/qt3drender-config.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

#if QT_CONFIG(qt3d_vulkan)
class QVulkanInstance;
namespace Qt3DRender {
Q_3DRENDERSHARED_PRIVATE_EXPORT
QVulkanInstance& staticVulkanInstance() noexcept;
} // Qt3DRender
#endif

QT_END_NAMESPACE

#endif // QT3DRENDER_VULKANINSTANCE_P_H
