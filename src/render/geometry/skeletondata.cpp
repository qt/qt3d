// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

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
