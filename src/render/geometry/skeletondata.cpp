/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
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
****************************************************************************/

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
