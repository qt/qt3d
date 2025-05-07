// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QT3DRENDER_QRENDERAPI_H
#define QT3DRENDER_QRENDERAPI_H

#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

enum class API {
    OpenGL,
    Vulkan,
    DirectX,
    Metal,
    RHI,
    Null
};

} // namespace Qt3Drender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERAPI_H
