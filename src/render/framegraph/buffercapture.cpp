/****************************************************************************
**
** Copyright (C) 2017 Juan José Casafranca
** Contact: https://www.qt.io/licensing/
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
**
**
**
****************************************************************************/


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
