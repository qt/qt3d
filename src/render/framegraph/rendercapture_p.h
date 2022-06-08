// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef RENDERCAPTURE_P_H
#define RENDERCAPTURE_P_H

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

#include <Qt3DRender/private/qrendercapture_p.h>
#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT RenderCapture : public FrameGraphNode
{
public:
    RenderCapture();

    void requestCapture(const QRenderCaptureRequest &request);
    bool wasCaptureRequested() const;
    QRenderCaptureRequest takeCaptureRequest();
    void addRenderCapture(int captureId, const QImage &image);

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    void syncRenderCapturesToFrontend(Qt3DCore::QAspectManager *manager);

private:

    QList<QRenderCaptureRequest> m_requestedCaptures;
    QList<RenderCaptureDataPtr> m_renderCaptureData;
    mutable QMutex m_mutex;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // RENDERCAPTURE_P_H
