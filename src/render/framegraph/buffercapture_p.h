// Copyright (C) 2017 Juan José Casafranca
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef BUFFERCAPTURE_P_H
#define BUFFERCAPTURE_P_H

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

#include <Qt3DRender/private/qbuffercapture_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT BufferCapture : public FrameGraphNode
{
public:
    BufferCapture();
};

} //Render

} //Qt3DRender

QT_END_NAMESPACE

#endif // BUFFERCAPTURE_P_H
