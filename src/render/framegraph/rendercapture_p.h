/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
******************************************************************************/

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
