// Copyright (C) 2017 Juan José Casafranca
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only


#include <Qt3DRender/private/qbuffercapture_p.h>
#include <Qt3DRender/private/buffercapture_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

BufferCapture::BufferCapture()
    : FrameGraphNode(FrameGraphNode::BufferCapture, QBackendNode::ReadWrite)
{

}

} //Render

} //Qt3DRender

QT_END_NAMESPACE
