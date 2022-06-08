// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "lightsource_p.h"
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/light_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

LightSource::LightSource() : entity(nullptr)
{
}

LightSource::LightSource(const Entity *entity, const std::vector<Light *> &lights)
    : entity(entity)
    , lights(lights)
{
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
