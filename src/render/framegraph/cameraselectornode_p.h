// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_CAMERASELECTOR_H
#define QT3DRENDER_RENDER_CAMERASELECTOR_H

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

#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QCameraSelector;

namespace Render {

class Renderer;

class Q_3DRENDERSHARED_PRIVATE_EXPORT CameraSelector : public FrameGraphNode
{
public:
    CameraSelector();

    Qt3DCore::QNodeId cameraUuid() const;

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    Qt3DCore::QNodeId m_cameraUuid;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_CAMERASELECTOR_H
