// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LIGHTSOURCE_P_H
#define QT3DRENDER_RENDER_LIGHTSOURCE_P_H

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

#include <Qt3DRender/private/qt3drender_global_p.h>
#include <vector>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Entity;
class Light;

struct Q_3DRENDERSHARED_PRIVATE_EXPORT LightSource {
    LightSource();
    LightSource(const Entity *entity, const std::vector<Light *> &lights);

    const Entity *entity;
    std::vector<Light *> lights;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_LIGHTSOURCE_P_H
