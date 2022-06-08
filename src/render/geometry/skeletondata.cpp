// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "skeletondata_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

SkeletonData::SkeletonData()
{
}

void SkeletonData::reserve(int size)
{
    joints.reserve(size);
    localPoses.reserve(size);
    jointNames.reserve(size);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
